#pragma once
#include "widget.h"

typedef struct Window Window;

struct Window {
	Widget base;
	v4 clip;
};

Window *create_window(Allocator *alloc, v2 size);

