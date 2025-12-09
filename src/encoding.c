#include "encoding.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================
// NRZ (Non-Return to Zero)
// ============================================

char *encode_nrz(const char *bitstream)
{
    size_t len = strlen(bitstream);

    char *encoded = malloc_string(bitstream, len, "encode_nrz");

    if (encoded == NULL)
        return NULL;

    for (size_t i = 0; i < len; i++)
    {
        if (bitstream[i] == '1')
            encoded[i] = HIGH;

        else if (bitstream[i] == '0')
            encoded[i] = LOW;

        else
        {

            fprintf(stderr, "Error: Caracter no válido en el bitstream: %c\n", bitstream[i]);
            free(encoded);
            return NULL;
        }
    }

    encoded[len] = '\0';

    return encoded;
}

char *decode_nrz(const char *encoded)
{
    if (encoded == NULL)
        return NULL;

    size_t len = strlen(encoded);

    char *bitstream = malloc_string(encoded, len, "decode_nrz");

    if (bitstream == NULL)
    {
        perror("Fallo en la asignación de memoria para decode_nrz");
        return NULL;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (encoded[i] == HIGH)
            bitstream[i] = '1';

        else if (encoded[i] == LOW)
            bitstream[i] = '0';

        else
        {

            fprintf(stderr, "Error: Caracter no válido en la codificación NRZ: %c\n", encoded[i]);
            free(bitstream);
            return NULL;
        }
    }

    bitstream[len] = '\0'; // Asegura el terminador nulo
    return bitstream;
}

// ============================================
// NRZ (Non-Return to Zero Inverted)
// ============================================

char *encode_nrzi(const char *bitstream)
{

    if (bitstream == NULL)
    {
        return NULL;
    }

    size_t len = strlen(bitstream);

    char *encoded = malloc_string(bitstream, len, "encode_nrzi");

    if (encoded == NULL)
    {
        perror("Fallo en la asignación de memoria para encode_nrzi");
        return NULL;
    }

    char previous_level = LOW;

    for (size_t i = 0; i < len; i++)
    {
        char current_bit = bitstream[i];

        if (current_bit == '1')
        {

            if (previous_level == HIGH)
            {
                encoded[i] = LOW;
                previous_level = LOW;
            }
            else
            {
                encoded[i] = HIGH;
                previous_level = HIGH;
            }
        }
        else if (current_bit == '0')
        {

            encoded[i] = previous_level;
        }
        else
        {

            fprintf(stderr, "Error: Caracter no válido en el bitstream: %c\n", current_bit);
            free(encoded);
            return NULL;
        }
    }

    encoded[len] = '\0';
    return encoded;
}

char *decode_nrzi(const char *encoded)
{
    if (encoded == NULL)
    {
        return NULL;
    }

    size_t len = strlen(encoded);

    if (len == 0)
    {
        char *empty = (char *)malloc(1);
        if (empty)
            empty[0] = '\0';
        return empty;
    }

    // Asigna memoria para la cadena decodificada (bitstream) + terminador nulo
    char *bitstream = malloc_string(encoded, len, "decode_nrzi");

    if (bitstream == NULL)
    {
        perror("Fallo en la asignación de memoria para decode_nrzi");
        return NULL;
    }

    char previous_level = LOW;

    for (size_t i = 0; i < len; i++)
    {
        char current_level = encoded[i];

        if (current_level != previous_level)
        {
            // Hubo una TRANSICIÓN -> El bit es '1'
            bitstream[i] = '1';
        }
        else
        {
            // NO hubo transición -> El bit es '0'
            bitstream[i] = '0';
        }

        // Actualizamos el nivel anterior para la siguiente iteración
        previous_level = current_level;

        if (current_level != HIGH && current_level != LOW)
        {
            // Manejo de error para caracteres no válidos
            fprintf(stderr, "Error: Caracter no válido en la codificación NRZ-I: %c\n", current_level);
            free(bitstream);
            return NULL;
        }
    }

    bitstream[len] = '\0'; // Terminador nulo
    return bitstream;
}

// ============================================
// Manchester
// ============================================

