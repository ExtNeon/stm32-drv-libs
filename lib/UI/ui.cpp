/*
 * ui.cpp
 *
 *  Created on: Apr 21, 2021
 *      Author: neovs
 */

#include "ui.h"

void ScreenView::draw() {
	draw_custom();
	if (children_count > 0) {
		for (uint8_t l = 0; l < 8; l++) { //MAX=4layers
			for (uint8_t c = 0; c < children_count; c++) {
				ScreenView *child = children[c];
				if (child->layer == l && child->visible) {
					child->draw();
				}
			}
		}
	}
}

void ScreenView::update_position() {
	int16_t parent_width = 0;
	int16_t parent_height = 0;
	if (parent != nullptr) {
		parent_width = parent->width;
		parent_height = parent->height;
	} else if (interface != nullptr) {
		parent_width = interface->get_width();
		parent_height = interface->get_height();
	}
	if (ScreenView_get_width() == -1 || ScreenView_get_height() == -1) {
		this->width = parent_width;
		this->height = parent_height;
	}
	/*
	 *
	AUTO_IN_LIST_LEFT,
	AUTO_IN_LIST_RIGHT,
	AUTO_IN_LIST_CENTER,
	AUTO_TOP_CENTER,
	AUTO_BOTTOM_CENTER,
	AUTO_LEFT_CENTER,
	AUTO_RIGHT_CENTER,
	AUTO_IN_SERIES_TOP,
	AUTO_IN_SERIES_BOTTOM,
	AUTO_IN_SERIES_CENTER,
	AUTO_CORNER_TOP_LEFT,
	AUTO_CORNER_TOP_RIGHT,
	AUTO_CORNER_BOTTOM_LEFT,
	AUTO_CORNER_BOTTOM_RIGHT,
	 */
	switch (position) { //TODO дописать
	case AUTO_CENTER:
		View_set_position(parent_width / 2 - width / 2,
				parent_height / 2 - height / 2);
		break;
	case AUTO_CORNER_TOP_LEFT:
		View_set_position(parent_width - width - 1, 0);
		break;
	case AUTO_TOP_CENTER:
			View_set_position(parent_width - width - 1, 0);
			break;
	case CONSTANT:
	default:
		break;
	}
}

void ScreenView::View_set_position(int16_t x, int16_t y) {
	this->x = x;
	this->y = y;
}

void ScreenView::View_set_x(int16_t x) {
	this->x = x;
}

void ScreenView::View_set_y(int16_t y) {
	this->y = y;
}

void ScreenView::ScreenView_set_dimension(int16_t width, int16_t height) {
	this->width = width;
	this->height = height;
	update_position();
}

void ScreenView::ScreenView_set_width(int16_t width) {
	this->width = width;
	update_position();
}

void ScreenView::ScreenView_set_height(int16_t height) {
	this->height = height;
	update_position();
}

void ScreenView::ScreenView_set_layer(uint8_t layer) {
	this->layer = layer;
}

void ScreenView::ScreenView_set_parent(ScreenView *parent) {
	if (parent != nullptr) {
		if (this->parent != nullptr) {
			this->parent->ScreenView_remove_child(this);
		}
		this->parent = parent;
		parent->ScreenView_add_child(this);
	}
}

void ScreenView::ScreenView_set_children_count(uint8_t count) {
	children_count = count;
}

void ScreenView::ScreenView_set_output_interface(
		ScreenViewOutputInterface *interface) {
	this->interface = interface;
}

void ScreenView::ScreenView_set_position_policy(UI_Position position_policy) {
	this->position = position_policy;
	update_position();
}

void ScreenView::ScreenView_set_visibility(bool visible) {
	this->visible = visible;
}

void ScreenView::ScreenView_add_child(ScreenView *child) {
	bool already_in = false;
	for (uint8_t i = 0; i < children_count; i++) {
		if (children[i] == child) {
			already_in = true;
			break;
		}
	}
	if (!already_in) {
		children[children_count++] = child;
	}
}

void ScreenView::ScreenView_remove_child(ScreenView *child) {
	for (uint8_t i = 0; i < children_count; i++) {
		if (children[i] == child) {
			for (uint8_t j = i; j < children_count - 1; j++) {
				children[j] = children[j + 1];
			}
			children_count--;
			break;
		}
	}
}

