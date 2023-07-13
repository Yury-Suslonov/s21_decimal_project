#include "s21_decimal.h"

//  TODO(genghiss): fix cppcheck error

// Функция принимает на вход целое 32-х битовое число, а возвращает
// число, переведенное в строку, при необходимости добавляя ведущие нули
char *s21_bitwise_int_to_str(int a) {
  s21_decimal dec = {{a, 0, 0, 0}};
  char *res = malloc(sizeof(char) * 32);
  for (int i = 0; i < 32; i++) {
    res[i] = s21_get_bit(dec, i);
  }
  return res;
}

char *s21_decimal_to_string(s21_decimal value) {
  char *result_string = (char *)calloc(32, sizeof(char));
  memset(result_string, '0', 29);
  char pow_of_two[30];
  memset(pow_of_two, '0', 28), pow_of_two[28] = '1', pow_of_two[29] = '\0';

  if (s21_get_bit(value, 0)) result_string[28] = '1';

  for (int bits = 1; bits < 96; bits++) {
    int flag = 0;
    for (int i = 28; i >= 0; i--) {
      int sym = pow_of_two[i] - '0';
      sym = (flag) ? (sym * 2 + flag) : (sym * 2);
      pow_of_two[i] = sym % 10 + '0';
      flag = sym / 10;
    }
    if (s21_get_bit(value, bits)) {
      flag = 0;
      for (int i = 28; i >= 0; i--) {
        int sym = pow_of_two[i] - '0' + result_string[i] - '0';
        sym = (flag) ? (sym + flag) : sym;
        result_string[i] = sym % 10 + '0';
        flag = sym / 10;
      }
    }
  }

  int pow = s21_get_power(value);
  if (pow < 0 || pow > 28) pow = 0;
  if (pow) {
    memmove(result_string + 30 - pow, result_string + 29 - pow, pow);
    result_string[29 - pow] = '.';
  }
  int numer = 0;
  while (result_string[numer] == '0' && numer < 29 - pow - 1) numer++;
  if (numer != 0) memmove(result_string, result_string + numer, 32 - numer);
  if (s21_get_bit(value, 127)) {
    memmove(result_string + 1, result_string, 30);
    result_string[0] = '-';
  }
  return result_string;
}

/*  получаем бит по индексу бита, сначала берем необходимый массив(в каждом по
32 бита), логически умножаем его на индекс искомого бита сдвинутый влево на 1(0
свдинутый влево дает 1) или на число у которого все элементы кроме элемента с
номером индекса равны нулю (остаток на 32 в случаи если искомый индекс не в
первом байте) s21_decimal *decimal - переменная типа decimal в которой получаем
бит int index - номер индекса которого получаем бит возвращяемое значение -
значение бита по номеру индекса(0 или 1)  */
int s21_get_bit(s21_decimal decimal, int index) {
  int res = (decimal.bits[index / 32] & (1 << (index % 32))) ? 1 : 0;
  return res;
}



/*  возвращяемое значение - степень -10-ти в которую нужное возвести число(в
десятичной СС), что бы в нужном месте поставить точку, если вернулось
отрицательное число - указана слишком большая точность */
int s21_get_power(s21_decimal decimal) {
  int res = decimal.bits[3];
  res <<= 1;   //  избавляемся от знака
  res >>= 17;  //  избаяляемся от нулей(0 - 15 бит + мы двигали влево на 1)
  res = (res > 28) ? res - 257 : res;
  /*  если res > 28(максимальной точности)
  вычитаем 256(масмиальной величене которое может поместиться в 8 бит, что бы
  получить 0 и меньше)  */
  return res;
}

/*  возращаемое значение 1 если минус, 0 если плюс
в 127 бите хранится знак, если в 127 бите не 0 или 1 вернется -1  */
int s21_get_sign(s21_decimal decimal) {
  int res = -1;
  if (s21_get_bit(decimal, 127) == 0) {
    res = 0;  //  плюс
  } else {
    res = 1;  //  минус
  }
  return res;
}

int s21_is_correct_decimal(s21_decimal decimal) {
  int code = 1;

  int power = s21_get_power(decimal);
  if (power < 0 || power > 28) {
    code = 0;
  }
  for (int i = 96; i < 112; i++) {
    if (s21_get_bit(decimal, i)) code = 0;
  }
  for (int i = 120; i < 127; i++) {
    if (s21_get_bit(decimal, i)) code = 0;
  }

  return code;
}

void s21_print_int_value(s21_decimal dec) {
  if (s21_get_sign(dec))
    printf("-");
  else
    printf("+");
  for (int i = 95; i >= 0; i--) {
    printf("%d", s21_get_bit(dec, i));
  }
  printf("\n");
}

/*  устанавлием бит по индексу с помощью логического выражения с -
умножения в единицу: c таким числом у которого все биты кроме индекса равны нулю
сложения в ноль: с числом у которого биты кроме индекса равны единице(или
применим логическое отрицание к числу у которого все элементы кроме элемента с
номером индекса равны нулю) s21_decimal *decimal - переменная типа decimal в
которой меняем бит int index - номер индекса которого меняем бит int v_bit -
значение в которое необходимо установить бит(0 или 1) */
void s21_set_bit(s21_decimal *decimal, int index, int v_bit) {
  if (v_bit == 1) {
    (decimal->bits[index / 32] |= (1 << (index % 32)));
  }
  if (v_bit == 0) {
    (decimal->bits[index / 32] &= ~(1 << (index % 32)));
  }
}

void s21_set_bit_1(s21_decimal *decimal, int index) {
    (decimal->bits[index / 32] |= (1 << (index % 32)));
}

void s21_set_bit_0(s21_decimal *decimal, int index) {
    (decimal->bits[index / 32] &= ~(1 << (index % 32)));
}

/*получаем переменную в которой перезапишем степень по указателю и
степень в десятичной СС*/
void s21_set_power(s21_decimal *decimal, int power) {
  for (int i = 112; i < 120; i++) {  //  идем по битам где хранится степень
    s21_set_bit(decimal, i, power & 1);  //  0b1 - это единица в 2-ой СС
    power >>= 1;  //  двигаем полученную степень вправо на 1 (11 >> 1 = 1)
  }
}

/*int sign - необходимое значение знака, 1 - минус, 0 - плюс  */
void s21_set_sign(s21_decimal *decimal, int sign) {
  s21_set_bit(decimal, 127, sign);
}

//  сдвиг влево, с защитой
int s21_shift_left(s21_decimal *value) {
  int val = 0, v = 0;
  if (value->bits[2] & (1 << 31))
    val = 1;
  else
    val = 0;
  if (!val) {
    for (int i = 0; i <= 2; i++) {
      if (value->bits[i] & (1 << 31))
        v = 1;
      else
        v = 0;
      value->bits[i] <<= 1;
      value->bits[i] = value->bits[i] + val;
      val = v;
    }
  }
  return val;
}

//  сдвиг вправо
void s21_shift_right(s21_decimal *value) {
  int val = 0, v = 0;
  if (value->bits[0] & 1)
    val = 1;
  else
    val = 0;
  if (!val) {
    for (int i = 2; i >= 0; i--) {
      if (value->bits[i] & 1)
        v = 1;
      else
        v = 0;
      value->bits[i] >>= 1;
      s21_set_bit(value, 32 * i + 31, val);
      val = v;
    }
  }
}
