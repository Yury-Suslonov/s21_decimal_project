#include "s21_decimal.h"

int s21_addit_frac_frac_mul(s21_decimal value_1, s21_decimal value_2,
                            s21_decimal *result, int digits_f_1,
                            int digits_f_2) {
  int over = 0;
  *result = DEC_ZERO;
  s21_decimal int1, int2, f1, f2;
  s21_decimal res1 = DEC_ZERO, res2 = DEC_ZERO, res3 = DEC_ZERO,
              res4 = DEC_ZERO;
  s21_decimal tmp1 = DEC_ZERO, tmp2 = DEC_ZERO;
  int over1 = 0, over2 = 0, over3 = 3, over4 = 0, over5 = 0, over6 = 0,
      over7 = 0;
  int digits_1 = digits_f_1;
  int digits_2 = digits_f_2;

  s21_set_power(&value_1, digits_1 / 2);
  s21_set_power(&value_2, digits_2 / 2);

  s21_splitting(value_1, value_2, &int1, &int2, &f1, &f2);

  int power_1 = (digits_1 - (digits_1 / 2)) + (digits_2 - (digits_2 / 2));

  over1 = s21_simplified_mul(int1, int2, &res1);
  s21_bank_if_need(&power_1, &res1);
  s21_set_power(&res1, power_1);

  int power_2 = digits_2 + (digits_1 - (digits_1 / 2));
  over2 = s21_simplified_mul(int1, f2, &res2);
  s21_bank_if_need(&power_2, &res2);
  s21_set_power(&res2, power_2);

  int power_3 = digits_1 + (digits_2 - (digits_2 / 2));
  over3 = s21_simplified_mul(int2, f1, &res3);
  s21_bank_if_need(&power_3, &res3);
  s21_set_power(&res3, power_3);

  int power_4 = digits_1 + digits_2;
  over4 = s21_simplified_mul(f1, f2, &res4);
  s21_bank_if_need(&power_4, &res4);
  s21_set_power(&res4, power_4);

  over5 = s21_add(res1, res2, &tmp1);
  over6 = s21_add(res3, res4, &tmp2);
  over7 = s21_add(tmp1, tmp2, result);

  if ((over1 + over2 + over3 + over4 + over5 + over6 + over7) > 0) over = 1;
  return over;
}

int s21_addit_int_frac_mul(s21_decimal value_1, s21_decimal value_2,
                           s21_decimal *result, int digits_i, int digits_f) {
  int over = 0;
  *result = DEC_ZERO;
  s21_decimal int1, int2, f1, f2;
  s21_decimal res1 = DEC_ZERO, res2 = DEC_ZERO, res3 = DEC_ZERO,
              res4 = DEC_ZERO;
  int over1 = 0, over2 = 0, over3 = 3, over4 = 0, over5 = 0, over6 = 0,
      over7 = 0, over8 = 0;
  int digits_1 = digits_i;
  int digits_2 = digits_f;

  s21_set_power(&value_1, digits_1 / 2);
  s21_set_power(&value_2, digits_2 / 2);
  s21_splitting(value_1, value_2, &int1, &int2, &f1, &f2);

  int power_1 = (digits_2 / 2) - (digits_1 / 2);
  over1 = s21_simplified_mul(int1, int2, &res1);
  s21_bank_if_need(&power_1, &res1);
  if (power_1 < 0) power_1 = 0;
  s21_set_power(&res1, power_1);

  int power_2 = (digits_2 / 2 + (digits_2 - (digits_2 / 2))) - (digits_1 / 2);
  over2 = s21_simplified_mul(int1, f2, &res2);
  s21_bank_if_need(&power_2, &res2);
  if (power_2 < 0) power_2 = 0;
  s21_set_power(&res2, power_2);

  int power_3 = digits_2 / 2;
  over3 = s21_simplified_mul(int2, f1, &res3);
  s21_bank_if_need(&power_3, &res3);
  if (power_3 < 0) power_3 = 0;
  s21_set_power(&res3, power_3);

  int power_4 = digits_2;
  over4 = s21_simplified_mul(f1, f2, &res4);
  s21_bank_if_need(&power_4, &res4);
  if (power_4 < 0) power_4 = 0;
  s21_set_power(&res4, power_4);

  over5 = s21_add(res1, *result, result);
  over6 = s21_add(res2, *result, result);
  over7 = s21_add(res3, *result, result);
  over8 = s21_add(res4, *result, result);

  if ((over1 + over2 + over3 + over4 + over5 + over6 + over7 + over8) > 0)
    over = 1;
  return over;
}

