/*
 * ui.h
 *
 *  Created on: Apr 21, 2021
 *      Author: neovs
 */

#ifndef SRC_LIB_UI_UI_H_
#define SRC_LIB_UI_UI_H_

#include <vector>
#include <main.h>
#include <math.h>
#include "../arraylist/arraylist.h"
#include "../fontset/fontset.h"
#include "../fontset/default_fonts.h"
#include <random>

#include "../string/string.h"

enum UI_Position {
	CONSTANT,
	AUTO_CENTER,
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
};

class ScreenViewOutputInterface {
	public:
		virtual void set_pixel(uint8_t x, uint8_t y, uint8_t color) = 0;
		virtual uint16_t get_width() = 0;
		virtual uint16_t get_height() = 0;
};

const uint16_t COLOR_BLACK = 0x000;
const uint16_t COLOR_RED = 0xF00;
const uint16_t COLOR_GREEN = 0x0F0;
const uint16_t COLOR_BLUE = 0x00F;
const uint16_t COLOR_WHITE = 0xFFF;
const uint16_t COLOR_YELLOW = 0xFF0;
const uint16_t COLOR_MAGENTA = 0xF0F;
const uint16_t COLOR_CYAN = 0x0FF;
const uint16_t COLOR_NAVY = 0x008;
const uint16_t COLOR_GRAY= 0x888;
#define COLOR_GREY COLOR_GRAY;
const uint16_t COLOR_SILVER = 0xCCC;




class ScreenView {
	private:
		ScreenView *children[32] = { nullptr };
		ScreenView *parent = nullptr;
		ScreenViewOutputInterface *interface = nullptr;
		int16_t x = 0, y = 0, width = -1, height = -1;
		uint8_t layer = 0;
		uint8_t children_count = 0;
		uint8_t child_draw_offset_x = 0;
		uint8_t child_draw_offset_y = 0;
		uint8_t child_draw_offset_from_width = 0;
		uint8_t child_draw_offset_from_height = 0;
		UI_Position position = CONSTANT;
		bool visible = true;
		bool check_display_bounds = true;
		void update_position();
		uint16_t foreground_color = COLOR_BLACK;
		uint16_t background_color = COLOR_WHITE;
	public:
		void handle_action() {
			draw();
		}
		void draw();
		virtual void draw_custom() {
		}
		void View_set_position(int16_t x = 0, int16_t y = 0);
		void View_set_x(int16_t x = 0);
		void View_set_y(int16_t y = 0);
		void ScreenView_set_dimension(int16_t width = -1, int16_t height = -1);
		void ScreenView_set_width(int16_t width = -1);
		void ScreenView_set_height(int16_t height = -1);
		void ScreenView_set_layer(uint8_t layer = 0);
		void ScreenView_set_parent(ScreenView *parent);
		void ScreenView_set_children_count(uint8_t count);
		void ScreenView_set_output_interface(
				ScreenViewOutputInterface *interface);
		void ScreenView_set_position_policy(UI_Position position_policy);
		void ScreenView_set_visibility(bool visible = true);
		void ScreenView_set_display_bounds_checking(bool check) {
			check_display_bounds = check;
		}
		void ScreenView_set_foreground_color(uint16_t foreground_color) {
			this->foreground_color = foreground_color;
		}
		void ScreenView_set_background_color(uint16_t background_color) {
			this->background_color = background_color;
		}
		void ScreenView_set_child_draw_offsets(uint8_t offset_by_x,
				uint8_t offset_by_y, uint8_t offset_from_width,
				uint8_t offset_from_heigth) {
			child_draw_offset_x = offset_by_x;
			child_draw_offset_y = offset_by_y;
			child_draw_offset_from_width = offset_from_width;
			child_draw_offset_from_height = offset_from_heigth;
		}
		void ScreenView_add_child(ScreenView *child); //DO NOT USE - service only
		void ScreenView_remove_child(ScreenView *child);
		void ScreenView_set_pixel(int16_t x, int16_t y, uint16_t color,
				ScreenView *sender = nullptr);
		int16_t ScreenView_get_width();
		int16_t ScreenView_get_height();
		int16_t ScreenView_get_x();
		int16_t ScreenView_get_y();
		uint8_t ScreenView_get_layer();
		uint16_t ScreenView_get_foreground_color() {
			return foreground_color;
		}
		uint16_t ScreenView_get_background_color() {
			return background_color;
		}
		bool ScreenView_get_visibility();
		ScreenView** ScreenView_get_children();
		ScreenView* ScreenView_get_parent();
		uint8_t ScreenView_get_children_count();
		ScreenViewOutputInterface* ScreenView_get_output_interface();
		UI_Position ScreenView_get_position_policy();
		ScreenView(ScreenView *parent = nullptr, int16_t width = -1,
				int16_t height = -1, uint8_t layer = 0, int16_t x = 0,
				int16_t y = 0);
		ScreenView(ScreenViewOutputInterface *interface, int16_t width = -1,
				int16_t height = -1, uint8_t layer = 0, int16_t x = 0,
				int16_t y = 0);
		~ScreenView();
};

