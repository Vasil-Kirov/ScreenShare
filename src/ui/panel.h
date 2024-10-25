#pragma once
#include "widget.h"

typedef struct Panel Panel;


struct Panel {
	Widget base;
	int gap;
};

Panel *create_panel(Widget *parent, b32 is_horizontal);