//  вспомогательная функция для s21_addit_int_frac_mul и s21_addit_frac_frac_mul
void s21_bank_if_need(int *power, s21_decimal *res) {
  if (*power >= 28) {
    for (; (*power) - 1 >= 28; (*power)--) {
      s21_bank_round(res);
    }
  }
}

/*если надо поделить на 10 и округлить эта функция вам поможет
принимает значение которое разделит на 10 и округлит по
правилам банковского округления*/
s21_decimal s21_bank_round(s21_decimal *value) {
  int tmp = value->bits[3];  //  сохраняем последний бит со знаком и степенью
  int power = s21_get_power(*value);
  s21_set_power(value, 0);
  s21_set_sign(value, 0);
  s21_decimal last = s21_simplified_div(*value, DEC_TEN, value);
  s21_decimal val = *value;  //  сохраняем последнее и предпоследнее числа(int)
  s21_decimal prev_last = s21_simplified_div(val, DEC_TEN, &val);
  if (last.bits[0] > 5) {  //  два правила банковского окруления
    s21_simplified_add(*value, DEC_ONE, value);
  } else if (last.bits[0] == 5 && prev_last.bits[0] % 2 == 1) {
    s21_simplified_add(*value, DEC_ONE, value);
  }
  value->bits[3] = tmp;  // возвращаем знак и степень на место
  s21_set_power(value, power - 1);  //  -1 ибо поделили на 10
  return last;
}

//  делит на 10, возвращяет остаток от деления
s21_decimal s21_div_10(s21_decimal *value_1) {
  unsigned long long val = (unsigned long long)value_1->bits[2];
  unsigned long long tmp = 0;
  s21_decimal mod = DEC_ZERO;
  for (int i = 2; i > 0;) {
    tmp = val % 10;
    value_1->bits[i] = val / 10;
    i--;
    val = tmp * (U_MAX_INT + 1) + (unsigned long long)value_1->bits[i];
    if (i == 0) {
      value_1->bits[i] = val / 10;
      mod.bits[i] = val % 10;
    }
  }
  s21_set_power(value_1, s21_get_power(*value_1) - 1);
  return mod;
}

/*выравнивает степени, домножает на 10(десятичную) для корректных операций в
дальнейшем получает по указателю две переменные куда и заносит новые равные
степени*/
int s21_equal_power(s21_decimal *value_1, s21_decimal *value_2) {
  s21_decimal null = DEC_ZERO;
  int exit = 1;       //  флаг выхода
  int max_power = 0;  //  вернем
  int overflow = 0;   //  переполнение
  int shift = 0;      //  сдвиг для уравнения степеней
  int flag = 0;
  int power_1 = s21_get_power(*value_1);
  int power_2 = s21_get_power(*value_2);
  int sign_1 = s21_get_sign(*value_1);
  int sign_2 = s21_get_sign(*value_2);
  if (power_1 > power_2) {  //  степени не равны, ищем большую
    max_power = power_1;
    shift = power_1 - power_2;  //  и устанавлием сдвиг
    flag = 1;
    if (s21_is_zero(*value_2)) shift = 0;
  } else {
    max_power = power_2;
    shift = power_2 - power_1;
    flag = 0;
    if (s21_is_zero(*value_1)) shift = 0;
  }
  for (; shift > 0 && exit;) {
    if (flag) {  //  в зависимости от большей степени умножаем другую на 10 если
                 //  возможно
      if ((overflow = s21_simplified_mul(*value_2, DEC_TEN, &null)) == 0) {
        s21_simplified_mul(*value_2, DEC_TEN, value_2);
        shift--;
      } else {  //  больше не можем умножать, значит будем использовать
        exit = 0;  //  банковское округление для другого числа
      }
    } else {
      if ((overflow = s21_simplified_mul(*value_1, DEC_TEN, &null)) == 0) {
        s21_simplified_mul(*value_1, DEC_TEN, value_1);
        shift--;
      } else {
        exit = 0;
      }
    }
    null = DEC_ZERO;
  }
  for (; shift >
         0;) {  //  если умножать больше нельзя, а разница степеней осталась
    if (flag) {  //  начинаем подгонять другое значение посредством деление на
                 //  10 и банковского округления
      s21_bank_round(value_1);
    } else {
      s21_bank_round(value_2);
    }
    shift--;
    max_power--;
  }
  s21_set_power(value_1, max_power);
  s21_set_power(value_2, max_power);
  s21_set_sign(value_1, sign_1);
  s21_set_sign(value_2, sign_2);
  return max_power;
}

