#pragma once
#include "../math.h"
#include "../renderer.h"
#include <vmem.h>

typedef struct Widget Widget;
b32 needs_redraw = false;

typedef enum {
	MSG_DRAW,
	MSG_LAYOUT,
	MSG_WIDTH,
	MSG_HEIGHT,
	MSG_MOUSEMOVE,
	MSG_MOUSEBUTTON,
	MSG_DYNAMIC_UPDATE,
} WidgetMessage;

typedef enum {
	WF_WINDOW           = BIT(0),
	WF_BUTTON           = BIT(1),
	WF_HORIZONTAL_PANEL = BIT(2),
	WF_VERTICAL_PANEL   = BIT(3),
	WF_HFILL            = BIT(4),
	WF_VFILL            = BIT(5),
} WidgetFlag;

struct Widget {
	int (*message)(Renderer *command_buffer, Widget *this, WidgetMessage message, void *opt_data);
	Widget *parent;
	Widget *next;
	Widget *first_child;
	Widget *last_child;
	uint child_count;
	v4 bounds;
	Allocator *alloc;
	u32 flags;
};

typedef struct {
	int button; // 0, 1, 2
	int clicks; // single, double, etc...
	b32 pressed; // or released
} MouseButtonEvent;

Widget *_create_widget(Allocator *alloc, size_t size, Widget *opt_parent, u32 flags);
int default_message_handler(Renderer *buffer, Widget *this, WidgetMessage message, void *data);
v4 offset_box(v4 box, v2 offset);

#define create_widget(t, alloc, opt_parent, flags) (t *)_create_widget(alloc, sizeof(t), opt_parent, flags)

