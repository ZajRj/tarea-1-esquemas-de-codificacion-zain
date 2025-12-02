#ifndef UTILS_H
#define UTILS_H

#include <stddef.h> // Para size_t
#include <stdio.h>

#define HIGH 'H' 
#define LOW  'L'

extern const char *const TABLE_4B5B[16];

char *malloc_string(const char *bitstream, size_t len, const char *func_name);

int bits_to_int(const char *block_4b);

void int_to_bits(int value, char *block_4b, int length);

const char *lookup_4b5b(const char *block_4b);


void plot_signal_header(const char *encoded,FILE *file, const char *title);
void plot_signal_high(const char *encoded,FILE *file);
void plot_signal_low(const char *encoded,FILE *file);


#endif // UTILS_H