void ScreenView::ScreenView_set_pixel(int16_t x, int16_t y, uint16_t set_color,
		ScreenView *sender) {

	x += this->x;
	y += this->y;
	bool is_inside = true;
	if (sender != nullptr) {
		is_inside =
				!check_display_bounds
						|| (x >= this->x + child_draw_offset_x
								&& y >= this->y + child_draw_offset_y
								&& x
										<= this->x + width
												- child_draw_offset_from_width
								&& y
										<= this->y + height
												- child_draw_offset_from_height);
	}
	if (is_inside) {
		if (interface == nullptr) {
			if (parent != nullptr) {
				parent->ScreenView_set_pixel(x, y, set_color, this);
			}
		} else {
			interface->set_pixel(x, y, set_color);
		}
	}
}

int16_t ScreenView::ScreenView_get_width() {
	return width;
}

int16_t ScreenView::ScreenView_get_height() {
	return height;
}

int16_t ScreenView::ScreenView_get_x() {
	return x;
}

int16_t ScreenView::ScreenView_get_y() {
	return y;
}

uint8_t ScreenView::ScreenView_get_layer() {
	return layer;
}

bool ScreenView::ScreenView_get_visibility() {
	return visible;
}

ScreenView** ScreenView::ScreenView_get_children() {
	return children;
}

ScreenView* ScreenView::ScreenView_get_parent() {
	return parent;
}

uint8_t ScreenView::ScreenView_get_children_count() {
	return children_count;
}

ScreenViewOutputInterface* ScreenView::ScreenView_get_output_interface() {
	return interface;
}

UI_Position ScreenView::ScreenView_get_position_policy() {
	return position;
}

ScreenView::ScreenView(ScreenView *parent, int16_t width, int16_t height,
		uint8_t layer, int16_t x, int16_t y) {
	ScreenView_set_parent(parent);
	ScreenView_set_layer(layer);
	View_set_position(x, y);
	ScreenView_set_dimension(width, height);
}

ScreenView::ScreenView(ScreenViewOutputInterface *interface, int16_t width,
		int16_t height, uint8_t layer, int16_t x, int16_t y) {
	ScreenView_set_output_interface(interface);
	ScreenView_set_layer(layer);
	View_set_position(x, y);
	ScreenView_set_dimension(width, height);
}

ScreenView::~ScreenView() {
	if (parent != nullptr) {
		parent->ScreenView_remove_child(this);
	}
}

unsigned int PRNG() {
	// Наше стартовое число - 4 541
	static unsigned int seed = 4541;

	// Берем стартовое число и, с его помощью, генерируем новое значение.
	// Из-за использования очень больших чисел (и переполнения) угадать следующее число исходя из предыдущего - очень сложно
	seed = (8253729 * seed + 2396403);

	// Берем стартовое число и возвращаем значение в диапазоне от 0 до 32767
	return seed % 32768;
}



void BackgroundRandomBlurView::draw_custom() {
	static std::random_device rd;
	static std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом
	static std::uniform_int_distribution<int> range(0, 65535);
	/*static int8_t ayi = 0;
	 if (ayi++ > 0) ayi = 0;*/
	for (int8_t ax = 0; ax < ScreenView_get_width() - 1; ax++) {
		for (int8_t ay = 0; ay < ScreenView_get_height() - 1; ay++) {
			if (mersenne() % (32 - noise_level) == 0)
				ScreenView_set_pixel(ax, ay,
						range(mersenne));
		}
	}
}

void RectView::draw_horizontal_line(int8_t x, int8_t y, int8_t length) {
	for (int8_t ax = x; ax < x + length; ax++) {
		ScreenView_set_pixel(ax, y, ScreenView_get_foreground_color());
	}
}

void RectView::draw_vertical_line(int8_t x, int8_t y, int8_t length) {
	for (int8_t ay = y; ay < y + length; ay++) {
		ScreenView_set_pixel(x, ay, ScreenView_get_foreground_color());
	}
}

int RectView::abs(int x) {
	if (x < 0) {
		return x * -1;
	}
	return x;
}

