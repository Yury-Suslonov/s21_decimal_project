#include "../s21_decimal.h"
#include <stdio.h>
int main() {
    s21_decimal res = DEC_ZERO;
    float f = powf(10,28);
    int er = s21_from_decimal_to_float(DEC_28_DIGITS, &f);
    s21_print_int_value(DEC_28_DIGITS);
    printf("error =%d\n f= %f\n", er, f);
    return 0;  //asdsad
}
