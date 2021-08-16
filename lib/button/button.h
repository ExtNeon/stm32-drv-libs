/*
 * button.h
 *
 *  Created on: 10 авг. 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_BUTTON_BUTTON_H_
#define SRC_LIB_BUTTON_BUTTON_H_
#include <main.h>

enum BTN_TYPE {
	BUTTON_NORMAL_LOW, BUTTON_NORMAL_HIGH
};

class Button {
	private:
		GPIO_TypeDef *GPIO_PORT = nullptr;
		uint16_t GPIO_Pin = 0;
		bool last_read_state = false;
		bool filtered_state = false;
		bool is_clicked = false;
		bool is_long_clicked = false;
		uint32_t last_state_change_time_ms = 0;
		uint32_t last_filtered_state_change_time_ms = 0;
		uint32_t min_press_duration_ms = 20;
		uint32_t long_press_duration_ms = 1500;
		BTN_TYPE button_type = BUTTON_NORMAL_LOW;
	public:
		bool is_pressed() {
			return filtered_state;
		}

		bool is_clicked() {
			bool is_clicked_copy = is_clicked;
			is_clicked = false;
			return is_clicked_copy;
		}

		bool is_long_pressed() {
			return filtered_state
					&& (HAL_GetTick() - last_filtered_state_change_time_ms
							>= long_press_duration_ms);
		}

		bool is_long_clicked() {
			bool is_long_clicked_copy = is_long_clicked;
			is_long_clicked = false;
			return is_long_clicked_copy;
		}

		void read_button() {
			bool readed_state = HAL_GPIO_ReadPin(GPIO_PORT, GPIO_Pin);
			uint32_t current_time_ms = HAL_GetTick();
			if (readed_state != last_read_state) {
				last_state_change_time_ms = readed_state;
				last_read_state = readed_state;
			} else {
				if (readed_state != filtered_state
						&& current_time_ms - last_state_change_time_ms
								>= min_press_duration_ms) {
					filtered_state = readed_state;
					if ((button_type == BUTTON_NORMAL_LOW && filtered_state)
							|| (button_type == BUTTON_NORMAL_HIGH
									&& !filtered_state)) {
						if (current_time_ms - last_filtered_state_change_time_ms
								< long_press_duration_ms) {
							is_clicked = true;
						} else {
							is_long_clicked = true;
						}
					}
					last_filtered_state_change_time_ms = current_time_ms;
				}
			}
		}

		Button(GPIO_TypeDef GPIO_PORT, uint16_t GPIO_Pin, BTN_TYPE button_type =
				BUTTON_NORMAL_LOW, uint32_t min_press_duration_ms = 20,
				uint32_t long_press_duration_ms = 1500);
};

#endif /* SRC_LIB_BUTTON_BUTTON_H_ */