void RectView::draw_custom() {
	if (!offsets_setted) {
		ScreenView_set_child_draw_offsets(1, 1, 1, 1);
		offsets_setted = true;
	}
	draw_horizontal_line(0, 0, ScreenView_get_width() - 1);
	draw_horizontal_line(0, ScreenView_get_height() - 1,
			ScreenView_get_width() - 1);
	draw_vertical_line(0, 0, ScreenView_get_height());
	draw_vertical_line(ScreenView_get_width() - 1, 0,
			ScreenView_get_height() - 1);
	if (fill)
		for (int8_t x = 1; x < ScreenView_get_width() - 1; x++)
			for (int8_t y = 1; y < ScreenView_get_height() - 1; y++)
				ScreenView_set_pixel(x, y, ScreenView_get_background_color());
}

fontset_character get_font_character_for_char(wchar_t input,
		const fontset_character font[]) {
	uint16_t i = 0;
	while (font[i].assoc != '\0') {
		if (font[i].assoc == input)
			return font[i];
		i++;
	}
	return font[i];
}

bool getBitstateOfByteStream(const uint8_t *stream, uint16_t size,
		uint16_t index) {
	uint16_t bytenum = index / 8;
	if (bytenum >= size)
		return 255;
	uint8_t bitnum = index - bytenum * 8;
	uint8_t mask = 0b10000000 >> bitnum;
	return stream[bytenum] & mask;
}

void draw_character(ScreenView *view, fontset_character character, int8_t x_pos,
		int8_t y_pos, bool invert) {
	uint8_t width = character.width;
	uint8_t height = character.height;
	uint16_t size = character.size;
	const uint8_t *pages = &(character.data_pages[character.page_start]);
	uint16_t index = 0;
	int8_t max_height = height + y_pos, max_width = width + x_pos;
	for (int8_t y = y_pos; y < max_height; y++) {
		for (int8_t x = x_pos; x < max_width; x++) {
			if (invert)
				view->ScreenView_set_pixel(x, y,
						getBitstateOfByteStream(pages, size, index++) ?
								view->ScreenView_get_background_color() :
								view->ScreenView_get_foreground_color());
			else
				view->ScreenView_set_pixel(x, y,
						getBitstateOfByteStream(pages, size, index++) ?
								view->ScreenView_get_foreground_color() :
								view->ScreenView_get_background_color());
		}
	}
}

Dimension get_string_dimension(const fontset_character *font,
		const wchar_t *str, const int16_t x, const int16_t y,
		const int16_t width_limit, bool break_line_if_border_reached,
		int8_t new_line_indent, int8_t interval_between_strings) {
	fontset_character character;
	int16_t internal_x = x + new_line_indent;
	int16_t internal_y = y;
	uint16_t last_width = 0;
	uint16_t last_height = internal_y;
	int16_t max_height = 0;
	uint16_t len = String::strlen(str);
	for (uint16_t i = 0; i < len; i++) {
		if (str[i] == '\n') {
			internal_x = x + new_line_indent;
			if ((str[i + 1] == L',' || str[i + 1] == L';')
					&& new_line_indent < 0) {
				internal_x = internal_x - new_line_indent;
			}
			internal_y += max_height + interval_between_strings;
			max_height = 0;
		} else {
			character = get_font_character_for_char(str[i], font);
			//last_width += character.width;
			if (character.height > max_height)
				max_height = character.height;
			if (internal_x >= width_limit - character.width
					&& break_line_if_border_reached) {
				internal_x = x + new_line_indent;
				if ((str[i + 1] == L',' || str[i + 1] == L';')
						&& new_line_indent < 0) {
					internal_x = internal_x - new_line_indent;
				}
				internal_y += max_height + interval_between_strings;
				max_height = 0;
				if (str[i] == L' ') {
					continue;
				}
			}
			internal_x += character.width;
			if (internal_x + 1 > last_width)
				last_width = internal_x + 1;
		}
	}
	last_height =
			last_height == internal_y ? max_height : internal_y - last_height;
	return {last_width, last_height};
	/*view->set_last_height(last_height);
	 view->set_last_width(last_width);*/
}

