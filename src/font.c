#include "font.h"
#include "log.h"
#include "renderer.h"

int glyph_size = 16;

Font load_font()
{
	Font result = {};

	FT_Error error = FT_Init_FreeType(&result.lib);
	if (error)
	{
		lfatal("Failed to init free type");
	}
	error = FT_New_Face(result.lib, "C:/Windows/Fonts/cour.ttf", 0, &result.face);
	if (error)
	{
		lfatal("Failed to load arial.ttf");
	}
	error = FT_Set_Char_Size(result.face, 0, glyph_size*64, 300, 300);
	if (error)
	{
		lfatal("Failed to set char size for font");
	}

	result.cached = malloc(256 * sizeof(Glyph));
	memset(result.cached, 0, 256 * sizeof(Glyph));

	return result;
}

Glyph get_glyph(Renderer *r, Font f, unsigned char glyph)
{
	if(f.cached[glyph].texture)
		return f.cached[glyph];

	FT_UInt idx = FT_Get_Char_Index(f.face, glyph);

	FT_Error error = FT_Load_Glyph(f.face, idx, FT_LOAD_RENDER);
	if(error)
	{
		lerror("Failed to load gylph: '%c'", glyph);
		return (Glyph){};
	}
	error = FT_Render_Glyph(f.face->glyph, FT_RENDER_MODE_NORMAL);
	if(error)
	{
		lerror("Failed to render gylph: '%c'", glyph);
		return (Glyph){};
	}
	FT_GlyphSlot slot = f.face->glyph;

	u32 *pixels = malloc(slot->bitmap.pitch * slot->bitmap.rows * 4);
	memset(pixels, 0, 4 * slot->bitmap.pitch * slot->bitmap.rows);
	u32 *scan = pixels;
	for(int y = 0; y < slot->bitmap.rows; ++y)
	{
		for(int x = 0; x < slot->bitmap.pitch; ++x)
		{
			u8 p = slot->bitmap.buffer[x + (y * slot->bitmap.pitch)];
			*scan =
				((u32)p) << 24 |
				((u32)p) << 16 |
				((u32)p) << 8  |
				p;
			scan++;
		}
	}

	SDL_Surface *surface = SDL_CreateSurfaceFrom(slot->bitmap.width, slot->bitmap.rows, SDL_PIXELFORMAT_RGBA32,
			pixels, slot->bitmap.pitch*4);

	if(surface == NULL)
	{
		lerror("Failed to create glyph surface: %s", SDL_GetError());
		return (Glyph){};
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(r->renderer, surface);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);
	SDL_DestroySurface(surface);

	Glyph res = {};
	res.texture = texture;
	res.advance = slot->advance;
	res.bearing = make_iv2(slot->bitmap_left, slot->bitmap_top);
	res.size = make_iv2(slot->bitmap.width, slot->bitmap.rows);

	f.cached[glyph] = res;
	return res;
}


