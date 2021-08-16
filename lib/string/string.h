/*
 * string.h
 *
 *  Created on: 25 апр. 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_STRING_STRING_H_
#define SRC_LIB_STRING_STRING_H_

#include <main.h>
#include <math.h>
#include <stdarg.h>

class String {
	public:
		static uint16_t chr_array_len(wchar_t *str);
		static uint16_t strlen(const wchar_t *str) {
			int i = 0;
			while (*str++)
				i++;
			return i; //отлажено
		}
		static void strcpy(wchar_t *destination, const wchar_t *source) {
			uint16_t i = 0;
			while (*source) {
				destination[i++] = *source++;
			}
			destination[i] = '\0';
		}
		static void strconvert_char_wchar(wchar_t *destination, const char *source) {
					uint16_t i = 0;
					while (*source) {
						destination[i++] = *source++;
					}
					destination[i] = '\0';
				}
		static void substring(wchar_t *destination, const wchar_t *source,
				uint16_t start, uint16_t length) {
			uint16_t len = 0;
			source = &source[start];
			while (*source && len < length) {
				destination[len++] = *source++;
			}
			destination[len] = '\0';
		}
		static void insert(wchar_t *destination, const wchar_t *source,
				uint16_t start) {
			int s_len = strlen(destination) - start;
			wchar_t *tmp = new wchar_t[s_len + 1];
			substring(tmp, destination, start, s_len);
			destination[start] = '\0';
			strcat(destination, source);
			strcat(destination, tmp);
			delete[] tmp;
		}
		static void strcat(wchar_t *destination, const wchar_t *source) {
			int len = strlen(destination);
			while (*source) {
				destination[len++] = *source++;
			}
			destination[len] = '\0';
		}
		static void cut_out(wchar_t *destination, uint16_t start,
				uint16_t length) {
			//0123456789o; st=3,length=2;
			//012cc56789o
			//a_st = st+length=5
			//tmp=substr(a_st, strlen(dest)-a_st'
			//tmp=56789o
			//dest[st] = 0
			//strcat(dest, tmp)
			int a_st = start + length;
			int tmp_len = strlen(destination) - a_st;
			wchar_t *tmp = new wchar_t[tmp_len + 1];
			substring(tmp, destination, a_st, tmp_len);
			destination[start] = '\0';
			strcat(destination, tmp);
			delete[] tmp;
		}
		static void replace(wchar_t *destination, const wchar_t *replace_by,
				uint16_t start, uint16_t length) {
			cut_out(destination, start, length);
			insert(destination, replace_by, start);
		}

		static int strstr1(const wchar_t *str1, const wchar_t *str2) {
			wchar_t *cp = (wchar_t*) str1;
			wchar_t *s1, *s2;
			int i = 0;
			if (!*str2) // если строка 2 нулевая
				return 0;
			while (*cp) {
				s1 = cp;
				s2 = (wchar_t*) str2;

				while (*s1 && *s2 && !(*s1 - *s2))
					s1++, s2++, i++;
				if (!*s2)
					return i; // позиция
				cp++;
			}
			return (0); // если  строка 1 нулевая
		}
		static int16_t strpos(const wchar_t *haystack, const wchar_t *needle,
				int start = 0) {
			/*
			 * Двигаемся по строке, инкрементируем указатель и позицию
			 * 		Если символ совпадает с первым символом иглы:
			 * 			двигаемся по строке пока игла или стог не будут равны нулю
			 * 				если равно, инкрементируем счётчик иглы и указатель стога
			 * 				иначе прерываем цикл
			 * 			если игла равна нулю - возвращаем позицию
			 * 			если стог равен нулю - возвращаем -1
			 */
			int pos = start, saved_pos = -1, needle_i = 1;
			while (haystack[pos]) {
				if (haystack[pos++] == *needle) {
					saved_pos = pos - 1;
					while (haystack[pos] && needle[needle_i]) {
						if (haystack[pos] == needle[needle_i]) {
							needle_i++;
							pos++;
						} else {
							needle_i = 1;
							pos++;
							saved_pos = -1;
							break;
						}
					}
					if (needle[needle_i] == '\0') {
						return saved_pos;
					} else if (haystack[pos] == '\0') {
						return -1;
					}
				}
			}
			return -1;
		}
		static uint8_t format(wchar_t *destination, const wchar_t *format,
				...) {
			/*
			 * Управляющая последовательность: %$t%
			 * Где %$ - начало тега
			 * t - тип
			 * % - конец тега.
			 * Пример: "This string has %$i% words, first of which is %$s%", 10, "this"
			 * Результат: "This string has 10 words first of which is this"
			 * Теги:
			 * i - integer
			 * fi - formatted integer (fi[min_rank_count]) : %$fi4% (3) = 0003
			 * d - double
			 * fd - formatted double (fd[min_digits_before_point].[max_digits_after_point]) : %$fd4,2% (3,1415) = 0003.14
			 * s - string
			 */
			strcpy(destination, format);
			//Переменная типа va_list – список аргументов
			va_list args;
			//Устанавливаем указатель на первый элемент
			va_start(args, format);
			int16_t tag_pos = -1;
			int16_t tag_end = -1;
			int16_t tag_len = -1;
			//	long arg_long = 0;
			int arg_int = 0;
			double arg_double = 0;
			wchar_t *arg_str = nullptr;
			wchar_t tag[256] = { 0 };
			do {
				tag_pos = strpos(destination, L"%$");
				if (tag_pos >= 0) {
					tag_end = strpos(destination, L"%", tag_pos + 1);
					if (tag_end > tag_pos + 2) {
						//0123%$i%56789o, 4;
						//tag_pos = 4
						//tag_end = 7
						//substr = 4+2 = 6, 7 - (4+2) = 1
						tag_len = tag_end - (tag_pos + 2);
						tag[0] = 0;
						substring(tag, destination, tag_pos + 2, tag_len);
						switch (*tag) {
						case 'i': //integer
							//Достаём следующий, указывая тип аргумента
							arg_int = va_arg(args, int);
							longToStr(arg_int, tag);
							break;
						case 'd': //double
							arg_double = va_arg(args, double);
							float_to_str(tag, arg_double);
							break;
						case 's': //string
							arg_str = va_arg(args, wchar_t*);
							strcpy(tag, arg_str);
							break;
						case 'f': //formatted
							if (tag[1] == 'i') {
								arg_int = va_arg(args, int);
								longToStr(arg_int, tag, wchar_to_int(tag[2]));
							} else {
								arg_double = va_arg(args, double);
								float_to_str(tag, arg_double, wchar_to_int(tag[4]), wchar_to_int(tag[2]));
							}
							break;
						default: //not a tag
							return 2; //Неизвестный тэг
							break;
						}
						replace(destination, tag, tag_pos,
								tag_end + 1 - tag_pos);
					} else {
						return 1; //синтаксическая ошибка
					}
				}
			} while (tag_pos >= 0);
			va_end(args); // завершаем обработку параметров
			return 0;
		}
		static uint16_t float_to_str(wchar_t *buf, double num,
				int8_t max_digits_after_point = -1,
				uint8_t min_digits_before_point = 0);
		static uint16_t longToStr(long l, wchar_t *buf,
				uint8_t forced_rank_count = 0);
		static void str_add_long_to_end(const long l, wchar_t *str,
				uint8_t forced_rank_count = 0) {
			uint16_t len = strlen(str);
			longToStr(l, &(str[len]), forced_rank_count);
		}
		static void str_add_float_to_end(const double num, wchar_t *str,
				int8_t max_digits_after_point = -1,
				uint8_t min_digits_before_point = 0) {
			uint16_t len = strlen(str);
			float_to_str(&(str[len]), num, max_digits_after_point,
					min_digits_before_point);
		}

		static uint8_t wchar_to_int(const wchar_t wchar) {
			switch (wchar) {
			case '0':
				return 0;
			case '1':
				return 1;
			case '2':
				return 2;
			case '3':
				return 3;
			case '4':
				return 4;
			case '5':
				return 5;
			case '6':
				return 6;
			case '7':
				return 7;
			case '8':
				return 8;
			case '9':
				return 9;
			default:
				return 0;
			}
		}
		static long str_to_int(const wchar_t *str) {
			uint8_t rank_count = strlen(str);
			bool negative = false;
			long val = 0;
			if (*str == '-') {
				negative = true;
				str++;
				rank_count--;
			}
			long rank_pow;
			while (rank_count && *str) {
				rank_pow = pow(10, rank_count);
				val += wchar_to_int(*str) * rank_pow;
				rank_count--;
				str++;
			}
			return val * (negative ? -1 : 1);
		}
};

#endif /* SRC_LIB_STRING_STRING_H_ */