void draw_string(ScreenView *view, const int16_t x, const int16_t y,
		const int16_t width_limit, const int16_t height_limit,
		const fontset_character *font, const wchar_t *str, bool invert,
		bool break_line_if_border_reached, int8_t new_line_indent,
		int8_t interval_between_strings) {
	fontset_character character;
	int16_t internal_x = x + new_line_indent;
	int16_t internal_y = y;
	int16_t max_height = 0;
	uint16_t len = String::strlen(str);
	for (uint16_t i = 0; i < len; i++) {
		if (str[i] == '\n') {
			internal_x = x + new_line_indent;
			if ((str[i + 1] == L',' || str[i + 1] == L';')
					&& new_line_indent < 0) {
				internal_x = internal_x - new_line_indent;
			}
			internal_y += max_height + interval_between_strings;
			max_height = 0;
		} else {
			character = get_font_character_for_char(str[i], font);
			if (character.height > max_height)
				max_height = character.height;
			if (internal_x >= width_limit - character.width
					&& break_line_if_border_reached) {
				internal_x = x + new_line_indent;
				if ((str[i + 1] == L',' || str[i + 1] == L';')
						&& new_line_indent < 0) {
					internal_x = internal_x - new_line_indent;
				}
				internal_y += max_height + interval_between_strings;
				max_height = 0;
				if (str[i] == L' ') {
					continue;
				}
			}
			if (internal_y < height_limit && internal_x < width_limit)
				draw_character(view, character, internal_x, internal_y, invert);
			else
				break;

			internal_x += character.width;
		}
	}
}

void StringView::automatic_set_dimension() {
	/*	int16_t width_before = last_width;
	 int16_t height_before = last_height;*/
	//draw_string(this, str, true);
	//if (last_width != width_before || last_height != height_before)
	Dimension d = get_string_dimension(font, str, 0, 0,
			ScreenView_get_parent()->ScreenView_get_width()
					- ScreenView_get_x(), break_line_if_border_reached,
			new_line_indent, interval_between_strings);
	ScreenView_set_dimension(d.width, d.height);
}

void StringView::draw_custom() {
	if (str == nullptr)
		return;
	automatic_set_dimension();
	draw_string(this, 0, 0,
			ScreenView_get_parent()->ScreenView_get_width()
					- ScreenView_get_x(),
			ScreenView_get_parent()->ScreenView_get_height()
					- ScreenView_get_y(), font, str, false,
			break_line_if_border_reached, new_line_indent,
			interval_between_strings);
}

void ImageView::draw_custom() {
	if (image != nullptr) {
		draw_character(this, *image, 0, 0);
	}
}

void MenuMetaElement::_apply_if_mode_display() {
	bool do_it = false;
	do_it = false;
	if (sub_elements[selected_index]->visible) {
		do_it = sub_elements[selected_index]->select_action(
				sub_elements[selected_index]);
	}
	switch (sub_elements[selected_index]->type) {
	case MENU_SCREEN:
		if (sub_elements[selected_index]->visible && do_it) {
			mode = CONTEXT_PASSED_TO_CHILD;
			sub_elements[selected_index]->visible = true;
		}
		break;
	case MENU_EDITABLE_RECORD:
		if (sub_elements[selected_index]->visible && do_it) {
			mode = EDIT;
			if (sub_elements[selected_index]->edit_mode == EDIT_BUFERIZED) {
				sub_elements[selected_index]->buffer =
						*sub_elements[selected_index]->attached_value;
			}
		}
		break;
	case MENU_EXIT:
		if (sub_elements[selected_index]->visible)
			cmd_exit();
		break;
	case MENU_ACTION:
	case MENU_STATIC_RECORD:
	case MENU_UNDEFINED:
		break;
	}
}

