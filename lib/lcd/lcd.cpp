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
 *
 * @author NeoVS
 * @date 20.04.2021
 * @license GPL3 or higher
 */

#include "lcd.h"

Nokia5110_LCD::Nokia5110_LCD(Shift_HC595& shift_reg, uint8_t pin_num_rst,
		uint8_t pin_num_ce, uint8_t pin_num_dc, uint8_t pin_num_din,
		uint8_t pin_num_clk) {
	this->pin_num_rst = pin_num_rst;
	this->pin_num_ce = pin_num_ce;
	this->pin_num_dc = pin_num_dc;
	this->pin_num_din = pin_num_din;
	this->pin_num_clk = pin_num_clk;
	use_hc595 = true;
	this->shift_reg = &shift_reg;
}

Nokia5110_LCD::Nokia5110_LCD(GPIO_TypeDef *GPIO_PORT_RST, uint16_t GPIO_Pin_RST,
		GPIO_TypeDef *GPIO_PORT_CE, uint16_t GPIO_Pin_CE,
		GPIO_TypeDef *GPIO_PORT_DC, uint16_t GPIO_Pin_DC,
		GPIO_TypeDef *GPIO_PORT_DIN, uint16_t GPIO_Pin_DIN,
		GPIO_TypeDef *GPIO_PORT_CLK, uint16_t GPIO_Pin_CLK) {
	this->GPIO_PORT_RST = GPIO_PORT_RST;
	this->GPIO_PORT_CE = GPIO_PORT_CE;
	this->GPIO_PORT_DC = GPIO_PORT_DC;
	this->GPIO_PORT_DIN = GPIO_PORT_DIN;
	this->GPIO_PORT_CLK = GPIO_PORT_CLK;

	this->GPIO_Pin_RST = GPIO_Pin_RST;
	this->GPIO_Pin_CE = GPIO_Pin_CE;
	this->GPIO_Pin_DC = GPIO_Pin_DC;
	this->GPIO_Pin_DIN = GPIO_Pin_DIN;
	this->GPIO_Pin_CLK = GPIO_Pin_CLK;
}

void Nokia5110_LCD::send(uint8_t val) {
	if (use_hc595) {
		for (uint8_t i = 0; i < 8; i++) {
			shift_reg->set_pin_and_write(pin_num_din, (val & (1 << (7 - i))));
			shift_reg->set_pin_and_write(pin_num_clk, 1);
			shift_reg->set_pin_and_write(pin_num_clk, 0);
		}
	} else {
		for (uint8_t i = 0; i < 8; i++) {
			HAL_GPIO_WritePin(GPIO_PORT_DIN, GPIO_Pin_DIN, (GPIO_PinState) (val & (1 << (7 - i))));
			HAL_GPIO_WritePin(GPIO_PORT_CLK, GPIO_Pin_CLK, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIO_PORT_CLK, GPIO_Pin_CLK, GPIO_PIN_RESET);
			//writePinFast(GPIO_PORT_CLK, GPIO_Pin_CLK, GPIO_PIN_RESET);
		}
	}
}

void Nokia5110_LCD::write(uint8_t data, bool is_data) {
	if (use_hc595) {
		shift_reg->set_pin_and_write(pin_num_dc, is_data);
		shift_reg->set_pin_and_write(pin_num_ce, 0);
		send(data);
		shift_reg->set_pin_and_write(pin_num_ce, 1);
	} else {
		HAL_GPIO_WritePin(GPIO_PORT_DC, GPIO_Pin_DC, (GPIO_PinState) is_data);
		HAL_GPIO_WritePin(GPIO_PORT_CE, GPIO_Pin_CE, GPIO_PIN_RESET);
		send(data);
		HAL_GPIO_WritePin(GPIO_PORT_CE, GPIO_Pin_CE, GPIO_PIN_SET);
	}
}

void Nokia5110_LCD::init() {
	if (use_hc595) {
		shift_reg->set_pin_and_write(pin_num_rst, 0);
		shift_reg->set_pin_and_write(pin_num_rst, 1);
	} else {
		HAL_GPIO_WritePin(GPIO_PORT_RST, GPIO_Pin_RST, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIO_PORT_RST, GPIO_Pin_RST, GPIO_PIN_SET);
		//writePinFast(GPIO_PORT_RST, GPIO_Pin_RST, true);
	}
	write(0x21, LCD_COMMAND); //LCD extended commands.
	write(contrast, LCD_COMMAND); //set LCD Vop(Contrast).
	write(temp_coef, LCD_COMMAND); //set temp coefficent.
	write(0x14, LCD_COMMAND); //LCD bias mode 1:40.
	write(0x20, LCD_COMMAND); //LCD basic commands.
	write(LCD_DISPLAY_NORMAL, LCD_COMMAND); //LCD normal.
}

void Nokia5110_LCD::clear_screen() {
	goXY(LCD_SETXADDR, LCD_SETYADDR);
	for (uint32_t i = 0; i < 504; i++) {
		write(0, LCD_DATA);
		vmem[i] = 0;
	}
}

void Nokia5110_LCD::goXY(uint8_t x, uint8_t y) {
	write(0x80 | x, LCD_COMMAND); //Column.
	write(0x40 | y, LCD_COMMAND); //Row.
}

void Nokia5110_LCD::refreshScr() {
	uint8_t y=0, x=0;
	bool page_skipped = false;
	goXY(LCD_SETXADDR, LCD_SETYADDR);
	for (uint16_t page = 0; page < 504; page++) {
			//uint16_t base = y * 84 + x;
			if (buffer[page] != vmem[page]) {
				if (page_skipped) {
					goXY(x, y);
				}
				write(buffer[page], LCD_DATA);
				vmem[page] = buffer[page];
				page_skipped = false;
			} else page_skipped = true;
			if (++x == 84) {
				x=0;
				y++;
			}
	}
	/*for (int j = 0; j < 6;) {
	 for (int i = 0; i < 84; i++) {
	 write(i % 2 == 0 ? 0b11110000 : 0b00001111, LCD_DATA);
	 }
	 j++;
	 }*/
}

void Nokia5110_LCD::set_pixel(uint8_t x, uint8_t y, bool pixel) {
	//page_i=y/8;
	//page_shift = page_i*get_width() + x;
	//bit_num=page_shift+(7-y)
	//p=((y/8)*get_width() + x)+(7-y)
	//p=7-y+x*8
/*	buffer->set_bit((x + (y / 8) *get_width())*8 +(7-y),
				pixel);*/
	/*buffer->set_bit(x * 8 + (7 - (y - ((y / 8) * 8))) + y / 8 * (get_width() * 8),
			pixel);*/
	if (x >= get_width())
			x = get_width() - 1;
		if (y >= get_height())
			y = get_height() - 1;

		if (pixel) {
			buffer[x + (y / 8) * get_width()] |= 1 << (y % 8);
		} else {
			buffer[x + (y / 8) * get_width()] &= ~(1 << (y % 8));
		}
}

void Nokia5110_LCD::clear_buf() {
	for (uint32_t i = 0; i < 504; i++) {
			buffer[i] = 0;
		}
}

void Nokia5110_LCD::set_contrast(uint8_t contrast) {
	this->contrast = contrast;
	init();
}

void Nokia5110_LCD::set_temp_coef(uint8_t temp_coef) {
	this->temp_coef = temp_coef;
	init();
}

void Nokia5110_LCD::reinit(bool refresh_screen) {
	init();
	if (refresh_screen) {
		refreshScr();
	}
}

