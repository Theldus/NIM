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

#include <dma.h>
#include <draw.h>
#include <draw_primitives.h>
#include <gif_tags.h>
#include <graph.h>
#include <gs_psm.h>
#include <kernel.h>
#include <tamtypes.h>
#include <packet.h>
#include <stdio.h>

#include "include/texture.h"

/* The minimum buffers needed for single buffered rendering. */
framebuffer_t frame;
zbuffer_t z;

/* The data packet. */
packet_t *packet;

/* Colors. */
color_t color;
color_t fill_color;

/* QuadWord. */
qword_t *qWord;

/* Screen size. */
u16 screen_width;
u16 screen_height;

/*============================================================================*
 *                     Initializes GS Environment                             *
 *============================================================================*/

/**
 * Allocates the framembuffer and initializes the graphic.
 */
static void init_gs()
{
	/* Define a 32-bit 640x448 framebuffer. (NTSC Standard) */
	screen_width  = 640;
	screen_height = 448;

	frame.width  = screen_width;
	frame.height = screen_height;
	frame.mask = 0;
	frame.psm = GS_PSM_32;

	/* Allocate some vram for our framebuffer */
	frame.address = graph_vram_allocate(frame.width, frame.height, frame.psm,
		GRAPH_ALIGN_PAGE);

	/* Disable the zbuffer. */
	z.enable = 0;
	z.address = 0;
	z.mask = 0;
	z.zsm = 0;

	/* Initialize the screen and tie the framebuffer to the read circuits. */
	graph_initialize(frame.address, frame.width, frame.height,
		frame.psm, 0, 0);
}

/**
 * Initializes the drawing environment, quite obvious no?
 * After that, everything is setup to start drawing some
 * cool things ;-).
 */
static void init_drawing_environment()
{
	qword_t *q;

	/* This is our generic qword pointer. */
	q = packet->data;

	/* This will setup a default drawing environment. */
	q = draw_setup_environment(q, 0, &frame, &z);
	q = draw_finish(q);

	/* Now send the packet, no need to wait since it's the first. */
	dma_channel_send_normal(DMA_CHANNEL_GIF, packet->data,
		q - packet->data, 0, 0);

	/* Wait until the finish event occurs. */
	draw_wait_finish();

	/* Wait until dmac is done. */
	dma_wait_fast();
	q = packet->data;

	/* Clear screen. */
	q = draw_clear(q, 0, 0, 0, frame.width, frame.height, 0, 0, 0);
	q = draw_finish(q);

	dma_channel_send_normal(DMA_CHANNEL_GIF, packet->data,
		q - packet->data, 0, 0);

	/* Wait until the screen is cleared. */
	draw_wait_finish();

	/* Update the screen. */
	graph_wait_vsync();

	/* Initializes my global QuadWord. */
	qWord = packet->data;
}

/**
 * Initializes everything, this and render_exit()
 * are the only external functions to initialize all
 * the graphical environment and *SHOULD* be called
 * before to do anything in the code.
 */
void render_setup()
{
	packet = packet_init(1024, PACKET_NORMAL);

	/* Init GIF dma channel. */
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	/* Init the GS, framebuffer, and zbuffer. */
	init_gs();

	/* Init the drawing environment and framebuffer. */
	init_drawing_environment();

	/* Init the texture stuff. */
	initTexture();
}

/**
 * Frees everything, this function together with
 * render_setup() makes a combo power to start
 * and finish the drawing process.
 *
 * This funtion *should* or *not* be called well
 * at the END of the code, since your code may
 * or not executing in a infinite loop.
 *
 * Should be noticed that if you test on a real
 * console, the screen will be blank after executing
 * this because we frees up the vram and shutdown the
 * graph sintetizer, alright?
 */
void render_exit()
{
	/* Free the vram. */
	graph_vram_free(frame.address);

	/* Free the packet. */
	packet_free(packet);

	/* Disable output and reset the GS. */
	graph_shutdown();

	/* Shutdown our currently used dma channel. */
	dma_channel_shutdown(DMA_CHANNEL_GIF, 0);

	/* Uninitialize texture. */
	exitTexture();

	/* Sleep. */
	SleepThread();
}

/*============================================================================*
 *                               Colors ;-)                                   *
 *============================================================================*/

/**
 * Sets the current color.
 * @p r Red
 * @p g Green
 * @p b Blue
 */
void g2d_set_color(u8 r, u8 g, u8 b)
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = 0xFF;
	color.q = 0;
}

/**
 * Gets the current color.
 * @return Current color.
 */
color_t g2d_get_color(void)
{
	return color;
}

/**
 * Sets the current filling color.
 * @p r Red
 * @p g Green
 * @p b Blue
 */
void g2d_set_fill_color(u8 r, u8 g, u8 b)
{
	fill_color.r = r;
	fill_color.g = g;
	fill_color.b = b;
	fill_color.a = 0xFF;
	fill_color.q = 0;
}