void MenuMetaElement::cmd_apply() {
	if (visible) {
		bool do_it = false;
		switch (mode) {
		case DISPLAY:
			_apply_if_mode_display();
			break;
		case EDIT:
			do_it = false;
			if (sub_elements[selected_index]->visible) {
				do_it = sub_elements[selected_index]->edit_apply_action(
						sub_elements[selected_index]);
			}
			if (sub_elements[selected_index]->visible && do_it) {
				if (sub_elements[selected_index]->edit_mode == EDIT_BUFERIZED) {
					*sub_elements[selected_index]->attached_value =
							sub_elements[selected_index]->buffer;
				}
				mode = DISPLAY;
			}
		case CONTEXT_PASSED_TO_CHILD:
			sub_elements[selected_index]->cmd_apply();
			break;
		}
	}
}
void MenuMetaElement::compose(uint16_t start_pos_y, ScreenView *view) {
	int16_t pos_y = start_pos_y;
	if (composer == COMPOSER_AUTOMATIC) {
		for (uint16_t i = 0; i < sub_elements.size(); i++) {
			sub_elements[i]->y = pos_y;
			int16_t temp_pos_y = pos_y
					+ get_string_dimension(sub_elements[i]->font,
							sub_elements[i]->name).height;
			if (sub_elements[i]->picture != nullptr
					&& sub_elements[i]->picture->height > temp_pos_y - pos_y)
				temp_pos_y = pos_y + sub_elements[i]->picture->height;
			pos_y = temp_pos_y + 1;
			if (mode == EDIT) {
				if (i == selected_index)
					sub_elements[i]->invert_val = !sub_elements[i]->invert_val;
			} else {
				sub_elements[i]->invert_val = false;
			}
			if (i == selected_index) {
				uint8_t awidth = get_string_dimension(sub_elements[i]->font,
						sub_elements[i]->name).width;
				if (awidth > (view->ScreenView_get_width() * 2) / 3) {
					sub_elements[i]->x = sub_elements[i]->x - 3;
					if (sub_elements[i]->x + awidth
							< view->ScreenView_get_width() / 3) {
						sub_elements[i]->x = 0;
					}
				}
			} else
				sub_elements[i]->x = 0;
		}
	}
}
void MenuMetaElement::cmd_next() {
	if (visible) {
		switch (mode) {
		case DISPLAY:
			if (++selected_index >= sub_elements.size())
				selected_index = 0;
			break;
		case EDIT:
			if (sub_elements[selected_index]->edit_mode == EDIT_BUFERIZED) {
				sub_elements[selected_index]->buffer +=
						sub_elements[selected_index]->change_val;
				if (sub_elements[selected_index]->buffer
						> sub_elements[selected_index]->max_val) {
					sub_elements[selected_index]->buffer =
							sub_elements[selected_index]->min_val;
				}
			} else {
				*sub_elements[selected_index]->attached_value +=
						sub_elements[selected_index]->change_val;
				if (*sub_elements[selected_index]->attached_value
						> sub_elements[selected_index]->max_val) {
					*sub_elements[selected_index]->attached_value =
							sub_elements[selected_index]->min_val;
				}
			}
			break;
		case CONTEXT_PASSED_TO_CHILD:
			sub_elements[selected_index]->cmd_next();
			break;
		}
	}
}
void MenuMetaElement::cmd_prev() {
	switch (mode) {
	case DISPLAY:
		if (--selected_index <= 0)
			selected_index = sub_elements.size() - 1;
		break;
	case EDIT:
		if (sub_elements[selected_index]->edit_mode == EDIT_BUFERIZED) {
			sub_elements[selected_index]->buffer -=
					sub_elements[selected_index]->change_val;
			if (sub_elements[selected_index]->buffer
					< sub_elements[selected_index]->min_val) {
				sub_elements[selected_index]->buffer =
						sub_elements[selected_index]->max_val;
			}
		} else {
			*sub_elements[selected_index]->attached_value -=
					sub_elements[selected_index]->change_val;
			if (*sub_elements[selected_index]->attached_value
					< sub_elements[selected_index]->min_val) {
				*sub_elements[selected_index]->attached_value =
						sub_elements[selected_index]->max_val;
			}
		}
		break;
	case CONTEXT_PASSED_TO_CHILD:
		sub_elements[selected_index]->cmd_prev();
		break;
	}
}
void MenuMetaElement::cmd_exit() {
	if (exit_action != nullptr && exit_action(this)) {
		switch (mode) {
		case EDIT:
		case DISPLAY:
			visible = false;
			mode = DISPLAY;
			if (parent != nullptr) {
				parent->mode = DISPLAY;
			}
			break;
		case CONTEXT_PASSED_TO_CHILD:
			sub_elements[selected_index]->cmd_exit();
			break;
		}
	}
}

