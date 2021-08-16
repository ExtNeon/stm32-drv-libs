/*
 * voltmeter.h
 *
 *  Created on: 7 мая 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_VOLTMETER_VOLTMETER_H_
#define SRC_LIB_VOLTMETER_VOLTMETER_H_

#include <main.h>
//#include "../tools/arraylist/arraylist.h"

//
// (1.20 * 4096.0 / x) / 4096
// 1.20/vref
class Voltmeter {
	private:
		uint8_t measures_middle_filter_max = 41;
		uint16_t *measures = nullptr;
		//ArrayList<uint16_t> measures;
		uint8_t sample_count = 31;
		uint8_t current_measure_index = 0;
		uint16_t *DMA_connected_ADC_val = nullptr;
		uint64_t measures_sum = 0;
		uint16_t measures_sum_count = 0;
		uint16_t ADC_average = 0;
		uint8_t dev_voltage_changed = false;
		float vdiv_transfer_coef = 1;
		Voltmeter *voltage_reference_voltmeter = nullptr;
		void findMedianN_optim(const uint16_t newVal) {
			measures[current_measure_index] = newVal;
			if ((current_measure_index < sample_count - 1)
					&& (measures[current_measure_index]
							> measures[current_measure_index + 1])) {
				for (int i = current_measure_index; i < sample_count - 1; i++) {
					if (measures[i] > measures[i + 1]) {
						uint16_t buff = measures[i];
						measures[i] = measures[i + 1];
						measures[i + 1] = buff;
					}
				}
			} else {
				if ((current_measure_index > 0)
						&& (measures[current_measure_index - 1]
								> measures[current_measure_index])) {
					for (int i = current_measure_index; i > 0; i--) {
						if (measures[i] < measures[i - 1]) {
							uint16_t buff = measures[i];
							measures[i] = measures[i - 1];
							measures[i - 1] = buff;
						}
					}
				}
			}
			if (++current_measure_index >= sample_count)
				current_measure_index = 0;
			measures_sum += measures[(int) sample_count / 2];
			measures_sum_count++;
			if (measures_sum_count == measures_middle_filter_max) {
				get_average_ADC_val();
			}
		}
	public:
		Voltmeter(uint16_t *VREF_val, uint8_t samples = 31,
				uint8_t middle_filter_measures_max = 41) {
			this->DMA_connected_ADC_val = VREF_val;
			this->measures_middle_filter_max = middle_filter_measures_max;
			this->sample_count = samples;
			if (this->sample_count == 0)
				this->sample_count = 1;
			this->measures = new uint16_t[this->sample_count];
			for (int i = 0; i < samples; i++) {
				process_measurement();
				HAL_Delay(1);
			}
		}
		Voltmeter(Voltmeter *vref_vm, uint16_t *ADC_val, float top_resistor_r =
				0, float bottom_resistor_r = 1, uint8_t samples = 31,
				uint8_t middle_filter_measures_max = 41) {
			vdiv_transfer_coef = (bottom_resistor_r
					/ (top_resistor_r + bottom_resistor_r));
			voltage_reference_voltmeter = vref_vm;
			this->DMA_connected_ADC_val = ADC_val;
			this->measures_middle_filter_max = middle_filter_measures_max;
			this->sample_count = samples;
			if (this->sample_count == 0)
				this->sample_count = 1;
			this->measures = new uint16_t[this->sample_count];
			for (int i = 0; i < samples; i++) {
				process_measurement();
				HAL_Delay(1);
			}
		}
		void process_measurement() {
			dev_voltage_changed = true;
			findMedianN_optim(*DMA_connected_ADC_val);
		}

		uint16_t get_average_ADC_val() {

			if (dev_voltage_changed) {
				ADC_average = (uint16_t) ((float) measures_sum
						/ (float) measures_sum_count);
				dev_voltage_changed = false;
				measures_sum = measures[(int) sample_count / 2];
				measures_sum_count = 1;
			}
			return ADC_average;
		}
		float get_voltage() {
			if (voltage_reference_voltmeter == nullptr)
				return (1.20 * 4096.) / (float) get_average_ADC_val();
			float vref_coef = 1.20
					/ (float) voltage_reference_voltmeter->get_average_ADC_val();
			return get_average_ADC_val()
					* (vref_coef / vdiv_transfer_coef);
		}
};

#endif /* SRC_LIB_VOLTMETER_VOLTMETER_H_ */