char *encode_manchester(const char *bitstream)
{
    if (bitstream == NULL)
        return NULL;

    size_t len = strlen(bitstream);

    char *encoded = malloc_string(bitstream, len * 2, "encode_manchester");

    if (encoded == NULL)
    {
        perror("Fallo en la asignación de memoria para encode_manchester");
        return NULL;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (bitstream[i] == '1')
        {
            // Transicion HL
            encoded[2 * i] = HIGH;
            encoded[2 * i + 1] = LOW;
        }
        else if (bitstream[i] == '0')
        {
            // Trasicion LH
            encoded[2 * i] = LOW;
            encoded[2 * i + 1] = HIGH;
        }
        else
        {
            fprintf(stderr, "Error: Caracter no válido en el bitstream: %c\n", bitstream[i]);
            free(encoded);
            return NULL;
        }
    }

    encoded[len * 2] = '\0';
    return encoded;
}

char *decode_manchester(const char *encoded)
{

    if (encoded == NULL)
        return NULL;

    size_t len = strlen(encoded);

    if (len % 2 != 0)
    {
        fprintf(stderr, "Error: La longitud de la señal codificada Manchester debe ser par.\n");
        return NULL;
    }

    size_t bitstream_len = len / 2;

    char *bitstream = malloc_string(encoded, bitstream_len, "decode_manchester");

    if (bitstream == NULL)
    {
        perror("Fallo en la asignación de memoria para decode_manchester");
        return NULL;
    }

    for (size_t i = 0; i < bitstream_len; i++)
    {
        char first_half = encoded[2 * i];
        char second_half = encoded[2 * i + 1];

        if (first_half == HIGH && second_half == LOW)
        {
            bitstream[i] = '1';
        }
        else if (first_half == LOW && second_half == HIGH)
        {
            bitstream[i] = '0';
        }
        else
        {
            fprintf(stderr, "Error: Secuencia no válida en la codificación Manchester: %c%c\n",
                    first_half, second_half);
            free(bitstream);
            return NULL;
        }
    }

    bitstream[bitstream_len] = '\0';
    return bitstream;
}

// ============================================
// 4B/5B
// ============================================

char *encode_4b5b(const char *bitstream)
{
    size_t len = strlen(bitstream);

    char *encoded_5b = malloc_string(bitstream, (len / 4) * 5, "encode_4b5b");

    int current_encoded_pos = 0;

    if (len % 4 != 0)
    {
        fprintf(stderr, "Error: La longitud del bitstream debe ser múltiplo de 4 para 4B/5B.\n");
        return NULL;
    }

    for (size_t i = 0; i < len; i += 4)
    {
        char block_4b[5];

        strncpy(block_4b, bitstream + i, 4);

        block_4b[4] = '\0';

        int index = bits_to_int(block_4b);

        const char *block_5b = TABLE_4B5B[index];

        strncpy(encoded_5b + current_encoded_pos, block_5b, 5);

        current_encoded_pos += 5;
    }

    encoded_5b[current_encoded_pos] = '\0';

    return encoded_5b;
}

char *decode_4b5b(const char *encoded)
{

    if (encoded == NULL)
        return NULL;

    size_t len = strlen(encoded);

    char *decoded_4b = malloc_string(encoded, (len / 5) * 4, "decode_4b5b");

    int current_decoded_pos = 0;

    for (size_t i = 0; i < len; i += 5)
    {
        char block_5b[6];

        strncpy(block_5b, encoded + i, 5);

        block_5b[5] = '\0';

        int found = 0;

        for (int j = 0; j < 16; j++)
        {
            if (strcmp(block_5b, TABLE_4B5B[j]) == 0)
            {
                char block_4b[5];
                int_to_bits(j, block_4b, 4);

                strncpy(decoded_4b + current_decoded_pos, block_4b, 4);

                current_decoded_pos += 4;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            fprintf(stderr, "Error: Secuencia no válida en la codificación 4B/5B: %s\n", block_5b);
            free(decoded_4b);
            return NULL;
        }
    }

    decoded_4b[current_decoded_pos] = '\0';

    return decoded_4b;
}

// ============================================
// Visualización de señales
// ============================================

void plot_signal(const char *title,const char *encoded, const char *filename)
{
    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        perror("Error al abrir el archivo");
        return;
    }

    size_t len = strlen(encoded);

    if (!len)
    {
        fprintf(file, "Cadena de señal vacía.\n");
        fclose(file);
        return;
    }

    plot_signal_header(encoded,file,title);

    plot_signal_high(encoded, file);

    plot_signal_low(encoded, file);

    fprintf(file, "\n\n");

    fclose(file);
}

// ============================================
// Simulación de ruido
// ============================================