void MenuView::draw_custom() {
	MenuMetaElement *menu_screen = menu;
	//Продвигаемся по дереву, пока не найдём текущий экран меню
	/*
	 * Смотрим на режим текущего меню
	 * если контекст передан, присваиваем menu_screen удерживающий контекст элемент
	 */
	do {
		if (menu_screen != nullptr && menu_screen->is_visible()
				&& menu_screen->get_mode() == CONTEXT_PASSED_TO_CHILD) {
			menu_screen =
					menu_screen->get_sub_elements()[menu_screen->get_selected_index()];
		}
	} while (menu_screen != nullptr && menu_screen->is_visible()
			&& menu_screen->get_mode() == CONTEXT_PASSED_TO_CHILD); //Находим целевой экран меню.
	if (menu_screen != nullptr && menu_screen->is_visible()) { //И, если он видимый...
		uint16_t line_pos_y = get_string_dimension(menu_screen->get_font(),
				menu_screen->get_name()).height + 1;
		for (uint16_t xx = 0; xx < ScreenView_get_width(); xx++) {//Рисуем линию
			for (uint16_t yy = 0; yy < ScreenView_get_height(); yy++) {
				ScreenView_set_pixel(xx, yy,
						yy == line_pos_y ?
								ScreenView_get_foreground_color() :
								ScreenView_get_background_color());
			}
		}
		draw_string(this, 0, 0, this->ScreenView_get_width(),
				this->ScreenView_get_height(), menu_screen->get_font(),
				menu_screen->get_name()); //Рисуем имя экрана меню
		menu_screen->compose(line_pos_y + 2, this);
		//Веселье с отрисовкой. Да. Жопа короче.
		/*
		 * Алгоритм: создаём временный строковый элемент, присваиваем ему текст, отрисовываем
		 * перемещаем его вниз, заменяем текст, снова отрисовываем. Да, нивелируется всё и вся.
		 * Второй вариант: создаём на каждый элемент по стрингвью. Пизда ресурсам. Отрисовываться
		 * будет две недели в лучшем случае. Плюс ебля с консистентностью памяти.
		 * Если выделим их в данном месте, то после выхода из подпрограммы, они со стека съебут,
		 * и дальше программу ждёт исключительно неопределённое поведение, да.
		 *
		 * Тогда просто рисуем текст, ёбамать, всё же настолько просто, ну.
		 */
		for (uint16_t i = 0; i < menu_screen->get_sub_elements().size(); i++) {
			uint16_t val_width = 0;
			MenuMetaElement *element = menu_screen->get_sub_elements()[i];
			if (element->is_visible()) {
				wchar_t val_str[20];
				if (element->get_type() == MENU_EDITABLE_RECORD) {
					switch (element->get_val_type()) {
					case TYPE_BOOL:
						String::strcpy(val_str,
								!!(*element->get_value_to_display()) ?
										str_yes : str_no);
						break;
					case TYPE_FLOAT:
						String::float_to_str(val_str,
								*element->get_value_to_display());
						break;
					case TYPE_INT:
						String::longToStr(*element->get_value_to_display(),
								val_str);
						break;
					case TYPE_UNDEF:
					default:
						String::strcpy(val_str, str_err);
						break;
					}
					val_width = get_string_dimension(element->get_font(),
							val_str).width;
					draw_string(this, ScreenView_get_width() - val_width,
							element->get_y(), this->ScreenView_get_width(),
							this->ScreenView_get_height(), element->get_font(),
							val_str, element->is_invert_val());
				} else if (element->get_type() == MENU_EXIT) {
					val_width = images_extra_back.width;
					draw_character(this, images_extra_back,
							ScreenView_get_width() - val_width,
							element->get_y());
				}
				int16_t __x = element->get_x();
				if (element->get_picture() != nullptr) {
					draw_character(this, *element->get_picture(), __x,
							element->get_y());
					__x += element->get_picture()->width + 1;
				}
				draw_string(this, __x, element->get_y(),
						this->ScreenView_get_width() - val_width - 12,
						this->ScreenView_get_height(), element->get_font(),
						element->get_name(),
						menu_screen->get_sub_elements()[menu_screen->get_selected_index()]
								== element);
			}
		}
	}
}