int s21_is_zero(s21_decimal value) {
  int res = 0;
  s21_decimal zero = DEC_ZERO;
  if (value.bits[0] == zero.bits[0] && value.bits[1] == zero.bits[1] &&
      value.bits[2] == zero.bits[2])
    res = 1;
  return res;
}

//  СКОРЕЕ ВСЕГО РАБОТАЕТ НЕВЕРНО(воздержитесь от использования или проверяйте
//  все) функция просто умножает число на 10 и вернут 1 в случае переполнения
//  надо убрать лишнии поступающие данные - убрать val2 и res
int s21_mul_10(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  *result = DEC_ZERO;
  s21_decimal tmp = DEC_ZERO;
  int overflow = 0;
  int digits = s21_signif_bit(value_1);  //  кол-во значащих бит в числе
  if (digits != 96 && digits != -1) {  //  при больших числах была ошибка
    for (int i = 0; i < digits && !overflow; i++) {  //  заходило лишний раз
      for (int k = 0; k < digits; k++) {
        /* умножаем в столбик, в tmp заносим
        сначала число получившееся в результате умноженния разряда единиц
        второго числа на все первое число, прибавляем к пустому result, обнуляем
        tmp, потом результат умножения разряда десятков второго чила на первое
        число...*/
        if (s21_get_bit(value_1, k) == 1 && s21_get_bit(value_2, i) == 1) {
          s21_set_bit(&tmp, k + i, 1);
        }
      }
      overflow = s21_simplified_add(tmp, *result, result);
      tmp = DEC_ZERO;
    }
  } else {
    overflow = 1;
  }
  return overflow;  //  код ошибки
}

/*принимаем переменную в которой необходимо узнать кол-во значащих бит
возращяем кол-во значащих бит*/
int s21_signif_bit(s21_decimal value_1) {
  int res = -1;
  for (int i = 0; i < 96;
       i++) {  //  идем по всем 96 битам и если находим единицу
    if (s21_get_bit(value_1, i) == 1)  //  приравнием к res
      res = i;  //  в конечном итоге получим максимальный содержательный бит
  }
  if (res != -1) res++;
  return res;
}

//  нужно переписать без использование dec_to_str или с использованием др
//  функции
int s21_signif_digits(s21_decimal value) {
  int res = 0;
  char *str = s21_decimal_to_string(value);
  res = (int)strlen(str);
  free(str);
  return res;
}

/*поступают две переменниые типа s21_decimal которые нужно сложить
 и переменная под результат(по указателю)
 вспомогательная функция сложения*/
int s21_simplified_add(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result) {
  *result = DEC_ZERO;
  int overflow = 0;
  for (int i = 0; i <= 95; i++) {  //  идем по 96 битам(3 массивам)
    int first_v = s21_get_bit(value_1, i);
    int second_v = s21_get_bit(value_2, i);
    int val =
        first_v + second_v +
        overflow;  //  складываем "в столбик" не забывая про единицу "в уме"
    overflow = val / 2;  //  "запоминаем" единицу "в уме"
    s21_set_bit(result, i, val % 2);
  }
  return overflow;  //  если тут осталась единица значит произошло
                    //  переполнение(inf)
}

