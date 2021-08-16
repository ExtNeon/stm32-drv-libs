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

#ifndef SRC_HC595_HC595_H_
#define SRC_HC595_HC595_H_

#include <main.h>

#include "../boolarray/boolarray.h"

class Shift_HC595 {
private:
	BoolArray shift_state; //MAX - 8 shift registers in serial
	GPIO_TypeDef *GPIO_PORT_Latch=nullptr, *GPIO_PORT_CLK=nullptr, *GPIO_PORT_Data=nullptr;
	uint16_t GPIO_Pin_Latch=0, GPIO_Pin_CLK=0, GPIO_Pin_Data=0;
	uint16_t pin_count=0;
	SPI_HandleTypeDef *hspi = nullptr;
	uint8_t *spi_reverse_buf = nullptr;
	bool use_spi = false;
public:
	Shift_HC595(uint16_t pin_count=0, GPIO_TypeDef *GPIO_PORT_Latch=nullptr,
			uint16_t GPIO_Pin_Latch=0, GPIO_TypeDef *GPIO_PORT_CLK=nullptr,
			uint16_t GPIO_Pin_CLK=0, GPIO_TypeDef *GPIO_PORT_Data=nullptr,
			uint16_t GPIO_Pin_Data=0);
	Shift_HC595(uint16_t pin_count, SPI_HandleTypeDef &hspi,  GPIO_TypeDef *GPIO_PORT_Latch,
			uint16_t GPIO_Pin_Latch);
	void set_pin(uint16_t pin_num, bool pin_state);
	bool read_pin(uint16_t pin_num);
	bool set_pin_and_write(uint16_t pin_num, bool pin_state);
	void write();
	void reset();
	void invert();
	void invert(uint16_t pin_num);
	uint16_t get_pin_count() {return pin_count;};
	~Shift_HC595() {delete[] spi_reverse_buf; shift_state.~BoolArray();};
};

#endif /* SRC_HC595_HC595_H_ */
