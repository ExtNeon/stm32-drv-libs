/*
 * arraylist.h
 *
 *  Created on: 25 апр. 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_ARRAYLIST_ARRAYLIST_H_
#define SRC_LIB_ARRAYLIST_ARRAYLIST_H_

#include <main.h>
#include <stdlib.h>
#include <cstring>

template<typename Type>
class ArrayList {
	private:
		uint16_t count = 0;
	public:
		Type *arr = nullptr;
		void addNew(Type element);
		void remove(Type element);
		void removeLast() {
			if (count > 0)
				remove(arr[count - 1]);
		}
		void clear() {
			count = 0;
			delete[] arr;
			arr = nullptr;
		}
		Type* getArray() {
			return arr;
		}
		uint16_t size() {
			return count;
		}
		Type getElement(uint16_t index) {
			return arr[index];
		}
		Type & operator[](int i) {
			return arr[i];
		}
		~ArrayList<Type>() {
			delete[] arr;
		}
};

template<typename Type>
void ArrayList<Type>::addNew(Type element) {
	Type *new_arr = new Type[count + 1];
	if (new_arr != nullptr) { //память успешно выделена
		if (count > 0) memcpy(new_arr, arr, count * sizeof(Type));
		count++;
		new_arr[count - 1] = element;
		delete[] arr;
		arr = new_arr;
	}
}

template<typename Type>
void ArrayList<Type>::remove(Type element) {
	int16_t index = -1;
	for (uint16_t i = 0; i < count; i++) {
		if (arr[i] == element) {
			index = i;
			break;
		}
	}
	if (index >= 0) {
		Type *new_arr = new Type[count - 1];
		if (new_arr != nullptr) {
			uint16_t j = index;
			index = 0;
			for (uint16_t i = 0; i < count; i++) {
				if (i != j) {
					new_arr[index++] = arr[i];
				}
			}
			delete[] arr;
			arr = new_arr;
			count--;
		}
	}
}

#endif /* SRC_LIB_ARRAYLIST_ARRAYLIST_H_ */
