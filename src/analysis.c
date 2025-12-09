#include "analysis.h"
#include "encoding.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Generador de bitstream fijo de 1000 bits (multiplo de 4 para 4B/5B)
void get_fixed_bitstream(char *buffer) {
    const char *pattern = "10110010";  // 8 bits
    buffer[0] = '\0';
    for (int i = 0; i < 125; i++) {  // 125 * 8 = 1000 bits
        strcat(buffer, pattern);
    }
}

// Cuenta errores entre dos bitstreams
int count_bit_errors(const char *original, const char *decoded) {
    if (!original || !decoded) return -1;
    
    int errors = 0;
    size_t len = strlen(original);
    size_t dec_len = strlen(decoded);
    size_t min_len = (len < dec_len) ? len : dec_len;
    
    for (size_t i = 0; i < min_len; i++) {
        if (original[i] != decoded[i]) {
            errors++;
        }
    }
    
    // Contar diferencia de longitud como errores
    errors += abs((int)len - (int)dec_len);
    return errors;
}

// Estadisticas simples
typedef struct {
    double min;
    double max;
    double sum;
    int count;
} Stats;

void update_stats(Stats *s, int errors) {
    if (s->count == 0 || errors < s->min) s->min = errors;
    if (s->count == 0 || errors > s->max) s->max = errors;
    s->sum += errors;
    s->count++;
}

void print_stats_row(const char *name, Stats *s) {
    double mean = s->sum / s->count;
    printf("| %-10s | %8.2f | %6.0f | %6.0f |\n", name, mean, s->min, s->max);
}

// ============================================
// 1. Overhead de codificación
// ============================================

void run_analysis_overhead(void) {
    printf("\n=== 5. Overhead de Codificación ===\n");
    char bitstream[1001];
    get_fixed_bitstream(bitstream);
    
    char *nrz = encode_nrz(bitstream);
    char *nrzi = encode_nrzi(bitstream);
    char *manchester = encode_manchester(bitstream);
    char *code_4b5b = encode_4b5b(bitstream);
    char *code_custom = encode_custom(bitstream);
    
    printf("| Esquema    | Bits Útiles | Bits Transmitidos | Overhead  |\n");
    printf("|------------|-------------|-------------------|-----------||\n");
    printf("| NRZ        | 1000        | %-17zu | %8.1f%% |\n", strlen(nrz), (double)(strlen(nrz)-1000)/1000*100);
    printf("| NRZI       | 1000        | %-17zu | %8.1f%% |\n", strlen(nrzi), (double)(strlen(nrzi)-1000)/1000*100);
    printf("| Manchester | 1000        | %-17zu | %8.1f%% |\n", strlen(manchester), (double)(strlen(manchester)-1000)/1000*100);
    printf("| 4B/5B      | 1000        | %-17zu | %8.1f%% |\n", strlen(code_4b5b), (double)(strlen(code_4b5b)-1000)/1000*100);
    printf("| ZR-Code    | 1000        | %-17zu | %8.1f%% |\n", strlen(code_custom), (double)(strlen(code_custom)-1000)/1000*100);
    
    free(nrz); free(nrzi); free(manchester); free(code_4b5b); free(code_custom);
}

// ============================================
// 2. Análisis estadístico de errores
// ============================================