class BackgroundRandomBlurView: public ScreenView {
	private:
		uint8_t noise_level = 30;
	public:
		using ScreenView::ScreenView;
		void draw_custom();
		void set_noise_level(uint8_t level) {
			noise_level = level > 30 ? 30 : level;
		}
		uint8_t get_noise_level() {
			return noise_level;
		}
};

class RectView: public ScreenView {
	private:
		void draw_horizontal_line(int8_t x, int8_t y, int8_t length);
		void draw_vertical_line(int8_t x, int8_t y, int8_t length);
		int abs(int x);
		bool fill = false;
		bool offsets_setted = false;
	public:
		using ScreenView::ScreenView;
		void draw_custom();
		void setFill(bool fill) {
			this->fill = fill;
		}
};

fontset_character get_font_character_for_char(wchar_t input,
		const fontset_character font[]);
bool getBitstateOfByteStream(const uint8_t *stream, uint16_t size,
		uint16_t index);

void draw_character(ScreenView *view, fontset_character character, int8_t x,
		int8_t y, bool invert = false);

class StringView;

typedef struct {
		uint16_t width, height;
} Dimension;

Dimension get_string_dimension(const fontset_character *font,
		const wchar_t *str, const int16_t x = 0, const int16_t y = 0,
		const int16_t width_limit = 84, bool break_line_if_border_reached =
				false, int8_t new_line_indent = -1,
		int8_t interval_between_strings = 1);

void draw_string(ScreenView *view, const int16_t x, const int16_t y,
		const int16_t width_limit, const int16_t height_limit,
		const fontset_character *font, const wchar_t *str, bool invert = false,
		bool break_line_if_border_reached = false, int8_t new_line_indent = -1,
		int8_t interval_between_strings = 1);

class StringView: public ScreenView {
	private:
		wchar_t *str = nullptr;
		const fontset_character *font = std_font_localized_h7;
		void automatic_set_dimension();
		uint16_t last_width = 0;
		uint16_t last_height = 0;
		bool break_line_if_border_reached = false;
		bool string_is_allocated_in_heap = false;
		int8_t interval_between_strings = 1;
		int8_t new_line_indent = -1;
	public:
		using ScreenView::ScreenView;
		void draw_custom();
		void set_font(const fontset_character *font) {
			this->font = font;
		}
		void attach_string(wchar_t *str) {
			ScreenView_set_display_bounds_checking(false);
			if (string_is_allocated_in_heap)
				delete[] this->str;
			this->str = str;
			string_is_allocated_in_heap = false;
		}
		void set_interval_between_strings(int8_t interval) {
			interval_between_strings = interval;
		}
		void set_new_line_indent(int8_t indent) {
			new_line_indent = indent;
		}
		int8_t get_new_line_indent() {
			return new_line_indent;
		}
		int8_t get_interval_between_strings() {
			return interval_between_strings;
		}
		void set_last_width(uint16_t new_width) {
			last_width = new_width;
		}
		void set_last_height(uint16_t new_height) {
			last_height = new_height;
		}
		const fontset_character* get_font() {
			return font;
		}
		void attach_as_static_string(wchar_t *str) {
			ScreenView_set_display_bounds_checking(false);
			if (string_is_allocated_in_heap)
				delete[] this->str;
			this->str = new wchar_t[String::strlen(str) + 1];
			String::strcpy(this->str, str);
			string_is_allocated_in_heap = true;
		}
		void attach_string(const wchar_t *str) {
			ScreenView_set_display_bounds_checking(false);
			if (string_is_allocated_in_heap)
				delete[] this->str;
			this->str = new wchar_t[String::strlen(str) + 1];
			String::strcpy(this->str, str);
			string_is_allocated_in_heap = true;
		}
		void set_break_line_if_border_reached(bool value) {
			break_line_if_border_reached = value;
		}
		bool is_set_break_lint_if_border_reached() {
			return break_line_if_border_reached;
		}
		~StringView() {
			if (string_is_allocated_in_heap) {
				delete[] this->str;
			}
		}
};

