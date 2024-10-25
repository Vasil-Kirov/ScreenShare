#include "panel.h"
#include "../log.h"

int panel_layout(Panel *p, Renderer *r)
{
	float hspace = p->base.bounds.z - p->base.bounds.x;
	float vspace = p->base.bounds.w - p->base.bounds.y;
	vspace -= p->gap * 2;
	hspace -= p->gap * 2;

	b32 is_horizontal = (p->base.flags & WF_HORIZONTAL_PANEL) != 0;

	if(p->base.child_count)
	{
		float space_per_child = (1.0f/p->base.child_count) * (is_horizontal ? hspace : vspace);
		float pos = p->gap;

		Widget *at = p->base.first_child;
		while(at)
		{
			float center = lerp(pos, pos+space_per_child, 0.5f);
			int width  = 0;
			int height = 0;

			v4 box = {};
			if(is_horizontal)
			{
				if(at->flags & WF_HFILL)
				{
					lfatal("Filling on panel layout direction is not supported");
				}
				else
				{
					width = at->message(r, at, MSG_WIDTH, NULL);
					if(width <= 0)
						width = space_per_child;
				}

				if(at->flags & WF_VFILL)
				{
					height = vspace;
				}
				else
				{
					height = at->message(r, at, MSG_HEIGHT, NULL);
				}

				center -= width / 2.0f;
				float y_center = lerp(0, vspace, 0.5f);
				box.x = center;
				box.z = center + width;
				box.y = y_center - (height / 2.0f);
				box.w = y_center + (height / 2.0f);
			}
			else
			{
				if(at->flags & WF_HFILL)
				{
					width = hspace;
				}
				else
				{
					width = at->message(r, at, MSG_WIDTH, NULL);
				}

				if(at->flags & WF_VFILL)
				{
					lfatal("Filling on panel layout direction is not supported");
				}
				else
				{
					height = at->message(r, at, MSG_HEIGHT, NULL);
					if(height <= 0)
						height = space_per_child;
				}

				center -= height / 2.0f;
				float x_center = lerp(0, hspace, 0.5f);
				box.x = x_center - (width / 2.0f);
				box.z = x_center + (width / 2.0f);
				box.y = center;
				box.w = center + height;
			}


			at->message(r, at, MSG_LAYOUT, &box);
			pos += space_per_child;
			at = at->next;
		}
	}

	return is_horizontal ? hspace : vspace;
}

int panel_message(Renderer *r, Widget *this, WidgetMessage message, void *data)
{
	int result = 0;
	Panel *p = (Panel *)this;
	switch(message)
	{
		case MSG_LAYOUT:
		{
			p->base.bounds = *(v4 *)data;
			panel_layout(p, r);
		} break;
		case MSG_DRAW:
		{
			{
				v2 at  = v2_add(make_v2(p->base.bounds.x, p->base.bounds.y), *(v2 *)data);
				v2 end = v2_add(make_v2(p->base.bounds.z, p->base.bounds.w), *(v2 *)data);
				v4 box = make_v4(at.x, at.y, end.x, end.y);
				draw_empty_rect(r, box);
			}
			Widget *at = this->first_child;

			v2 parent_offset = make_v2(this->bounds.x, this->bounds.y);
			parent_offset = v2_add(parent_offset, *(v2 *)data);
			while(at)
			{
				at->message(r, at, message, &parent_offset);
				at = at->next;
			}
		} break;
		case MSG_WIDTH:
		{
			float hspace = this->bounds.z - this->bounds.x;
			result = hspace;
		} break;
		case MSG_HEIGHT:
		{
			float vspace = this->bounds.w - this->bounds.y;
			result = vspace;
		} break;
		default: default_message_handler(r, this, message, data);
	}
	return result;
}

Panel *create_panel(Widget *parent, b32 is_horizontal)
{
	u32 flags = 0;
	if(is_horizontal)
		flags = WF_HORIZONTAL_PANEL | WF_HFILL;
	else
		flags = WF_VERTICAL_PANEL   | WF_VFILL;
	Panel *panel = create_widget(Panel, parent->alloc, parent, flags);
	panel->base.message = panel_message;
	panel->gap = 0;

	return panel;
}

