/*
 * string.cpp
 *
 *  Created on: 25 апр. 2021 г.
 *      Author: neovs
 */

#include "string.h"

uint16_t String::longToStr(long l, wchar_t *buf, uint8_t forced_rank_count) {
	long copyOfl = l;
	uint8_t counter = 0;
	uint8_t length = 0;
	if (forced_rank_count < 1)
		forced_rank_count = 1;
	if (copyOfl < 0) {
		copyOfl *= -1;
		if (buf != nullptr)
			buf[counter] = '-';
		counter++;
		length++;
	}

	while (l || length < forced_rank_count) {
		l /= 10;
		if (buf != nullptr)
			buf[length] = '0';
		length++;
	}
	if (length < forced_rank_count) {
		length = forced_rank_count;
	}
//printf("test");
	if (buf != nullptr) {
		counter = length - 1;
		buf[length] = '\0'; //EOL
		while (copyOfl) {
			uint8_t currentDigit = copyOfl % 10;
			switch (currentDigit) {
			case 0:
				buf[counter--] = '0';
				break;
			case 1:
				buf[counter--] = '1';
				break;
			case 2:
				buf[counter--] = '2';
				break;
			case 3:
				buf[counter--] = '3';
				break;
			case 4:
				buf[counter--] = '4';
				break;
			case 5:
				buf[counter--] = '5';
				break;
			case 6:
				buf[counter--] = '6';
				break;
			case 7:
				buf[counter--] = '7';
				break;
			case 8:
				buf[counter--] = '8';
				break;
			case 9:
				buf[counter--] = '9';
				break;
			default:
				buf[counter--] = 'E';
				break;

			}
			copyOfl /= 10;
		}
	}
	return length;

}

uint16_t String::float_to_str(wchar_t *buf, double f,
		int8_t max_digits_after_point, uint8_t min_digits_before_point) {
	/*
	 * Вычисляем число цифр целого
	 * Умножаем на 10 в степени количества цифр после точки
	 * Начинаем итеративно делить на десять, пока оставшееся
	 * количество цифр не станет равно количеству цифр целого
	 */
	uint8_t int_part_count = 0;
	float f_copy = f;
	if (f_copy < 0)
		f_copy *= -1;
	if (max_digits_after_point == -1) { //AUTO
		/*			Нужно найти количество цифр после точки, полезных для отображения.
		 * 			Весьма хреново, если число у нас 12.4, а мы его отображаем как 12.4000000
		 * 			Точно так же хреново, если число у нас 12.04, а мы его отображаем, как 12
		 * 			Настолько же хреново, если число у нас 12.91284123012358123124752938412, и нам придётся всё это отображать.
		 * 			Вот и думаем, ну его нахер, или что - либо делать?
		 */
		int64_t int_of_f = f_copy;
		if (f == int_of_f) { //Число целое.
			return longToStr(f, buf, min_digits_before_point);
		} else {
			double copy_f_after_point = f_copy - int_of_f;
			max_digits_after_point = 0;
			//0.35
			//3.5
			//3.5 => 3
			// 3.5 - 3 = 0.5
			/*
			 * 0.34999999999999999999999999
			 * 3.499999999999999
			 * 4.999999999999999
			 * 9.999999999999
			 * 9.9999999
			 */
			int8_t int_copy = -1;
			while (copy_f_after_point > 0 && max_digits_after_point <= 6) {
				copy_f_after_point *= 10.;
				if (int_copy == (int) copy_f_after_point) {
					copy_f_after_point = 0;
				} else {
					int_copy = copy_f_after_point;
					copy_f_after_point -= int_copy;
					max_digits_after_point++;
				}
			}
		}
	}
	if (min_digits_before_point == 0)
		min_digits_before_point = 1;
	//f_copy += min_digits_before_point;
	while ((int32_t) f_copy || int_part_count < min_digits_before_point) {
		f_copy /= 10;
		int_part_count++;
	}
	f *= pow(10.0, max_digits_after_point);
	uint8_t max_digits_count = int_part_count + max_digits_after_point;
	int8_t counter = max_digits_count;
	bool f_less_than_zero = f < 0;
	uint8_t str_length = int_part_count + (max_digits_after_point ? 1 : 0)
			+ max_digits_after_point + f_less_than_zero;
	if (buf == nullptr)
		return str_length;
	for (uint8_t i = 0; i < str_length; i++) {
		buf[i] = 'E';
	}
	buf[counter + f_less_than_zero + 1] = 0;
	if (f_less_than_zero) {
		f *= -1;
		buf[0] = '-';
	}
	bool dot_inserted = false;
	counter += f_less_than_zero;
	while (counter >= f_less_than_zero) {
		if (counter == int_part_count + f_less_than_zero && !dot_inserted
				&& max_digits_after_point) {
			buf[counter--] = '.';
			dot_inserted = true;
			continue;
		}
		uint8_t currentDigit = ((uint64_t) f) % 10;
		switch (currentDigit) {
		case 0:
			buf[counter--] = '0';
			break;
		case 1:
			buf[counter--] = '1';
			break;
		case 2:
			buf[counter--] = '2';
			break;
		case 3:
			buf[counter--] = '3';
			break;
		case 4:
			buf[counter--] = '4';
			break;
		case 5:
			buf[counter--] = '5';
			break;
		case 6:
			buf[counter--] = '6';
			break;
		case 7:
			buf[counter--] = '7';
			break;
		case 8:
			buf[counter--] = '8';
			break;
		case 9:
			buf[counter--] = '9';
			break;
		default:
			buf[counter--] = 'E';
		}
		f /= 10;
	}
	return str_length;
}

/*
 const String& operator+=(String &left, String right) {
 left.add_to_end(right);
 return left;*/
/*wchar_t *buf = (wchar_t*) malloc(sizeof(wchar_t)*(left.length() + right.length() + 1));
 left.set_new_length(String::concat(buf, left.as_chr_str(), right.as_chr_str()));
 left.set_new_container(buf);
 return left;*/
//}
