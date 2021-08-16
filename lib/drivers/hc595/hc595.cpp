/*
 * Shift_HC595 - класс - драйвер, автоматизирущий работу со сдвиговыми регистрами
 * 		*HC595.
 * Позволяет работать с регистрами на максимальной скорости, в зависимости от
 * 		выбранного метода работы.
 * Представляет регистры подключённые последовательно, как набор пинов. Отсчёт начинается
 * 		с 0 - это Q0 первого регистра.
 * Позволяет массово задать состояние пинов, а затем применить это состояние в
 * 		один момент.
 * Чем больше ног - тем ниже скорость, поскольку даже при изменении состояния
 * 		одного пина в произвольном месте, необходимо записать в регистры
 * 		состояния всех ног!
 * Поддерживает работу с регистрами через SPI, а также софтовую реализацию, если все
 * 		SPI у вас оказались заняты. При этом скорость обмена значительно снизится!
 * При работе через SPI, максимальная скорость интерфейса: 4.5 Мбит/с. Если превысить,
 * 		то стабильность значительно снижается, вплоть до невозможности управления
 * 		регистрами вообще. Имейте ввиду!
 * Параметры SPI:
 * 	* Mode = SPI_MODE_MASTER;
 * 	* DataSize = SPI_DATASIZE_8BIT;
 * 	* CLKPolarity = SPI_POLARITY_LOW;
 * 	* CLKPhase = SPI_PHASE_1EDGE;
 * 	* FirstBit = SPI_FIRSTBIT_LSB;
 * 	* BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; //Справедливо для F103C8T6 на 72МГц.
 *
 * Использование:
 * Конструкторы - Shift_HC595(uint16_t pin_count, GPIO_TypeDef *GPIO_PORT_Latch,
 *							  uint16_t GPIO_Pin_Latch, GPIO_TypeDef *GPIO_PORT_CLK,
 *							  uint16_t GPIO_Pin_CLK, GPIO_TypeDef *GPIO_PORT_Data,
 *							  uint16_t GPIO_Pin_Data);
 *							  - для реализации софтовым методом.
 *
 *				  Shift_HC595(uint16_t pin_count, SPI_HandleTypeDef &hspi,
 *				  	  	  	  GPIO_TypeDef *GPIO_PORT_Latch, uint16_t GPIO_Pin_Latch);
 *				  	  	  	  - для реализации через SPI.
 *				  Конструктор по умолчанию нужен только по требованию C++.
 *				  ИСПОЛЬЗОВАТЬ ЕГО НЕЛЬЗЯ!
 *
 * Чтение и запись отдельных пинов - void set_pin(uint16_t pin_num, bool pin_state);
 *									 bool read_pin(uint16_t pin_num);
 *									 - для установки состояния пина, но запись при этом
 *									 не происходит. Удобно, когда нужно одновременно
 *									 записать состояние нескольких пинов.
 *
 * 							bool set_pin_and_write(uint16_t pin_num, bool pin_state);
 * 							- Сразу же записывает состояние пина в регистр,
 * 							  но только если его состояние поменялось! Поэтому его нельзя
 * 							  применять в случаях, когда до этого вы изменили состояния
 * 							  других пинов методом set_pin и не вызвали write, поскольку
 * 							  их изменение он не учитывает!
 * 							  Возвращает true, если записал данные в регистр, и false,
 * 							  если нет.
 *
 * 							 void write();
 * 							 - применить изменения, и записать их в регистр.
 *
 * Массовые операции:
 * 							void reset(); - сбросить все выходы в false.
 *							void invert(); - инвертировать все выходы.
 *							void invert(uint16_t pin_num); - инвертировать отдельный пин.
 *
 *
 * @author NeoVS
 * @date 20.04.2021
 * @license GPL3 or higher
 */

#include "hc595.h"