class ImageView: public ScreenView {
	private:
		const fontset_character *image = nullptr;
	public:
		using ScreenView::ScreenView;
		void draw_custom();
		void set_image(const fontset_character *image) {
			this->image = image;
			ScreenView_set_dimension(image->width, image->height);
		}
};

class AnimationView: public ScreenView {

};

class ScrollBar: public ScreenView {

};

/*
 *    Меню - это рекурсивный список. Имеет следующие обязательные поля:
 * Массив - контейнер наследников:
 * Обязательный пункт "выход". Тип: действие.
 * Наименование
 * Булев тип: отображать? (По умолчанию: true). Имеет геттер и сеттер. - наследуется
 * Тип: Экран меню, редактируемая запись, статическая запись, действие.
 * Компоновщик: автоматический, статический. По умолчанию автомат.
 * Действие при выборе (функция, принимающая выбранный пункт и возвращающая boolean. Если вернула false, выбор не осуществляется). Имеет сеттер.
 * Действие при изменение значения (функция, принимающая выбранный пункт и возвращающая boolean. Если вернула false, изменение не осуществляется). Имеет сеттер.
 * Действие при выходе из данного меню (функция, принимающая this и возвращающая boolean. Если вернула false, выход не осуществляется). Имеет сеттер.
 * Текущий режим: отображается; редактируется; контекст передан подменю; Имеет геттер и сеттер.
 * Режим редактирования: буферизированный (значение присваивается только после его принятия)
 * 						 прямой (значение присваивается мгновенно)
 * Текущий выбранный элемент. По умолчанию: 0 (выход). Имеет геттер и сеттер.
 * Таймаут для ввода, при истечении которого происходит выход из меню; Имеет геттер и сеттер.
 * Отображаемое значение. Тип: Menu_AttachedValue. По умолчанию null. Имеет геттер и сеттер.

 Обязательные функции:
 * Функция-заглушка для потомков: выход, выбор, изменение значения. Пустое тело функции. Возвращает true.
 * Функция-команда "next". Вызывает переход на следующий пункт/увеличение значения.
 * Функция-команда "prev". Вызывает переход на предыдущий пункт/уменьшение значения.
 * Функция-команда "apply". Вызывает применение текущего значения/выбор текущего пункта.
 * Функция-команда "exit". Вызывает выход из меню/отмену изменения значения.
 */

enum MenuType {
	MENU_SCREEN,
	MENU_EDITABLE_RECORD,
	MENU_STATIC_RECORD,
	MENU_ACTION,
	MENU_EXIT,
	MENU_UNDEFINED
};

enum MenuComposerType {
	COMPOSER_AUTOMATIC, COMPOSER_STATIC
};

enum MenuMode {
	DISPLAY, EDIT, CONTEXT_PASSED_TO_CHILD
};

enum MenuEditMode {
	EDIT_BUFERIZED, EDIT_DIRECT
};

enum MenuValueType {
	TYPE_BOOL, TYPE_INT, TYPE_FLOAT, TYPE_UNDEF
};

