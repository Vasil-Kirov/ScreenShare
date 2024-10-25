#include "layout.h"


Layout *create_layout(v2 top_left, v2 size, Allocator *alloc)
{
	Layout *res = alloc_type(Layout, alloc);
	res->alloc = alloc;
	res->start = v2_add(top_left, make_v2(PADDING, PADDING));
	res->at = res->start;
	res->end = v2_add(res->start, size);

	return res;
}

v2 layout_push_rect(Layout *l, v2 rect_size)
{
	v2 res = l->at;
	l->at.y += rect_size.y + PADDING;

	return res;
}

