#include "s21_decimal.h"

/* получает две переменные для сложения и указатель на результат
возвращает код ошибки*/
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int overflow = 0;  //  код ошибки 1-int, 2-(-inf), 0-ok
  if (!s21_is_correct_decimal(value_1) && !s21_is_correct_decimal(value_2)) {
    *result = DEC_ZERO;
  } else {
    *result = DEC_ZERO;
    int sign_1 = s21_get_sign(value_1);
    int sign_2 = s21_get_sign(value_2);
    int sign = 0;
    int max_power =
        s21_equal_power(&value_1, &value_2);  //  выравниваем степени
    *result = DEC_ZERO;  //  для корректного сложения, так же получаем степень
                         //  для результата
    s21_set_sign(&value_1,
                 0);  //  значения сохранены, обнуляем что бы не было конфликтов
    s21_set_sign(&value_2, 0);
    if (sign_1 == sign_2) {  //  если знаки одинаковые складываем столбиком
      overflow = s21_simplified_add(value_1, value_2, result);
      if (sign_1 == 1) {
        sign = 1;
        s21_set_sign(result, 1);  //  если знаки отрицательны
        if (overflow > 0) overflow = 2;  //  переполнение, ушли в -inf
      }
    } else {
      if (s21_is_greater_simplified(
              value_1,
              value_2)) {  //  знаки разные - значит
                           //  вычитаем из большего меньшее
        overflow = s21_simplified_sub(value_1, value_2, result);
        if (sign_1 == 1) {
          s21_set_sign(result, 1);
          sign = 1;
          if (overflow > 0) overflow = 2;
        }
      } else {
        overflow = s21_simplified_sub(value_2, value_1, result);
        if (sign_2 == 1) {
          sign = 1;
          s21_set_sign(result, 1);
          if (overflow > 0) overflow = 2;
        }
      }
    }
    if (overflow && s21_get_power(value_1)) {  //  из-за большой степени
      s21_bank_round(
          &value_1);  //  можем не поместиться, что бы избежать, округляем
      s21_bank_round(&value_2);  //  и по новой
      overflow = s21_add(value_1, value_2, result);
      max_power--;
    }
    s21_set_power(
        result,
        max_power);  //  устанавливаем итоговую степень, знак поставлен ранее
    s21_set_sign(result, sign);
  }
  return overflow;
}

