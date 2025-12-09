#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char *const TABLE_4B5B[16] = {
    "11110", // 0 (0000)
    "01001", // 1 (0001)
    "10100", // 2 (0010)
    "10101", // 3 (0011)
    "01010", // 4 (0100)
    "01011", // 5 (0101)
    "01110", // 6 (0110)
    "01111", // 7 (0111)
    "10010", // 8 (1000)
    "10011", // 9 (1001)
    "10110", // 10 (1010)
    "10111", // 11 (1011)
    "11010", // 12 (1100)
    "11011", // 13 (1101)
    "11100", // 14 (1110)
    "11101"  // 15 (1111)
};

char *malloc_string(const char *bitstream, size_t len, const char *func_name)
{
    char *new_string = (char *)malloc(len + 1);

    if (new_string == NULL)
    {
        fprintf(stderr, "Error en %s: Fallo en la asignación de memoria.\n", func_name);
        perror("Detalles del error de malloc");
        return NULL;
    }

    new_string[0] = '\0';
    return new_string;
}

int bits_to_int(const char *block_4b)
{
    int value = 0;
    for (int i = 0; i < 4; i++)
    {
        if (block_4b[i] == '1')
        {
            value = (value << 1) | 1;
        }
        else if (block_4b[i] == '0')
        {
            value = (value << 1) | 0;
        }
        else
        {
            // Error en los datos de entrada (no es '0' ni '1')
            return -1;
        }
    }
    return value;
}

void int_to_bits(int value, char *block_4b, int length)
{
    for (int i = length - 1; i >= 0; i--)
    {
        block_4b[i] = (value & 1) ? '1' : '0';
        value >>= 1;
    }
    block_4b[length] = '\0';
}

const char *lookup_4b5b(const char *block_4b)
{
    int index = bits_to_int(block_4b);

    if (index >= 0 && index < 16)
    {
        return TABLE_4B5B[index];
    }

    // Devolver NULL si el bloque 4B no es válido
    return NULL;
}

void plot_signal_header(const char *encoded,FILE *file, const char *title)
{
    
    size_t len = strlen(encoded);

    // Encabezado y bits 
    fprintf(file, "## Diagrama ASCII de Señal %s ##\n", title);
    fprintf(file, "Bits:    ");

    for (size_t i = 0; i < len; i++)
    {
        fprintf(file, "%c   ", encoded[i]);
    }
    fprintf(file, "\n");
}

void plot_signal_high(const char *encoded, FILE *file)
{
    size_t len = strlen(encoded);
    // Nivel Superior (HIGH)
    fprintf(file, "Nivel H: ");
    char prev_level = '\0';

    for (size_t i = 0; i < len; i++)
    {
        char current_level = encoded[i];

        // Dibuja la transición vertical (si ocurre)
        // La transición se dibuja al inicio del nuevo bit.
        int is_transition = 0;

        if (i > 0 && prev_level != current_level)
            is_transition = 1;

        // Transición
        if (is_transition)
            fprintf(file, "|");
        else
            fprintf(file, " ");

        if (current_level == HIGH || current_level == '1')
            fprintf(file, "¯¯¯");

        else
            fprintf(file, "   ");

        prev_level = current_level;
    }
}

void plot_signal_low(const char *encoded, FILE *file)
{
    size_t len = strlen(encoded);

    // Nivel Inferior (LOW)
    fprintf(file, "\nNivel L: ");
    char prev_level = '\0'; // Carácter nulo para la primera iteración


    for (size_t i = 0; i < len; i++)
    {
        char current_level = encoded[i];

        // Dibuja la transición vertical
        int is_transition = 0;
        if (i > 0 && prev_level != current_level)
            is_transition = 1;

        // Transición
        if (is_transition)
            fprintf(file, "|");
        else
            fprintf(file, " ");

        // Dibuja el nivel horizontal
        if (current_level == LOW || current_level == '0')
            fprintf(file, "___");

        else

            fprintf(file, "   ");

        prev_level = current_level;
    }
}