/**
 * Gets the current filling color.
 * @return Current color.
 */
color_t g2d_get_fill_color(void)
{
	return fill_color;
}

/*============================================================================*
 *                            Basic Primitives                                *
 *============================================================================*/

/**
 * Draws a filled rectangle on the screen.
 * The coordinates are in pixels ;-).
 *
 * @p x0 - Top most left  X coordinate.
 * @p y0 - Top most right Y coordinate.
 * @p x1 - Bottom most left  X coordinate.
 * @p y1 - Bottom most right Y coordinate.
 */
void g2d_fill_rect(u16 x0, u16 y0, u16 x1, u16 y1)
{
	rect_t rc;
	rc.v0.x = x0;
	rc.v0.y = y0;
	rc.v0.z = 0;

	rc.v1.x = x1;
	rc.v1.y = y1;
	rc.v1.z = 0;

	rc.color.r = fill_color.r;
	rc.color.g = fill_color.g;
	rc.color.b = fill_color.b;
	rc.color.a = fill_color.a;
	rc.color.q = 1.0f;
	qWord = draw_rect_filled(qWord, 0, &rc);
}

/**
 * Draws the current texture on given coordinates.
 * The coordinates are in pixels ;-).
 *
 * @p x - Top most left  X coordinate.
 * @p y - Top most right Y coordinate.
 * @p w - Texture width.
 * @p h - Texture height.
 */
void g2d_fill_rect_textured(u16 x, u16 y, u16 w, u16 h)
{
	texrect_t rc;

	rc.v0.x = x;
	rc.v0.y = y;
	rc.v0.z = 0;

	rc.t0.u = 0;
	rc.t0.v = 0;
	
	rc.v1.x = x + w;
	rc.v1.y = y + h;
	rc.v1.z = 0;
	
	rc.t1.u = w;
	rc.t1.v = h;
	
	rc.color.r = fill_color.r;
	rc.color.g = fill_color.g;
	rc.color.b = fill_color.b;
	rc.color.a = fill_color.a;
	rc.color.q = 1.0f;
	qWord = draw_rect_textured(qWord, 0, &rc);
}

/**
 * Draws a specified piece of the current
 * texture on given coordinates.
 * The coordinates are in pixels ;-).
 *
 * @p x - Top most left  X coordinate.
 * @p y - Top most right Y coordinate.
 * @p u - X displacement of the current texture.
 * @p v - Y displacement of the current texture.
 * @p w - Piece width.
 * @p h - Piece height.
 */
void g2d_fill_rect_textured_uv(u16 x0, u16 y0, u16 x1, u16 y1,
	u16 u, u16 v, u16 w, u16 h)
{
	texrect_t rc;

	rc.v0.x = x0;
	rc.v0.y = y0;
	rc.v0.z = 0;

	rc.t0.u = u;
	rc.t0.v = v;
	
	rc.v1.x = x1;
	rc.v1.y = y1;
	rc.v1.z = 0;
	
	rc.t1.u = u + w;
	rc.t1.v = v + h;
	
	rc.color.r = fill_color.r;
	rc.color.g = fill_color.g;
	rc.color.b = fill_color.b;
	rc.color.a = fill_color.a;
	rc.color.q = 1.0f;
	qWord = draw_rect_textured(qWord, 0, &rc);
}

/**
 * Clear the specified area given coordinates.
 * The coordinates are in pixels ;-).
 *
 * @p x - Top most left  X coordinate.
 * @p y - Top most right Y coordinate.
 * @p w - Area width.
 * @p h - Area height.
 */
void g2d_clrscr(u16 x, u16 y, u16 w, u16 h)
{
	qWord = draw_clear(qWord, 0, x, y, w, h, 0, 0, 0);
}

/**
 * Establishes the frame start and adjust some things
 * to the next drawing.
 *
 * It's expected that your main game loop shoud be looks
 * like this:
 *
 * for(;;)
 * {
 *    beginFrame();
 *
 *    updateLogic(); <-- you shoud be write something 
 *                       like this
 *
 *    endFrame();
 * }
 */
void beginFrame(void)
{
	qWord = packet->data;
}

/**
 * Fetches the data written until now in the QuadWord packet
 * and sends to the DMA. This function marks the end of the
 * frame and should be the last thing to be called into your 
 * main loop.
 */
void endFrame(void)
{
	/* Finishes the current drawing. */
	qWord = draw_finish(qWord);

	/* Waits for some previous transfer. */
	dma_wait_fast();

	/* Sends throughout DMA. */
	dma_channel_send_normal(DMA_CHANNEL_GIF, packet->data,
		qWord - packet->data, 0, 0);

	/* Waits for VSync. */
	draw_wait_finish();
	graph_wait_vsync();
}
