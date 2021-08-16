/*
 * speaker.h
 *
 *  Created on: Aug 12, 2021
 *      Author: neovs
 */

#ifndef SRC_LIB_SPEAKER_SPEAKER_H_
#define SRC_LIB_SPEAKER_SPEAKER_H_
#include <main.h>
#include "../arraylist/arraylist.h"

struct melody_note_t {
		uint16_t frequency, duration;
};

class Speaker {
	private:
		TIM_HandleTypeDef *timer = nullptr;
		uint32_t tim_channel = TIM_CHANNEL_1;
		ArrayList<melody_note_t> melody;
		uint16_t melody_current_note = 65535;
		uint64_t melody_note_millis_counter = 0;
		uint16_t melody_repeat_count = 0;
		bool melody_non_overrideable = false;
		void play_note(uint16_t frequency);
	public:
		void beep(uint64_t duration, uint16_t freq) {
			uint16_t dur[1] = { duration };
			uint16_t freqs[1] = { freq };
			play_melody(freqs, dur, 1);
		}
		void play_melody(uint16_t freq[], uint16_t durations[], uint8_t length,
				bool prioritized = false, uint16_t repeat_count = 0) {
			if (melody_non_overrideable && melody.size())
				return;
			melody.clear();
			for (int i = 0; i < length; i++) {
				melody.addNew( { durations[i], freq[i] });
			}
			melody_current_note = 0;
			melody_note_millis_counter = 0;
			melody_repeat_count = repeat_count;
			melody_non_overrideable = prioritized;
			play_note(freq[0]);
		}
		void stop_melody();
		void process_melody_ms() {
			if (melody.size()) {
				if (++melody_note_millis_counter
						>= melody.getElement(melody_current_note).duration) {
					if (++melody_current_note >= melody.size()) {
						//END MELODY
						if (melody_repeat_count) {
							melody_repeat_count--;
							melody_current_note = 0;
							melody_note_millis_counter = 0;
							play_note(
									melody.getElement(melody_current_note).frequency);
						} else {
							stop_melody();
						}
					} else {
						//NEXT NOTE
						play_note(
								melody.getElement(melody_current_note).frequency);
						melody_note_millis_counter = 0;
					}
				}
			}
		}
};

#endif /* SRC_LIB_SPEAKER_SPEAKER_H_ */