//  НУЖНО ТЕСТИРОВАТЬ
//  обработать переполнение в -inf и +inf(возможно add уже предусматривает)
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int overflow = 0;
  if (!s21_is_correct_decimal(value_1) && !s21_is_correct_decimal(value_2)) {
    *result = DEC_ZERO;
  } else {
    *result = DEC_ZERO;
    s21_negate(value_2, &value_2);
    overflow = s21_add(value_1, value_2, *&result);
  }
  return overflow;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int overflow = 0;
  if (!s21_is_correct_decimal(value_1) && !s21_is_correct_decimal(value_2)) {
    *result = DEC_ZERO;
  } else {
    *result = DEC_ZERO;
    s21_decimal tmp = DEC_ZERO;
    if (s21_is_less(value_1, value_2)) {
      tmp = value_1;
      value_1 = value_2;
      value_2 = tmp;
      tmp = DEC_ZERO;
    }
    int sign = 0, sign_1 = s21_get_sign(value_1),
        sign_2 = s21_get_sign(value_2);
    s21_set_sign(&value_1, 0);
    s21_set_sign(&value_2, 0);
    int power_1 = s21_get_power(value_1);
    int power_2 = s21_get_power(value_2);
    int overflow1 = 0, overflow2 = 0, overflow3 = 0, overflow4 = 0,
        overflow5 = 0, overflow6 = 0, overflow7 = 0;
    s21_decimal int_v1 = DEC_ZERO, int_v2 = DEC_ZERO, frac_v1 = DEC_ZERO,
                frac_v2 = DEC_ZERO;
    if (sign_1 || sign_2) sign = 1;
    if (sign_1 && sign_2) sign = 0;
    if (power_1 > 0 ||
        power_2 > 0) {  //  если имеем дело с дробными множителями
      s21_decimal res1 = DEC_ZERO, res2 = DEC_ZERO, res3 = DEC_ZERO,
                  res4 = DEC_ZERO;
      s21_decimal null = DEC_ZERO, tmp1 = DEC_ZERO, tmp2 = DEC_ZERO;

      int digits_int_1 = 0, digits_int_2 = 0, digits_frac_1 = 0,
          digits_frac_2 = 0;
      int digits_1 = s21_signif_digits(value_1) - 1,
          digits_2 = s21_signif_digits(value_2) - 1;

      s21_splitting(value_1, value_2, &int_v1, &int_v2, &frac_v1, &frac_v2);

      digits_int_1 = s21_signif_digits(int_v1);
      digits_int_2 = s21_signif_digits(int_v2);
      digits_frac_1 = digits_1 - digits_int_1;
      digits_frac_2 = digits_2 - digits_int_2;

      s21_set_power(&int_v1,
                    0);  //  разбиваем множители на целые и дробные части
      s21_set_power(&int_v2, 0);
      s21_set_power(&frac_v1, 0);
      s21_set_power(&frac_v2, 0);

      overflow6 = s21_simplified_mul(int_v1, int_v2,
                                     &res1);  //  перемножаем целые части

      if ((overflow1 = s21_simplified_mul(int_v1, frac_v2, &null)) ==
          1) {  //  перемножаем целую(1-го) и дробную(2-го)
        overflow1 = s21_addit_int_frac_mul(int_v1, frac_v2, &res2, digits_int_1,
                                           digits_frac_2);
      } else {
        overflow1 = s21_simplified_mul(int_v1, frac_v2, &res2);
        s21_set_power(&res2, power_2);
      }

      if ((overflow2 = s21_simplified_mul(int_v2, frac_v1, &null)) ==
          1) {  //  перемножаем целую(2-го) и дробную(1-го)
        overflow2 = s21_addit_int_frac_mul(int_v2, frac_v1, &res3, digits_int_2,
                                           digits_frac_1);
      } else {
        overflow2 = s21_simplified_mul(int_v2, frac_v1, &res3);
        s21_set_power(&res3, power_1);
      }

      if ((overflow3 = s21_simplified_mul(frac_v1, frac_v2, &null)) == 1 ||
          (digits_frac_1 + digits_frac_2) >=
              28) {  //  перемножаем дробные части
        overflow3 = s21_addit_frac_frac_mul(frac_v1, frac_v2, &res4,
                                            digits_frac_1, digits_frac_2);
      } else {
        overflow3 = s21_simplified_mul(frac_v1, frac_v2, &res4);
        s21_set_power(&res4, power_1 + power_2);
      }

      if ((overflow1 + overflow2 + overflow3 + overflow6) != 0) {
        overflow = 1;
      }  //  следим за переполнением

      overflow4 =
          s21_add(res3, res4, &tmp1);  //  складываем полученные произведения
      overflow5 = s21_add(tmp1, res2, &tmp2);

      if ((overflow4 + overflow5) != 0 && overflow == 0) {
        overflow = 1;
      }

      overflow7 = s21_add(res1, tmp2, result);  //  складываем все
      if (overflow7) overflow = 1;

    } else {  //  если нету дробной части
      overflow = s21_simplified_mul(value_1, value_2, result);
    }
    s21_set_sign(result, sign);  //  знак
    if (overflow && sign) overflow = 2;
  }
  return overflow;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal buffer1 = DEC_ZERO, buffer2 = DEC_ZERO;
  int error = 0;
  if (!result) {
    error = 3;
  } else {
    *result = DEC_ZERO;
    if (s21_is_equal(value_1, value_2)) {
      error = 0;
      *result = DEC_ZERO;
    } else if (s21_is_less(value_1, value_2)) {
      *result = value_1;
    } else if (s21_is_equal(value_2, DEC_ZERO)) {
      error = 3;
    } else {
      int exponent1 = s21_get_power(value_1);
      int exponent2 = s21_get_power(value_2);
      if (exponent1 == exponent2) {
        s21_set_power(&value_1, 0);
        s21_set_power(&value_2, 0);
        simple_dividing(&buffer1, &buffer2, value_1, value_2);
        s21_set_power(&buffer1, exponent1);
      } else if (exponent1 > exponent2) {
        s21_set_power(&value_1, 0);
        s21_set_power(&value_2, 0);
        for (int i = exponent2; i < exponent1; i++) {
          s21_simplified_mul(value_2, DEC_TEN, &value_2);
        }
        simple_dividing(&buffer1, &buffer2, value_1, value_2);
        s21_set_power(&buffer1, exponent1);
      } else if (exponent2 > exponent1) {
        s21_set_power(&value_1, 0);
        s21_set_power(&value_2, 0);
        for (int i = exponent1; i < exponent2; i++) {
          s21_simplified_mul(value_1, DEC_TEN, &value_1);
        }
        simple_dividing(&buffer1, &buffer2, value_1, value_2);
        s21_set_power(&buffer1, exponent2);
      }
      *result = buffer1;
    }
  }
  return error;
}