Shift_HC595::Shift_HC595(uint16_t pin_count, GPIO_TypeDef *GPIO_PORT_Latch,
		uint16_t GPIO_Pin_Latch, GPIO_TypeDef *GPIO_PORT_CLK,
		uint16_t GPIO_Pin_CLK, GPIO_TypeDef *GPIO_PORT_Data,
		uint16_t GPIO_Pin_Data) {
	this->pin_count = pin_count;
	if (pin_count > 0) {
		this->GPIO_PORT_CLK = GPIO_PORT_CLK;
		this->GPIO_PORT_Data = GPIO_PORT_Data;
		this->GPIO_PORT_Latch = GPIO_PORT_Latch;
		this->GPIO_Pin_CLK = GPIO_Pin_CLK;
		this->GPIO_Pin_Data = GPIO_Pin_Data;
		this->GPIO_Pin_Latch = GPIO_Pin_Latch;
		shift_state = BoolArray(pin_count);
		this->reset();
	}
}

Shift_HC595::Shift_HC595(uint16_t pin_count, SPI_HandleTypeDef &hspi,
		GPIO_TypeDef *GPIO_PORT_Latch, uint16_t GPIO_Pin_Latch) {

	this->pin_count = pin_count;
	if (pin_count > 0) {
		this->GPIO_PORT_Latch = GPIO_PORT_Latch;
		this->GPIO_Pin_Latch = GPIO_Pin_Latch;
		shift_state = BoolArray(pin_count);
		this->hspi = &hspi;
		use_spi = true;
		spi_reverse_buf = new uint8_t[shift_state.get_size_bytes()];
		this->reset();
	}
}

bool __inline__ Shift_HC595::read_pin(uint16_t pin_num) {
	if (pin_num < pin_count)
		return shift_state.read_bit(pin_num);
	else
		return false;
}

void Shift_HC595::reset() {
	if (pin_count > 0) {
		shift_state.reset();
		HAL_GPIO_WritePin(GPIO_PORT_Latch, GPIO_Pin_Latch, GPIO_PIN_RESET); //Запрещаем изменения
		write();
	}
}

void Shift_HC595::invert() {
	if (pin_count > 0) {
		shift_state.invert();
		write();
	}
}

void Shift_HC595::invert(uint16_t pin_num) {
	if (pin_num < pin_count) {
		shift_state.set_bit(pin_num, !shift_state.read_bit(pin_num));
		write();
	}
}

void __inline__ Shift_HC595::set_pin(uint16_t pin_num, bool pin_state) {
	if (pin_num < pin_count)
		shift_state.set_bit(pin_num, pin_state);
}

bool Shift_HC595::set_pin_and_write(uint16_t pin_num, bool pin_state) {
	if (pin_num < pin_count) {
		if (shift_state.read_bit(pin_num) != pin_state) {
			set_pin(pin_num, pin_state);
			write();
			return true;
		}
	}
	return false;
}

void Shift_HC595::write() {
	if (pin_count > 0) {
		if (use_spi) {
			uint8_t *shift_state_buf = shift_state.get_as_bytes();
			uint16_t shift_state_size_bytes = shift_state.get_size_bytes();
			uint16_t shift_state_buf_index = 0;
			for (int i = shift_state_size_bytes - 1; i >= 0; i--) {
				spi_reverse_buf[i] = shift_state_buf[shift_state_buf_index++];
			}
			HAL_SPI_Transmit(hspi, spi_reverse_buf,
					shift_state.get_size_bytes(), 1000);
		} else {
			for (uint16_t i = pin_count; i > 0; i--) {
				HAL_GPIO_WritePin(GPIO_PORT_Data, GPIO_Pin_Data,
						(GPIO_PinState) shift_state.read_bit(i - 1)); //Устанавливаем состояние
				HAL_GPIO_WritePin(GPIO_PORT_CLK, GPIO_Pin_CLK, GPIO_PIN_SET); //Тактирование вверх
				HAL_GPIO_WritePin(GPIO_PORT_CLK, GPIO_Pin_CLK, GPIO_PIN_RESET); //Тактирование вниз
			}
		}
		HAL_GPIO_WritePin(GPIO_PORT_Latch, GPIO_Pin_Latch, GPIO_PIN_SET); //Применяем состояние на выход
		HAL_GPIO_WritePin(GPIO_PORT_Latch, GPIO_Pin_Latch, GPIO_PIN_RESET); //Запрещаем изменения
	}
}