void run_analysis_error_stats(double ber) {
    printf("\n=== 6. Análisis Estadístico de Errores (BER=%.2f, N=100) ===\n", ber);
    
    Stats s_nrz = {10000, 0, 0, 0};
    Stats s_nrzi = {10000, 0, 0, 0};
    Stats s_man = {10000, 0, 0, 0};
    Stats s_4b5b = {10000, 0, 0, 0};
    Stats s_custom = {10000, 0, 0, 0};
    
    char bitstream[1001];
    get_fixed_bitstream(bitstream);
    
    for (int i = 0; i < 100; i++) {
        char bitstream_copy[1001];
        strcpy(bitstream_copy, bitstream);
        add_noise(bitstream_copy, ber);
        
        // NRZ
        char *enc = encode_nrz(bitstream_copy);
        char *dec = decode_nrz(enc);
        update_stats(&s_nrz, count_bit_errors(bitstream, dec));
        free(enc); free(dec);
        
        // NRZI
        enc = encode_nrzi(bitstream_copy);
        dec = decode_nrzi(enc);
        update_stats(&s_nrzi, count_bit_errors(bitstream, dec));
        free(enc); free(dec);
        
        // Manchester
        enc = encode_manchester(bitstream_copy);
        dec = decode_manchester(enc);
        if (dec) {
            update_stats(&s_man, count_bit_errors(bitstream, dec));
            free(dec);
        } else {
            update_stats(&s_man, 1000);  // Fallo total
        }
        free(enc);
        
        // 4B/5B
        enc = encode_4b5b(bitstream_copy);
        dec = decode_4b5b(enc);
        if (dec) {
            update_stats(&s_4b5b, count_bit_errors(bitstream, dec));
            free(dec);
        } else {
            update_stats(&s_4b5b, 1000);
        }
        free(enc);

        // ZR-Code
        enc = encode_custom(bitstream_copy);
        dec = decode_custom(enc);
        if (dec) {
            update_stats(&s_custom, count_bit_errors(bitstream, dec));
            free(dec);
        } else {
            update_stats(&s_custom, 1000);
        }
        free(enc);
    }
    
    printf("| Esquema    | Media    | Mínimo | Máximo |\n");
    printf("|------------|----------|--------|--------|\n");
    print_stats_row("NRZ", &s_nrz);
    print_stats_row("NRZI", &s_nrzi);
    print_stats_row("Manchester", &s_man);
    print_stats_row("4B/5B", &s_4b5b);
    print_stats_row("ZR-Code", &s_custom);
}

// ============================================
// 3. Curva BER vs Tasa de Error Efectiva
// ============================================

void run_analysis_ber_curve(void) {
    printf("\n=== 7. Curva BER vs Tasa de Error Efectiva ===\n");
    double bers[] = {0.001, 0.005, 0.01, 0.02, 0.05, 0.1};
    
    printf("| BER In | NRZ Out | NRZI Out | Manch Out | 4B/5B Out | ZR-Code   |\n");
    printf("|--------|---------|----------|-----------|-----------|-----------||\n");
    
    // Bitstream de 10000 bits
    char bitstream[10001];
    bitstream[0] = '\0';
    char chunk[1001];
    get_fixed_bitstream(chunk);
    for(int i=0; i<10; i++) strcat(bitstream, chunk);
    
    for (int i = 0; i < 6; i++) {
        double current_ber = bers[i];
        
        char bitstream_copy[10001];
        strcpy(bitstream_copy, bitstream);
        add_noise(bitstream_copy, current_ber);
        
        // NRZ
        char *enc = encode_nrz(bitstream_copy);
        char *dec = decode_nrz(enc);
        double err_nrz = (double)count_bit_errors(bitstream, dec) / 10000.0;
        free(enc); free(dec);
        
        // NRZI
        enc = encode_nrzi(bitstream_copy);
        dec = decode_nrzi(enc);
        double err_nrzi = (double)count_bit_errors(bitstream, dec) / 10000.0;
        free(enc); free(dec);

        // Manchester
        enc = encode_manchester(bitstream_copy);
        dec = decode_manchester(enc);
        double err_man = dec ? (double)count_bit_errors(bitstream, dec) / 10000.0 : 1.0;
        free(enc); if(dec) free(dec);

        // 4B/5B
        enc = encode_4b5b(bitstream_copy);
        dec = decode_4b5b(enc);
        double err_4b5b = dec ? (double)count_bit_errors(bitstream, dec) / 10000.0 : 1.0;
        free(enc); if(dec) free(dec);

        // ZR-Code
        enc = encode_custom(bitstream_copy);
        dec = decode_custom(enc);
        double err_custom = dec ? (double)count_bit_errors(bitstream, dec) / 10000.0 : 1.0;
        free(enc); if(dec) free(dec);
        
        printf("| %.3f  | %.4f  | %.4f   | %.4f    | %.4f      | %.4f     |\n", 
               current_ber, err_nrz, err_nrzi, err_man, err_4b5b, err_custom);
    }
}