class MenuMetaElement {
	private:
		ArrayList<MenuMetaElement*> sub_elements;
		static bool always_true(MenuMetaElement *el) {
			return true;
		}
		MenuMetaElement *parent = nullptr;
		const wchar_t *name;
		MenuType type = MENU_UNDEFINED;
		MenuComposerType composer = COMPOSER_AUTOMATIC;
		bool (*select_action)(MenuMetaElement *selected_object) = always_true;
		bool (*edit_apply_action)(
				MenuMetaElement *selected_object) = always_true;
		bool (*exit_action)(MenuMetaElement *selected_object) = always_true;
		MenuMode mode = DISPLAY;
		MenuEditMode edit_mode = EDIT_BUFERIZED;
		int16_t selected_index = 0; //exit;
		uint32_t menu_exit_timeout = 30000;
		double *attached_value = nullptr;
		double change_val;
		double buffer;
		double min_val;
		double max_val;
		bool visible = true;
		bool show_name = true;
		bool invert_val = false;
		const fontset_character *font = std_font_localized_h7;
		int16_t x = 0;
		int16_t y = 0;
		MenuValueType val_type = TYPE_UNDEF;
		const fontset_character *picture = nullptr;
		void _apply_if_mode_display();

	public:

		double* get_value_to_display() {
			return (parent->mode == EDIT
					&& parent->sub_elements[parent->selected_index] == this
					&& edit_mode == EDIT_BUFERIZED) ? &buffer : attached_value;
		}
		void cmd_apply();
		void compose(uint16_t start_pos_y, ScreenView *view);
		void cmd_next();
		void cmd_prev();
		void cmd_exit();
		void add_child(MenuMetaElement *child) {
			sub_elements.addNew(child);
		}
		MenuMetaElement(const wchar_t *_name, MenuType _type = MENU_SCREEN,
				MenuMetaElement *_parent = nullptr, double *_attached_value =
						nullptr, double _change_val = 1, double _min_val =
						-INFINITY, double _max_val = INFINITY) {
			name = _name;
			type = _type;
			parent = _parent;
			attached_value = _attached_value;
			change_val = _change_val;
			min_val = _min_val;
			max_val = _max_val;
			if (parent != nullptr)
				parent->add_child(this);
			if (type == MENU_SCREEN) {
				new MenuMetaElement(L"back", MENU_EXIT, this);
			}
			if (min_val == 0 && max_val == 1)
				val_type = TYPE_BOOL;
			else if (((int) (*attached_value)) == *attached_value
					&& ((int) min_val) == min_val && ((int) max_val) == max_val)
				val_type = TYPE_INT;
			else
				val_type = TYPE_FLOAT;
		}

		double* get_attached_value() const {
			return attached_value;
		}

		void set_attached_value(double *attached_value = nullptr) {
			this->attached_value = attached_value;
		}

		double get_change_val() const {
			return change_val;
		}

		void set_change_val(double change_val) {
			this->change_val = change_val;
		}

		MenuComposerType get_composer() const {
			return composer;
		}

		void set_composer(MenuComposerType composer = COMPOSER_AUTOMATIC) {
			this->composer = composer;
		}

		void set_edit_apply_action(
				bool (*edit_apply_action)(
						MenuMetaElement *selected_object) = always_true) {
			this->edit_apply_action = edit_apply_action;
		}

		MenuEditMode get_edit_mode() const {
			return edit_mode;
		}

		void set_edit_mode(MenuEditMode edit_mode = EDIT_BUFERIZED) {
			this->edit_mode = edit_mode;
		}

		void set_exit_action(
				bool (*exit_action)(
						MenuMetaElement *selected_object) = always_true) {
			this->exit_action = exit_action;
		}

		const fontset_character* get_font() const {
			return font;
		}

		void set_font(const fontset_character *font = std_font_localized_h7) {
			this->font = font;
		}

		double get_max_val() const {
			return max_val;
		}

		void set_max_val(double max_val) {
			this->max_val = max_val;
		}

		uint32_t get_menu_exit_timeout() const {
			return menu_exit_timeout;
		}

		void set_menu_exit_timeout(uint32_t menu_exit_timeout = 30000) {
			this->menu_exit_timeout = menu_exit_timeout;
		}

		double get_min_val() const {
			return min_val;
		}

		void set_min_val(double min_val) {
			this->min_val = min_val;
		}