/* algorithm of simple division:
for example, we have A = 11001 (25)
                     B = 101 (5)
we are going to substract B from A
result | substraction
          11001 -
  1       101
          -----
            10 -
  0         101
            ---
            101 -
  1         101
            ---
 ||            0
  5
first of all we put n bits of A to temporal s21_decimal variable, where n =
number of bits(B) if we cant subctract B from temporal, than we write 0 to
result and add next(according to variable "index") bit from A to temporal if we
can subsctract, we write 1 to result and add next(according to variable "index")
bit from A to temporal repeat until we reach end of A (go through all bits), or
remainder becomes zero */

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal quotient = DEC_ZERO, remainder = DEC_ZERO;
  *result = DEC_ZERO;
  int error = 0, minus = 0;
  int num_of_muls = 0;
  /* for ex. if we divide 3 and 200 we want to multiply 3 by 10 two times so it
  becomes 300, we store number of multiplications in this variable*/
  if (s21_get_sign(value_1) != s21_get_sign(value_2))
    minus = 1;  // dealing with sign
  int base_exp_of_value1 = s21_get_power(value_1);
  int base_exp_of_value2 = s21_get_power(value_2);
  s21_set_sign(&value_1, 0);
  s21_set_sign(&value_2, 0);
  // if (s21_is_greater(value_1, DEC_MAX) || s21_is_greater(value_2, DEC_MAX)) {
  //     error = 1; // too big numbers
  // } else if (s21_is_less(value_1, DEC_MIN) || s21_is_less(value_2, DEC_MIN))
  // {
  //     error = 2;  // too small numbers
  if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    error = 3;  // division by zero
  } else if (value_1.bits[0] == 0 && value_1.bits[1] == 0 &&
             value_1.bits[2] == 0) {
    *result = DEC_ZERO;
  } else if (value_2.bits[0] == 1 && value_2.bits[1] == 0 &&
             value_2.bits[2] == 0 && !s21_get_power(value_2)) {
    *result = value_1;  // if divisor is 1 then result is dividend
    if (minus) s21_set_sign(result, 1);
  } else {
    int max_exp =
        (s21_get_power(value_1) > s21_get_power(value_2))
            ? s21_get_power(value_1)
            : s21_get_power(value_2);  // we use max exponent when we have
                                       // several zeros after dot 0.00****
    max_exp = (s21_get_power(value_1) == s21_get_power(value_2)) ? 0 : max_exp;

    int exponent = abs(exp_check(value_1, value_2));
    if (exp_check(value_1, value_2) >
        0) {  // if we have 0.35 and 0.2 we make first number into 3.5
      for (int i = 0; i < exponent; i++) {
        s21_simplified_mul(value_1, DEC_TEN, &value_1);
        num_of_muls++;
        num_of_muls = num_of_muls - base_exp_of_value2;
      }
      s21_set_power(&value_1, s21_get_power(value_2));
    } else if (exp_check(value_1, value_2) <
               0) {  // if we have 0.2 and 0.35 we make second number into 3.5
      for (int i = 0; i < exponent; i++) {
        s21_simplified_mul(value_2, DEC_TEN, &value_2);
      }
      s21_set_power(&value_2, s21_get_power(value_1));
      max_exp = -max_exp;
    }
    if (exp_check(value_1, value_2) ==
        0) {  // if we have equal exponent we nulify it
      s21_set_power(&value_1, 0);
      s21_set_power(&value_2, 0);
    }
    int tmp = 0;  // number of significant digits in remainder (for ex. 0.00123,
                  // 123 are significant digits)
    int chast = 0;  // digit that we add to fractional part of our number

    while (s21_is_less(value_1, value_2)) {
      s21_simplified_mul(value_1, DEC_TEN, &value_1);
      num_of_muls++;
    }

    simple_dividing(&remainder, &quotient, value_1,
                    value_2);  // defining the quotient and remainder

    if (remainder.bits[0] != 0 || remainder.bits[1] != 0 ||
        remainder.bits[2] != 0) {  // merging the quotient and remainder
      *result = quotient;
      while (s21_is_greater(remainder, DEC_ZERO) && tmp < 8) {
        chast = 0;
        s21_simplified_mul(remainder, DEC_TEN, &remainder);
        while (s21_is_greater_or_equal(remainder, value_2)) {
          s21_sub(remainder, value_2, &remainder);
          chast++;
        }
        tmp++;
        if (tmp + num_of_muls + max_exp - 1 < 8) {
          s21_simplified_mul(*result, DEC_TEN, result);
          s21_add(*result, (s21_decimal){{chast, 0, 0, 0}}, result);
        }
        s21_set_power(result, tmp);
      }
    } else {
      *result = quotient;
      if (max_exp == 0) {
        s21_set_power(result, s21_get_power(*result) + num_of_muls);
      }
      if (max_exp < 0 && num_of_muls <= 1) {
        for (int i = 0; i < exponent; i++) {
          s21_simplified_mul(*result, DEC_TEN, result);
        }
      }
      if ((max_exp < 0) && (num_of_muls + 1 == exponent - base_exp_of_value1)) {
        for (int i = 0; i <= exponent; i++) {
          s21_simplified_mul(*result, DEC_TEN, result);
        }
      }
    }
    if (minus) s21_set_sign(result, 1);
  }
  return error;
}