// ============================================
// 4. Análisis de ráfagas de errores
// ============================================

void add_burst_noise(char *bitstream, int burst_len, int bursts_count) {
    size_t len = strlen(bitstream);
    for(int k=0; k<bursts_count; k++) {
        int start = rand() % (len - burst_len);
        for(int i=0; i<burst_len; i++) {
            bitstream[start+i] = (bitstream[start+i] == '0') ? '1' : '0';
        }
    }
}

void run_analysis_burst_noise(void) {
    printf("\n=== 8. Análisis de Ráfagas de Errores ===\n");
    printf("Simulando 5 ráfagas de 10 bits en mensaje de 1000 bits\n\n");
    
    char bitstream[1001];
    get_fixed_bitstream(bitstream);
    
    char bitstream_copy[1001];
    
    // NRZ
    strcpy(bitstream_copy, bitstream);
    add_burst_noise(bitstream_copy, 10, 5);
    char *enc = encode_nrz(bitstream_copy);
    char *dec = decode_nrz(enc);
    int err_nrz = count_bit_errors(bitstream, dec);
    free(enc); free(dec);

    // NRZI
    strcpy(bitstream_copy, bitstream);
    add_burst_noise(bitstream_copy, 10, 5);
    enc = encode_nrzi(bitstream_copy);
    dec = decode_nrzi(enc);
    int err_nrzi = count_bit_errors(bitstream, dec);
    free(enc); free(dec);
    
    // Manchester
    strcpy(bitstream_copy, bitstream);
    add_burst_noise(bitstream_copy, 10, 5);
    enc = encode_manchester(bitstream_copy);
    dec = decode_manchester(enc);
    int err_man = dec ? count_bit_errors(bitstream, dec) : 1000;
    free(enc); if(dec) free(dec);
    
    // 4B/5B
    strcpy(bitstream_copy, bitstream);
    add_burst_noise(bitstream_copy, 10, 5);
    enc = encode_4b5b(bitstream_copy);
    dec = decode_4b5b(enc);
    int err_4b5b = dec ? count_bit_errors(bitstream, dec) : 1000;
    free(enc); if(dec) free(dec);

    // ZR-Code
    strcpy(bitstream_copy, bitstream);
    add_burst_noise(bitstream_copy, 10, 5);
    enc = encode_custom(bitstream_copy);
    dec = decode_custom(enc);
    int err_custom = dec ? count_bit_errors(bitstream, dec) : 1000;
    free(enc); if(dec) free(dec);
    
    printf("| Esquema    | Errores Resultantes |\n");
    printf("|------------|---------------------||\n");
    printf("| NRZ        | %-19d |\n", err_nrz);
    printf("| NRZI       | %-19d |\n", err_nrzi);
    printf("| Manchester | %-19d |\n", err_man);
    printf("| 4B/5B      | %-19d |\n", err_4b5b);
    printf("| ZR-Code    | %-19d |\n", err_custom);
}

// ============================================
// Función principal que ejecuta todos los análisis
// ============================================

void run_all_analysis(double ber) {
    printf("\n");
    printf("=================================================================\n");
    printf("  PARTE B: ANÁLISIS CUANTITATIVO\n");
    printf("=================================================================\n");
    printf("Alumno: Zain Rondon\n");
    printf("CI: 30.601.248\n");
    printf("BER: %.2f\n", ber);
    printf("=================================================================\n");
    
    run_analysis_overhead();
    run_analysis_error_stats(ber);
    run_analysis_ber_curve();
    run_analysis_burst_noise();
    
    printf("\n=================================================================\n");
    printf("  ANÁLISIS COMPLETADO\n");
    printf("=================================================================\n\n");
}
