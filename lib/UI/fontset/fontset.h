/*
 * fontset.h
 *
 *  Created on: 26 апр. 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_FONTSET_FONTSET_H_
#define SRC_LIB_FONTSET_FONTSET_H_

typedef struct { //Метаданные
		wchar_t assoc;
		uint8_t width,height;
		uint16_t page_start;
		uint16_t size;
		const uint8_t *data_pages;
} fontset_character;


#endif /* SRC_LIB_FONTSET_FONTSET_H_ */