int bits_quantity(
    s21_decimal value) {  // function returns last '1' bit of s21_decimal number
  int i = 95;
  for (; !s21_get_bit(value, i); i--) {
  }
  return i;
}

int exp_check(s21_decimal value_1,
              s21_decimal value_2) {  // function determines wich number has
                                      // bigger exponent
  int value_1_exponent = s21_get_power(value_1);
  int value_2_exponent = s21_get_power(value_2);
  return value_1_exponent - value_2_exponent;
}

void simple_dividing(s21_decimal *remainder, s21_decimal *quotient,
                     s21_decimal value_1, s21_decimal value_2) {
  int position = bits_quantity(value_1);  // index of the last bit of dividend
  int size = bits_quantity(value_2);      // index of the last bit of divisor

  // printf("value 1:\n");
  // s21_print_int_value(value_1);
  // printf("value 2:\n");
  // s21_print_int_value(value_2);
  // printf("\n\n");
  // printf("pos = %d\n, size = %d\n", position, size);
  for (int i = 0; i <= size;
       i++) {  // first entry, filling remainder decimal with last n bits of
               // dividend, where n is number of bits in divisor
    // int bit1 = s21_get_bit(value_1, position - i);
    int bit1 = (value_1.bits[(position - i) / 32] & (1 << ((position - i) % 32))) ? 1 : 0;
    // printf("remainder = ");
    // s21_print_int_value(*remainder);
    // printf("bit = %d\ni = %d\n", bit, i);
    s21_shift_left(remainder);
    if (bit1 == 1) {
      s21_set_bit_1(remainder, 0);
    } else {
      s21_set_bit_0(remainder, 0);
    }
    // s21_set_bit(remainder, 0, bit);                 // tut
  }

  // printf("remainder after filling:\n");
  // s21_print_int_value(*remainder);
  // printf("quotient after filling:\n");
  // s21_print_int_value(*quotient);
  // printf("\n\n");

  int index = position - size -
              1;  // index, that we will use when iterating in dividend

  if (s21_is_less(*remainder, value_2)) {  // first iteration
    s21_shift_left(remainder);
    // int bit2 = s21_get_bit(value_1, index);
    int bit2 = (value_1.bits[(index) / 32] & (1 << ((index) % 32))) ? 1 : 0;
    if (bit2 == 1) {
      s21_set_bit_1(remainder, 0);
    } else {
      s21_set_bit_0(remainder, 0);
    }
    // s21_set_bit(remainder, 0, bit);                  // tut
  } else {
    s21_sub(*remainder, value_2, remainder);
    s21_shift_left(remainder);
    // int bit3 = s21_get_bit(value_1, index);
    int bit3 = (value_1.bits[(index) / 32] & (1 << ((index) % 32))) ? 1 : 0;
    if (bit3 == 1) {
      s21_set_bit_1(remainder, 0);
    } else {
      s21_set_bit_0(remainder, 0);
    }

    // s21_set_bit(remainder, 0, bit3);                 // tut
    s21_shift_left(quotient);
    s21_set_bit_1(quotient, 0);
    // s21_set_bit(quotient, 0, 1);
  }

  // printf("remainder after first iteration:\n");
  // s21_print_int_value(*remainder);
  // printf("quotient after first iteration:\n");
  // s21_print_int_value(*quotient);
  // printf("\n\n");
  // s21_set_sign(remainder, 0);
  while (index >= 0) {  // iterating in cycle until reaching the end of dividend
    s21_set_sign(remainder, 0);
    if (s21_is_less(*remainder, value_2)) {
      index--;
      s21_shift_left(remainder);
      // int bit4 = s21_get_bit(value_1, index);
      int bit4 = (value_1.bits[(index) / 32] & (1 << ((index) % 32))) ? 1 : 0;
      // s21_set_bit(remainder, 0, bit4);

      if (bit4 == 1) {
      s21_set_bit_1(remainder, 0);
    } else {
      s21_set_bit_0(remainder, 0);
    }

      s21_shift_left(quotient);
      // s21_set_bit(quotient, 0, 0);
      s21_set_bit_0(quotient, 0);
    } else {
      index--;
      s21_sub(*remainder, value_2, remainder);
      s21_shift_left(remainder);
      // int bit5 = s21_get_bit(value_1, index);
      int bit5 = (value_1.bits[(index) / 32] & (1 << ((index) % 32))) ? 1 : 0;
      // s21_set_bit(remainder, 0, bit5);
      if (bit5 == 1) {
      s21_set_bit_1(remainder, 0);
    } else {
      s21_set_bit_0(remainder, 0);
    }
      s21_shift_left(quotient);
      s21_set_bit_1(quotient, 0);
      // s21_set_bit(quotient, 0, 1);
    }
    // printf("remainder in cycle, iteration numero %d:\n", position - index);
    // s21_print_int_value(*remainder);
    // printf("quotient in cycle, iteration numero %d:\n", position - index);
    // s21_print_int_value(*quotient);
    // printf("\n\n");
  }

  // printf("remainder after cycle:\n");
  // s21_print_int_value(*remainder);
  // printf("quotient after cycle:\n");
  // s21_print_int_value(*quotient);

  if (s21_is_less(value_1, value_2)) *quotient = DEC_ZERO;
  s21_shift_right(remainder);
  s21_set_sign(remainder, 0);
}