//  надо бы проверить работает при value_1 < value_2(думаю нет)
//  степень предварительно занулить и знак установить в +| (скорее всего)
//  вернет остаток от деления
s21_decimal s21_simplified_div(s21_decimal value_1, s21_decimal value_2,
                               s21_decimal *result) {
  *result = DEC_ZERO;
  s21_decimal v1 = value_1;
  s21_decimal mod;
  s21_decimal v2 = value_2;
  int num_1 = s21_signif_bit(value_1);
  int num_2 = s21_signif_bit(value_2);
  int shift = 0;
  shift = num_1 - num_2;
  for (int i = 0; i < shift;
       i++) {  //  выравниваем меньшее число слева по большему
    s21_shift_left(&value_2);  // так, когда делим в столбик
  }
  for (int i = 0; i <= shift && !s21_shift_left(result);
       i++) {  //  деление в столбик
    if (s21_is_greater_simplified(value_1, value_2) ||  //  нужно поменять на
        (value_1.bits[0] ==
             value_2.bits[0] &&  //  s21_is_greater_or_equal_simplified
         value_1.bits[1] ==
             value_2.bits[1] &&  //  ибо в фулл версии есть выравнивание которые
                                 //  уже сделали
         value_1.bits[2] == value_2.bits[2])) {
      s21_simplified_sub(value_1, value_2, &value_1);
      result->bits[0] += 1;
    }
    s21_shift_right(
        &value_2);  //  как и в делении, вычли, занесли циферку в ответ
  }                 //  сдвинули
  s21_simplified_mul(*result, v2, &v2);  //  для mod
  s21_simplified_sub(v1, v2, &mod);
  return mod;
}

//  вспомогательная функция упрощенного умножения*/
//  описание в s21_mul_10
int s21_simplified_mul(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result) {
  s21_decimal tmp = DEC_ZERO;
  *result = DEC_ZERO;
  int over = 0;
  int digits = s21_signif_bit(value_2);
  for (int i = 0; i < 3; i++) {
    result->bits[i] = 0;
  }
  for (int i = 0; i < digits && !over; i++) {
    if (s21_get_bit(value_2, i) == 1) {
      over = s21_simplified_add(*result, value_1, result);
    }
    if (s21_shift_left(&value_1) && i != digits - 1) {
      over = 1;
    }
  }
  if (over) {
    *result = tmp;
  }
  return over;
}

/*поступают две переменниые типа s21_decimal которые нужно вычесть
и переменная под результат(по указателю)
вспомогательная функция вычитания*/
int s21_simplified_sub(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result) {
  *result = DEC_ZERO;
  int overflow = 0;
  for (int i = 0; i <= 95; i++) {  //  идем по 96 битам(3 массивам)
    int first_v = s21_get_bit(value_1, i);
    int second_v = s21_get_bit(value_2, i);
    int val = first_v - second_v - overflow;
    s21_set_bit(
        *&result, i,
        (val + 2) %
            2);  //  при v=0:(0+2)%2=0;при v=1:(1+2)%2=1;при v=-1:(-1+2)%2=1
    if (val < 0)  //  если "заняли" единицу
      overflow = 1;
    else
      overflow = 0;
  }
  return overflow;
}

void s21_splitting(s21_decimal value_1, s21_decimal value_2,
                   s21_decimal *int_v1, s21_decimal *int_v2,
                   s21_decimal *frac_v1, s21_decimal *frac_v2) {
  *int_v1 = DEC_ZERO, *int_v2 = DEC_ZERO, *frac_v1 = DEC_ZERO,
  *frac_v2 = DEC_ZERO;
  int shift_1 = s21_get_power(value_1);
  int shift_2 = s21_get_power(value_2);
  s21_decimal tmp = DEC_TEN;
  s21_decimal mod = DEC_ZERO;
  for (int i = 0; i < shift_1; i++) {
    mod = s21_simplified_div(value_1, DEC_TEN, &value_1);
    if (i > 1) s21_mul_10(tmp, DEC_TEN, &tmp);
    if (i == 0) {
      s21_simplified_add(*frac_v1, mod, frac_v1);
    } else {
      s21_simplified_mul(mod, tmp, &mod);
      s21_simplified_add(*frac_v1, mod, frac_v1);
    }
  }
  *int_v1 = value_1;
  mod = DEC_ZERO;
  tmp = DEC_TEN;
  for (int i = 0; i < shift_2; i++) {
    mod = s21_simplified_div(value_2, DEC_TEN, &value_2);
    if (i > 1) s21_mul_10(tmp, DEC_TEN, &tmp);
    if (i == 0) {
      s21_simplified_add(*frac_v2, mod, frac_v2);
    } else {
      s21_simplified_mul(mod, tmp, &mod);
      s21_simplified_add(*frac_v2, mod, frac_v2);
    }
  }
  *int_v2 = value_2;
}
