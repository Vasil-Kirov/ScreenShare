#pragma once
#include "widget.h"
#include <vmem.h>

#define MAX_WIDGETS 10240

typedef struct {
	Widget **widgets;
	i32 widget_count;
	Allocator alloc;
} UI;

UI *init_ui();
void push_widget(UI *ui, Widget *widget);
