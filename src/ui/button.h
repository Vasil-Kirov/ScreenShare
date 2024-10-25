#pragma once
#include "vstring.h"
#include "widget.h"

typedef struct Button Button;

struct Button {
	Widget base;
	v4 color;
	String text;
	void (*on_click)(void*);
	void *pass_data;
	bool is_highlighted;
};

Button *create_button(Widget *parent, String text);
void button_set_on_click(Button *b, void (*on_click)(void*), void *pass_data);

