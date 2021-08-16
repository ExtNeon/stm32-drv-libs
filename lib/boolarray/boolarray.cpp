/*
 * BoolArray - класс - недоконтейнер, упрощающий работу с битовыми картами.
 * Экономит память и достаточно эффективно работает с отдельными битами.
 * Использование:
 * Конструктор - BoolArray(uint64_t size). Параметр size - количество битов в карте.
 * Чтение и запись отдельных битов - void set_bit(uint64_t pos, bool state);
 *									 bool read_bit(uint64_t pos);
 * Можно получить доступ к массиву с байтами, хранящему данные:
 * 									 uint8_t * get_as_bytes();
 * Размер массива: 					 uint32_t get_size_bytes();
 * Массовые операции:
 * 		Инвертировать все биты: 	 void invert();
 * 		Сбросить все биты в false:   void reset();
 *
 * @author NeoVS
 * @date 20.04.2021
 * @license GPL3 or higher
 */

#include "boolarray.h"

BoolArray::BoolArray(uint64_t size) {
	size_bytes = size/8;
	if (size - (size_bytes*8) > 0) {
		size_bytes++;
	}
	byte_array = new uint8_t[size_bytes];
	size_bits = size;
}

void BoolArray::set_bit(uint64_t pos, bool state) {
	uint32_t bytenum = pos / 8;
	uint32_t bitnum = pos - bytenum * 8;
	uint8_t mask = 0;
	if (state) {
		mask = (0b10000000 >> bitnum);
		byte_array[bytenum] |= mask;
	} else {
		mask = ~(0b10000000 >> bitnum);
		byte_array[bytenum] &= mask;
	}

}

bool BoolArray::read_bit(uint64_t pos) {
	uint32_t bytenum = pos / 8;
	if (bytenum >= size_bytes)
		return false;
	uint8_t bitnum = pos - bytenum * 8;
	uint8_t mask = 0b10000000 >> bitnum;
	return byte_array[bytenum] & mask;
}

uint8_t * BoolArray::get_as_bytes() {
	return byte_array;
}
void BoolArray::reset() {
	for (uint32_t i = 0; i < size_bytes; i++) {
		byte_array[i] = 0;
	}
}

uint32_t BoolArray::get_size_bytes() {
	return size_bytes;
}

void BoolArray::invert() {
	for (uint32_t i = 0; i < size_bytes; i++) {
			byte_array[i] = ~byte_array[i];
		}
}

