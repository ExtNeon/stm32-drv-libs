/*
 * voltmeter.h
 *
 *  Created on: 7 мая 2021 г.
 *      Author: neovs
 */

#ifndef SRC_LIB_VOLTMETER_VOLTMETER_H_
#define SRC_LIB_VOLTMETER_VOLTMETER_H_

#include <main.h>
#include "../arraylist/arraylist.h"
/*
 *
 #ifndef VOLTMETER_H_
 #define VOLTMETER_H_

 #include "stm32f1xx_hal.h"

 #define VM_ADC_SAMPLES 31
 #define VM_MEASURES_MIDDLE_FILTER_SAMPLES_MAX 41

 struct vm_t {
 uint16_t measures[VM_ADC_SAMPLES];
 uint8_t measure_counter;
 volatile uint16_t *ADC_val_arr;
 volatile uint64_t measures_sum;
 volatile uint16_t measures_sum_count;
 uint8_t ADC_val_index;
 uint16_t ADC_average;
 float voltage;
 uint8_t dev_voltage_changed;
 float vdiv_transfer_coef;
 };

 void voltmeter_create(volatile struct vm_t *vm_instance,
 volatile uint16_t *ADC_input_val_arr, uint8_t ADC_val_index);
 void voltmeter_create_w_res_divider(volatile struct vm_t *vm_instance,
 volatile uint16_t *ADC_input_val_arr, uint8_t ADC_val_index,
 float top_resistor_val, float bottom_resistor_val);
 void voltmeter_process_measurement(volatile struct vm_t *vm_instance);
 uint16_t voltmeter_get_average_ADC_val(volatile struct vm_t *vm_instance);
 float voltmeter_get_voltage(volatile struct vm_t *vm_instance);
 void findMedianN_optim(volatile struct vm_t *vm_instance, uint16_t newVal);
 #endif
 */

/*
 void voltmeter_create(volatile struct vm_t *vm_instance,
 volatile uint16_t *ADC_input_val_arr, uint8_t ADC_val_index) {
 voltmeter_create_w_res_divider(vm_instance, ADC_input_val_arr,
 ADC_val_index, 0, 1);
 }
 void voltmeter_create_w_res_divider(volatile struct vm_t *vm_instance,
 volatile uint16_t *ADC_input_val_arr, uint8_t ADC_val_index,
 float top_resistor_val, float bottom_resistor_val) {
 ADC_average = 0;
 ADC_val_arr = ADC_input_val_arr;
 ADC_val_index = ADC_val_index;
 dev_voltage_changed = 0;
 measures_sum = 0;
 measures_sum_count = 1;
 measure_counter = 0;
 voltage = 0;
 vdiv_transfer_coef = (3.3 / 4096.)
 / (bottom_resistor_val / (top_resistor_val + bottom_resistor_val));
 for (int i = 0; i < VM_ADC_SAMPLES; i++) {
 voltmeter_process_measurement(vm_instance);
 HAL_Delay(1);
 }
 }
 void voltmeter_process_measurement(volatile struct vm_t *vm_instance) {
 findMedianN_optim(vm_instance,
 ADC_val_arr[ADC_val_index]);
 dev_voltage_changed = 1;
 }

 //#define VM_ADC_SAMPLES 10  // порядок медианы
 // медиана на N значений со своим буфером, ускоренный вариант
 void findMedianN_optim(volatile struct vm_t *vm_instance, uint16_t newVal) {
 measures[measure_counter] = newVal;
 if ((measure_counter < VM_ADC_SAMPLES - 1)
 && (measures[measure_counter]
 > measures[measure_counter + 1])) {
 for (int i = measure_counter; i < VM_ADC_SAMPLES - 1;
 i++) {
 if (measures[i] > measures[i + 1]) {
 uint16_t buff = measures[i];
 measures[i] = measures[i + 1];
 measures[i + 1] = buff;
 }
 }
 } else {
 if ((measure_counter > 0)
 && (measures[measure_counter - 1]
 > measures[measure_counter])) {
 for (int i = measure_counter; i > 0; i--) {
 if (measures[i] < measures[i - 1]) {
 uint16_t buff = measures[i];
 measures[i] = measures[i - 1];
 measures[i - 1] = buff;
 }
 }
 }
 }
 if (++measure_counter >= VM_ADC_SAMPLES)
 measure_counter = 0;
 measures_sum +=
 measures[(int) VM_ADC_SAMPLES / 2];
 measures_sum_count++;
 if (measures_sum_count == VM_MEASURES_MIDDLE_FILTER_SAMPLES_MAX) {
 voltmeter_get_average_ADC_val(vm_instance);
 }
 }

 uint16_t voltmeter_get_average_ADC_val(volatile struct vm_t *vm_instance) {

 if (dev_voltage_changed) {
 ADC_average = (uint16_t) ((float) measures_sum
 / (float) measures_sum_count);
 dev_voltage_changed = 0;
 measures_sum = measures[(int) VM_ADC_SAMPLES
 / 2];
 measures_sum_count = 1;
 }
 return ADC_average;
 }
 float voltmeter_get_voltage(volatile struct vm_t *vm_instance) {
 return voltmeter_get_average_ADC_val(vm_instance)
 * (vdiv_transfer_coef);
 }
 */
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
