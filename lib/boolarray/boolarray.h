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

#ifndef SRC_LIB_BOOLARRAY_BOOLARRAY_H_
#define SRC_LIB_BOOLARRAY_BOOLARRAY_H_
#include <main.h>

class BoolArray {
private:
	uint8_t *byte_array = new uint8_t[0];
	uint32_t size_bytes = 1;
	uint64_t size_bits;
public:
	BoolArray(uint64_t size = 0);
	void set_bit(uint64_t pos, bool state);
	bool read_bit(uint64_t pos);
	uint8_t * get_as_bytes();
	uint32_t get_size_bytes();
	void reset();
	void invert();
	~BoolArray() {delete[] byte_array;};
};



#endif /* SRC_LIB_BOOLARRAY_BOOLARRAY_H_ */
