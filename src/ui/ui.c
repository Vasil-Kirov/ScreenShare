#include "ui.h"

UI *init_ui()
{
	Allocator alloc = arena_create(MB(1))->alloc;
	Allocator *alloc_ptr = &alloc;
	UI *res = alloc_type(UI, alloc_ptr);
	res->alloc = alloc;
	res->widgets = res->alloc.alloc(sizeof(Widget *) * MAX_WIDGETS, &res->alloc);

	return res;
}

void push_widget(UI *ui, Widget *widget)
{
	ui->widgets[ui->widget_count++] = widget;
}