		const wchar_t* get_name() const {
			return name;
		}

		void set_name(const wchar_t *name) {
			this->name = name;
		}

		const MenuMetaElement* get_parent() const {
			return parent;
		}

		const fontset_character* get_picture() const {
			return picture;
		}

		void set_picture(const fontset_character *picture = nullptr) {
			this->picture = picture;
		}

		void set_select_action(
				bool (*select_action)(
						MenuMetaElement *selected_object) = always_true) {
			this->select_action = select_action;
		}

		int16_t get_selected_index() const {
			return selected_index;
		}

		void set_selected_index(int16_t selected_index = 0) {
			this->selected_index = selected_index;
		}

		bool is_show_name() const {
			return show_name;
		}

		void set_show_name(bool show_name = true) {
			this->show_name = show_name;
		}

		ArrayList<MenuMetaElement*>& get_sub_elements() {
			return sub_elements;
		}

		MenuValueType get_val_type() const {
			return val_type;
		}

		void set_val_type(MenuValueType val_type = TYPE_UNDEF) {
			this->val_type = val_type;
		}

		bool is_visible() const {
			return visible;
		}

		void set_visible(bool visible = true) {
			this->visible = visible;
		}

		int16_t get_x() const {
			return x;
		}

		void set_x(int16_t x = 0) {
			this->x = x;
		}

		int16_t get_y() const {
			return y;
		}

		void set_y(int16_t y = 0) {
			this->y = y;
		}

		MenuMode get_mode() const {
			return mode;
		}

		MenuType get_type() const {
			return type;
		}

		bool is_invert_val() const {
			return invert_val;
		}
};

class MenuView: public ScreenView {
		MenuMetaElement *menu = nullptr;
		const wchar_t *str_yes = L"Да";
		const wchar_t *str_no = L"Нет";
		const wchar_t *str_err = L"ERR";
	public:
		using ScreenView::ScreenView;
		void set_menu(MenuMetaElement *menu) {
			this->menu = menu;
		}
		void draw_custom();
};