void add_noise(char *bitstream, double ber)
{
    ber = 0.08;

    size_t len = strlen(bitstream);

    for (size_t i = 0; i < len; i++)
    {
        double rand_val = (double)rand() / RAND_MAX;

        if (rand_val < ber)
        {
            bitstream[i] = (bitstream[i] == '0') ? '1' : '0';
        }
    }
}
// ============================================
// Esquema Personalizado: ZR-Code (Zain Rondon Code)
// Codificacion 2B/3B con resistencia a inversion de polaridad
// ============================================

// Tablas de codificacion 2B/3B
static const char *ZR_TABLE_LOW[4] = {
    "LLL",  // 00: sin transiciones
    "LLH",  // 01: 1 transicion
    "LHL",  // 10: 2 transiciones
    "LHH"   // 11: 1 transicion
};

static const char *ZR_TABLE_HIGH[4] = {
    "HHH",  // 00: sin transiciones
    "HHL",  // 01: 1 transicion
    "HLH",  // 10: 2 transiciones
    "HLL"   // 11: 1 transicion
};

char *encode_custom(const char *bitstream)
{
    if (bitstream == NULL)
        return NULL;

    size_t len = strlen(bitstream);
    size_t adjusted_len = (len % 2 == 0) ? len : len + 1;
    size_t num_pairs = adjusted_len / 2;

    char *encoded = malloc_string(bitstream, num_pairs * 3, "encode_custom");
    if (encoded == NULL)
    {
        perror("Fallo en asignacion de memoria para encode_custom");
        return NULL;
    }

    char last_level = LOW;
    size_t out_pos = 0;

    for (size_t i = 0; i < adjusted_len; i += 2)
    {
        char bit1 = (i < len) ? bitstream[i] : '0';
        char bit2 = (i + 1 < len) ? bitstream[i + 1] : '0';

        if ((bit1 != '0' && bit1 != '1') || (bit2 != '0' && bit2 != '1'))
        {
            fprintf(stderr, "Error: Caracter no valido: %c%c\n", bit1, bit2);
            free(encoded);
            return NULL;
        }

        int index = ((bit1 - '0') << 1) | (bit2 - '0');
        const char *pattern = (last_level == LOW) ? ZR_TABLE_LOW[index] : ZR_TABLE_HIGH[index];

        encoded[out_pos++] = pattern[0];
        encoded[out_pos++] = pattern[1];
        encoded[out_pos++] = pattern[2];

        last_level = pattern[2];
    }

    encoded[out_pos] = '\0';
    return encoded;
}

char *decode_custom(const char *encoded)
{
    if (encoded == NULL)
        return NULL;

    size_t len = strlen(encoded);
    if (len % 3 != 0)
    {
        fprintf(stderr, "Error: Longitud debe ser multiplo de 3\n");
        return NULL;
    }

    size_t num_blocks = len / 3;
    size_t bitstream_len = num_blocks * 2;

    char *bitstream = malloc_string(encoded, bitstream_len, "decode_custom");
    if (bitstream == NULL)
    {
        perror("Fallo en asignacion de memoria para decode_custom");
        return NULL;
    }

    char last_level = LOW;
    size_t bit_pos = 0;

    for (size_t i = 0; i < num_blocks; i++)
    {
        char s1 = encoded[i * 3];
        char s2 = encoded[i * 3 + 1];
        char s3 = encoded[i * 3 + 2];

        if ((s1 != HIGH && s1 != LOW) || (s2 != HIGH && s2 != LOW) || (s3 != HIGH && s3 != LOW))
        {
            fprintf(stderr, "Error: Caracter no valido: %c%c%c\n", s1, s2, s3);
            free(bitstream);
            return NULL;
        }

        int found = 0;
        int decoded_value = -1;
        const char **table = (last_level == LOW) ? ZR_TABLE_LOW : ZR_TABLE_HIGH;

        for (int j = 0; j < 4; j++)
        {
            if (s1 == table[j][0] && s2 == table[j][1] && s3 == table[j][2])
            {
                decoded_value = j;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            fprintf(stderr, "Error: Patron no valido: %c%c%c\n", s1, s2, s3);
            free(bitstream);
            return NULL;
        }

        bitstream[bit_pos++] = ((decoded_value & 0x02) >> 1) + '0';
        bitstream[bit_pos++] = (decoded_value & 0x01) + '0';

        last_level = s3;
    }

    bitstream[bit_pos] = '\0';
    return bitstream;
}
