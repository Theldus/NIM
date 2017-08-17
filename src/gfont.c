/*
 * Copyright(C) 2016 Davidson Francis <davidsondfgl@gmail.com>
 *
 * This file is part of NIM - PS2 Edition.
 *
 * NIM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * NIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NIM. If not, see <http://www.gnu.org/licenses/>.
 */

#include <tamtypes.h>
#include <stdio.h>
#include "include/render.h"
#include "include/gfont.h"

static u8 font_size    =  0;
static s8 font_spacing = -5;

/**
 * Puts a char in the screen given coordinates
 * @p x X coordinate
 * @p y Y coordinate
 * @p c Character
 */
void gputc(u16 x, u16 y, u16 c)
{
	u16 *tc; /* Texture coordinates.     */
	u16 x0;  /* Top most left X axis.    */
	u16 y0;  /* Top most right Y axis.   */

	tc = &fixed_tc[c*4];
	x0 = *tc++;
	y0 = *tc++;

	g2d_fill_rect_textured_uv(x, y, x + FONT_WIDTH + font_size,
		y + FONT_HEIGHT + font_size, x0, y0, FONT_WIDTH, FONT_HEIGHT);
}

/**
 * Puts a string on the screen given coordinates.
 * @p x X coordinate
 * @p y Y coordinate
 * @p str string
 */
void gputs(u16 x, u16 y, const char *str)
{
	int c;
	
	/* Write string. */
	while ((c = *str++) != '\0')
	{
		if (c == '\n')
		{
			x = 0;
			y += FONT_HEIGHT + font_size;
			continue;
		}

		gputc(x, y, c);
		x += FONT_WIDTH + font_size + font_spacing;
		
		if ( (x + FONT_WIDTH + font_size + font_spacing) > screen_width)
		{
			x = 0;
			y += FONT_HEIGHT + font_size;
		}
	}
}

/**
 * Puts a formatted string on the screen given coordinates.
 * @p x X coordinate
 * @p y Y coordinate
 * @p format string format
 */
void gprintf(u16 x, u16 y, const char *format, ...)
{
	int n;            /* Characters written. */
	va_list args;     /* Arguments.          */
	char buffer[128]; /* Buffer.             */

	va_start(args, format);
	n = vsprintf(buffer, format, args);
	va_end(args);

	if (n > 0)
		gputs(x, y, buffer);	
}

/* -----------------------------------------------------*
 * Font adjustments.                                    *
 * -----------------------------------------------------*/

/**
 * Resize the screen font size. (in pixels)
 * @p size Size to be added.
 */
void set_font_size(u8 size)
{
	font_size = size;
}

/**
 * Gets the current screen font size. (in pixels).
 * @returns Screen font size.
 */
u8 get_font_size()
{
	return font_size;
}

/**
 * Sets the spacing between chars in the current
 * font.
 *
 * @p size Spacing, in pixels.
 *
 * @note You can add negative values here, to aproximate
 * the chars or positive values to distance them.
 */
void set_font_spacing(u8 size)
{
	font_size += size;
}

/**
 * Gets the current spacing between the chars.
 * @returns Spacing, in pixels.
 */
u8 get_font_spacing()
{
	return font_size;
}