/*
 class UI_AbstractScreenView {
 private:
 ArrayList<UI_AbstractScreenView*> children;
 UI_AbstractScreenView *parent = nullptr;
 LCD *lcd = nullptr;
 int16_t x, y, width, height;
 uint8_t layer;
 UI_Position position = CONSTANT;
 bool visible = true;
 public:
 void setX(int16_t x) {
 this->x = x;
 }
 void setY(int16_t y) {
 this->y = y;
 }
 void setParent(UI_AbstractScreenView *parent);
 void setWidth(int16_t width) {
 this->width = width;
 update_position();
 }
 void setHeight(int16_t height) {
 this->height = height;
 update_position();
 }
 void setLayer(uint8_t layer) {
 this->layer = layer;
 }
 void setVisible(bool visible) {
 this->visible = visible;
 }
 void setXY(int16_t x, int16_t y) {
 this->x = x;
 this->y = y;
 }
 void setPositionPolicy(UI_Position position) {
 this->position = position;
 update_position();
 }
 UI_Position getPositionPolicy() {
 return position;
 }
 void update_position();
 ArrayList<UI_AbstractScreenView*>* getChildren() {
 return &children;
 }
 void setWidthHeight(int16_t width, int16_t height) {
 this->width = width;
 this->height = height;
 update_position();
 }
 bool getVisible() {
 return visible;
 }
 void set_pixel(int16_t x, int16_t y, bool val);
 int16_t getX() {
 return x;
 }
 int16_t getY() {
 return y;
 }
 int16_t getWidth() {
 return width;
 }
 int16_t getHeight() {
 return height;
 }
 uint8_t getLayer() {
 return layer;
 }
 UI_AbstractScreenView* getParent() {
 return parent;
 }
 ;
 LCD* getLCD() {
 return lcd;
 }
 void setLCD(LCD *lcd) {
 this->lcd = lcd;
 }
 void draw();
 virtual void custom_draw() = 0;
 UI_AbstractScreenView(LCD *lcd = nullptr, int16_t x = 0, int16_t y = 0,
 uint8_t layer = 0, UI_Position position_policy = CONSTANT,
 int16_t width = -1, int16_t height = -1) {
 setLCD(lcd);
 setXY(x, y);
 setLayer(layer);
 setWidth(width == -1 ? lcd->get_width() : width);
 setHeight(height == -1 ? lcd->get_height() : height);
 position = position_policy;
 parent = this;
 }
 UI_AbstractScreenView(UI_AbstractScreenView *parent, int16_t x = 0,
 int16_t y = 0, uint8_t layer = 0, UI_Position position_policy =
 CONSTANT, int16_t width = -1, int16_t height = -1) {
 setX(x);
 setY(y);
 setLayer(layer);
 setWidth(width);
 setHeight(height);
 position = position_policy;
 setParent(parent);
 }
 void addChild(UI_AbstractScreenView *child);
 ~UI_AbstractScreenView() {
 children.~ArrayList();
 }
 };

 class UI_ScreenView: public UI_AbstractScreenView {
 public:
 using UI_AbstractScreenView::UI_AbstractScreenView;
 void custom_draw() {
 }
 };

 class UI_BackgroundDotBlurView: public UI_AbstractScreenView {
 public:
 using UI_AbstractScreenView::UI_AbstractScreenView;
 void custom_draw();
 };

 class UI_BackgroundRandomBlurView: public UI_AbstractScreenView {
 public:
 using UI_AbstractScreenView::UI_AbstractScreenView;
 void custom_draw();
 };

 class UI_RectView: public UI_AbstractScreenView {
 private:
 void draw_line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
 void draw_horizontal_line(int8_t x, int8_t y, int8_t length);
 void draw_vertical_line(int8_t x, int8_t y, int8_t length);
 int abs(int x);
 bool fill = false;
 public:
 using UI_AbstractScreenView::UI_AbstractScreenView;
 void custom_draw();
 void setFill(bool fill) {
 this->fill = fill;
 }
 };

 enum StringValueType {
 U8, U16, U32, U64, S8, S16, S32, S64, FLOAT, DOUBLE, STRING, UNKNOWN
 };

 class AbstractAttachedValue {
 uint8_t *u8_value = nullptr;
 uint16_t *u16_value = nullptr;
 uint32_t *u32_value = nullptr;
 uint64_t *u64_value = nullptr;
 int8_t *s8_value = nullptr;
 int16_t *s16_value = nullptr;
 int32_t *s32_value = nullptr;
 int64_t *s64_value = nullptr;
 float *float_value = nullptr;
 double *double_value = nullptr;
 char *str_value = nullptr;
 AttachedValueType type = UNKNOWN;
 public:
 AbstractAttachedValue(uint8_t *u8_value = nullptr) {
 this->u8_value = u8_value;
 type = U8;
 }
 AbstractAttachedValue(uint16_t *u16_value = nullptr) {
 this->u16_value = u16_value;
 type = U16;
 }
 AbstractAttachedValue(uint32_t *u32_value = nullptr) {
 this->u32_value = u32_value;
 type = U32;
 }
 AbstractAttachedValue(uint64_t *u64_value = nullptr) {
 this->u64_value = u64_value;
 type = U64;
 }
 AbstractAttachedValue(int8_t *s8_value = nullptr) {
 this->s8_value = s8_value;
 type = S8;
 }
 AbstractAttachedValue(int16_t *s16_value = nullptr) {
 this->s16_value = s16_value;
 type = S16;
 }
 AbstractAttachedValue(int32_t *s32_value = nullptr) {
 this->s32_value = s32_value;
 type = S32;
 }
 AbstractAttachedValue(int64_t *s64_value = nullptr) {
 this->s64_value = s64_value;
 type = S64;
 }
 AbstractAttachedValue(float *float_value = nullptr) {
 this->float_value = float_value;
 type = FLOAT;
 }
 AbstractAttachedValue(double *double_value = nullptr) {
 this->double_value = double_value;
 type = DOUBLE;
 }
 AbstractAttachedValue(char *str = nullptr) {
 str_value = str;
 type = STRING;
 }
 char* getValue();

 };
 */

#endif /* SRC_LIB_UI_UI_H_ */
