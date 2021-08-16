/*
 * Nokia5110_LCD - класс - драйвер дисплея Nokia5110/3310.
 * Позволяет работать с дисплеем на максимальной скорости, в зависимости от
 * 		выбранного метода работы.
 * Предоставляет возможность управления как напрямую, так и через сдвиговые регистры,
 * 		при этом скорость работы через регистры значительно ниже. Для уменьшения влияния
 * 		данного эффекта предусмотрен избирательный вывод с двойным буфером - передаются
 * 		только данные о тех страницах (блок 1x8y), которые были изменены.
 *
 * Использование:
 * Конструкторы -
 * 				Nokia5110_LCD(Shift_HC595 shift_reg, uint8_t pin_num_rst,
 *							uint8_t pin_num_ce, uint8_t pin_num_dc, uint8_t pin_num_din,
 *							uint8_t pin_num_clk);
 *				- для работы через сдвиговые регистры (средняя скорость вывода).
 *
 *				Nokia5110_LCD(GPIO_TypeDef *GPIO_PORT_RST, uint16_t GPIO_Pin_RST,
 *							  GPIO_TypeDef *GPIO_PORT_CE, uint16_t GPIO_Pin_CE,
 *							  GPIO_TypeDef *GPIO_PORT_DC, uint16_t GPIO_Pin_DC,
 *							  GPIO_TypeDef *GPIO_PORT_DIN, uint16_t GPIO_Pin_DIN,
 *							  GPIO_TypeDef *GPIO_PORT_CLK, uint16_t GPIO_Pin_CLK);
 *				- для работы напрямую (очень быстро)
 *
 * Методы общего назначения:
 * 							void refreshScr(); - записать на дисплей данные с буфера.
 *
 *							void setPixel(uint8_t x, uint8_t y, bool pixel);
 *							- установить отдельный пиксель.
 *
 *							void clear_buf();
 *							- очистить буфер
 *
 *							void clear_screen();
 *							- очистить экран без очищения буфера.
 *
 *							void set_contrast(uint8_t contrast);
 *							- установить контрастность
 *
 *							void set_temp_coef(uint8_t temp_coef);
 *							- установить алгоритм температурной коррекции
 *
 *							void reinit(bool refresh_screen = true);
 *							- переинициализировать дисплей (нужно в случае использования
 *							в среде сильных помех, чтобы восстановить или предотвратить
 *							потерю соединения с дисплеем.
 *
 *							BoolArray * get_buf();
 *							- получить указатель на видеобуфер. Нужен в экзотических случаях.
 *
 * @author NeoVS
 * @date 20.04.2021
 * @license GPL3 or higher
 */

#ifndef SRC_LIB_NOKIA5110_NOKIA5110_H_
#define SRC_LIB_NOKIA5110_NOKIA5110_H_

#include <main.h>
#include "../hc595/hc595.h"
#include "../../UI/ui.h"
#include <stdlib.h>

#define LCD_COMMAND false
#define LCD_DATA true

#define LCD_SETYADDR 0x40
#define LCD_SETXADDR 0x80
#define LCD_DISPLAY_BLANK 0x08
#define LCD_DISPLAY_NORMAL 0x0C
#define LCD_DISPLAY_ALL_ON 0x09
#define LCD_DISPLAY_INVERTED 0x0D


class Nokia5110_LCD: public ScreenViewOutputInterface {
private:
	GPIO_TypeDef *GPIO_PORT_RST, *GPIO_PORT_CE, *GPIO_PORT_DC, *GPIO_PORT_DIN,
			*GPIO_PORT_CLK;
	uint16_t GPIO_Pin_RST, GPIO_Pin_CE, GPIO_Pin_DC, GPIO_Pin_DIN, GPIO_Pin_CLK;
	bool use_hc595 = false;
	uint8_t pin_num_rst, pin_num_ce, pin_num_dc, pin_num_din, pin_num_clk;
	Shift_HC595 *shift_reg;
	uint8_t buffer[504];
	uint8_t vmem[504];
	uint8_t contrast = 0xB4, temp_coef = 0x03;
	void send(uint8_t val);
	void write(uint8_t data, bool is_data);

	void goXY(uint8_t x, uint8_t y);
	void flush_buffer(bool send_data = false);
public:
	void init();
	const uint16_t size=4032;
	uint16_t get_width() {return 84;};
	uint16_t get_height() {return 48;};
	Nokia5110_LCD(Shift_HC595& shift_reg, uint8_t pin_num_rst,
			uint8_t pin_num_ce, uint8_t pin_num_dc, uint8_t pin_num_din,
			uint8_t pin_num_clk);
	Nokia5110_LCD(GPIO_TypeDef *GPIO_PORT_RST, uint16_t GPIO_Pin_RST,
			GPIO_TypeDef *GPIO_PORT_CE, uint16_t GPIO_Pin_CE,
			GPIO_TypeDef *GPIO_PORT_DC, uint16_t GPIO_Pin_DC,
			GPIO_TypeDef *GPIO_PORT_DIN, uint16_t GPIO_Pin_DIN,
			GPIO_TypeDef *GPIO_PORT_CLK, uint16_t GPIO_Pin_CLK);
	void refreshScr();
	void set_pixel(uint8_t x, uint8_t y, uint16_t pixel);
	void handle_action() {refreshScr();};
	void clear_buf();
	void clear_screen();
	void set_contrast(uint8_t contrast);
	void set_temp_coef(uint8_t temp_coef);
	void reinit(bool refresh_screen = true);
	uint8_t * get_buffer() {return buffer;};
};

#endif /* SRC_LIB_NOKIA5110_NOKIA5110_H_ */
