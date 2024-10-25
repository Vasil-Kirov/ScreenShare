#pragma once
#include "../math.h"
#include "config.h"
#include "vmem.h"

typedef struct Layout Layout;

struct Layout {
	Allocator *alloc;
	v2 start;
	v2 at;
	v2 end;
};

Layout *create_layout(v2 top_left, v2 size, Allocator *alloc);
v2 layout_push_rect(Layout *l, v2 rect_size);

