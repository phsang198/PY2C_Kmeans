#ifndef READ
#define READ
#include <stdio.h>
#include <stdint.h>
#include <math.h>

int64_t binary_to_negative_int(unsigned char* binary, int byte_size, int start) {
    // Calculate the decimal value of the binary number
    int64_t decimal_value = 0;
    for (int i = start; i < byte_size; ++i) {
        decimal_value = (decimal_value << 8) | binary[i];
    }

    // Check if the MSB is 1 (negative value)
    if (binary[start] & 0x80) {
        // Apply two's complement to get the negative value
        decimal_value = -((int64_t)((1ULL << (8 * byte_size)) - decimal_value));
    }

    return decimal_value;
}

int POW(int exponent) {
    int result = 1;

    for (int i = 0; i < exponent; ++i) {
        result *= 2;
    }

    return result;
}
int bin_to_int_digit(unsigned char* in, int length, int start)
{
    if (in[start] & 0x80)
    {
        return binary_to_negative_int(in, length, start);
    }

    int exp = 0;
    int i;
    int bin = 0;
    for (i = length - 1; i >= start; i--)
    {
        bin += in[i] * POW( exp);
        exp += 1;
    }
    return bin;
}
#endif // READ