/*
 void UI_AbstractScreenView::set_pixel(int16_t x, int16_t y, bool val) {
 x += getX();
 y += getY();
 if (x >= getX() && y >= getY() && x < getX() + getWidth()
 && y < getY() + getHeight()) {
 if (parent != this) {
 parent->set_pixel(x, y, val);
 } else {
 lcd->set_pixel(x, y, val);
 }
 }
 }

 void UI_AbstractScreenView::addChild(UI_AbstractScreenView *child) {
 }

 void UI_AbstractScreenView::setParent(UI_AbstractScreenView *parent) {
 if (parent != this && parent != nullptr) {
 if (this->getParent() != nullptr) {
 this->getParent()->getChildren()->remove(parent);
 }
 setWidth(width == -1 ? parent->getWidth() : width);
 setHeight(height = height == -1 ? parent->getHeight() : height);
 this->parent = parent;
 parent->getChildren()->addNew(this);
 update_position();
 }
 }

 void UI_AbstractScreenView::update_position() {
 if (getParent() != nullptr) {
 switch (getPositionPolicy()) { //TODO дописать
 case AUTO_CENTER:
 setX(getParent()->getWidth() / 2 - getWidth() / 2 - 1);
 setY(getParent()->getHeight() / 2 - getHeight() / 2 - 1);
 case CONSTANT:
 default:
 break;
 }
 }
 }

 void UI_AbstractScreenView::draw() {
 custom_draw();
 for (uint8_t layer = 0; layer < 16; layer++) {
 for (uint16_t i = 0; i < children.size(); i++) {
 UI_AbstractScreenView *object = children.getElement(i);
 if (object != nullptr) {
 if (object->getVisible() && object->getLayer() == layer) {
 object->draw();
 }
 }
 }
 }
 }

 void UI_BackgroundDotBlurView::custom_draw() {
 for (int8_t ax = getX(); ax < getX() + getWidth() - 1; ax += 2) {
 for (int8_t ay = getY(); ay < getY() + getHeight() - 1; ay += 2) {
 getParent()->set_pixel(ax, ay, true);
 }
 }
 }

 void UI_BackgroundRandomBlurView::custom_draw() {
 for (int8_t ax = getX(); ax < getX() + getWidth() - 1; ax++) {
 for (int8_t ay = getY(); ay < getY() + getHeight() - 1; ay += 2) {
 getParent()->set_pixel(ax, ay, rand() % 3 == 0);
 }
 }
 }

 void UI_RectView::draw_horizontal_line(int8_t x, int8_t y, int8_t length) {
 for (int8_t ax = x; ax < x + length; ax++) {
 getParent()->set_pixel(ax, y, true);
 }
 }

 void UI_RectView::draw_vertical_line(int8_t x, int8_t y, int8_t length) {
 for (int8_t ay = y; ay < y + length; ay++) {
 getParent()->set_pixel(x, ay, true);
 }
 }

 int UI_RectView::abs(int x) {
 if (x < 0) {
 return x * -1;
 }
 return x;
 }

 void UI_RectView::draw_line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2) {
 double delta, tx, ty;
 if (abs(x2 - x1) > abs(y2 - y1)) {
 delta = ((double) (y2 - y1) / (double) (x2 - x1));
 ty = (double) y1;
 if (x1 > x2) {
 for (int i = x1; i >= x2; i--) {
 getParent()->set_pixel(i, (int) (ty + 0.5), true);
 ty = ty - delta;
 }
 } else {
 for (int i = x1; i <= x2; i++) {
 getParent()->set_pixel(i, (int) (ty + 0.5), true);
 ty = ty + delta;
 }
 }
 } else {
 delta = ((float) (x2 - x1) / (float) (y2 - y1));
 tx = (float) (x1);
 if (y1 > y2) {
 for (int i = y2 + 1; i > y1; i--) {
 getParent()->set_pixel((int) (tx + 0.5), i, true);
 tx = tx + delta;
 }
 } else {
 for (int i = y1; i < y2 + 1; i++) {
 getParent()->set_pixel((int) (tx + 0.5), i, true);
 tx = tx + delta;
 }
 }
 }
 }

 void UI_RectView::custom_draw() {
 draw_horizontal_line(getX(), getY(), getWidth());
 draw_horizontal_line(getX(), getY() + getHeight() - 1, getWidth());
 draw_vertical_line(getX(), getY(), getHeight());
 draw_vertical_line(getX() + getWidth(), getY(), getHeight());
 for (int8_t x = getX() + 1; x < getWidth() + getX(); x++)
 for (int8_t y = getY() + 1; y < getHeight() + getY() - 1; y++)
 getParent()->set_pixel(x, y, fill);
 }

 char* AbstractAttachedValue::getValue() {
 return nullptr;
 }
 */
