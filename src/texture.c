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
#include <graph.h>
#include <gs_psm.h>
#include <tamtypes.h>
#include <packet.h>
#include <stdio.h>

/* Maximum texture size. */
#define MAX_SIZE 512

packet_t *texturePacket;
texbuffer_t texbuf;

int vRamTexture = 0;

/**
 * Initializes the texture video memory and packet.
 */
void initTexture(void)
{
	vRamTexture = graph_vram_allocate(MAX_SIZE, MAX_SIZE, GS_PSM_32,
		GRAPH_ALIGN_BLOCK);

	texturePacket = packet_init(50, PACKET_NORMAL);
}

/**
 * Uninitialize texture and packet.
 */
void exitTexture(void)
{
	graph_vram_free(vRamTexture);
	packet_free(texturePacket);
}

/**
 * Copies an given texture to GS video memory.
 * @p w Texture width.
 * @p h Texture height.
 */
void loadTexture(void *texture, int w, int h)
{
	qword_t *q;

	if (!vRamTexture)
	{
		printf("Trying to load a texture without allocated memory!\n");
		return;
	}

	q = texturePacket->data;

	/* Copies the texture from EE to the GS memory. */
	q = draw_texture_transfer(q, texture, w, h, GS_PSM_32, vRamTexture, w);
	q = draw_texture_flush(q);

	/* Send the packet. */
	dma_channel_send_chain(DMA_CHANNEL_GIF, texturePacket->data,
		q - texturePacket->data, 0, 0);
	
	dma_wait_fast();
}

/**
 * Setup the texture buffer and instructs
 * where is the texture in memory.
 * @p w Texture width.
 * @p h Texture height.
 */
void setupTexture(u16 w, u16 h)
{
	clutbuffer_t clut;
	qword_t *q;

	q = texturePacket->data;

	/* Setup the texture buffer. */
	texbuf.address = vRamTexture;
	texbuf.width   = w;
	texbuf.psm     = 0;

	texbuf.info.width  = draw_log2(w);
	texbuf.info.height = draw_log2(h);
	texbuf.info.components = TEXTURE_COMPONENTS_RGBA;
	texbuf.info.function = TEXTURE_FUNCTION_DECAL;

	/* Clut buffer. */
	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = 0;
	clut.load_method = CLUT_NO_LOAD;
	clut.address = 0;

	/* Send the packet. */
	q = draw_texturebuffer(q, 0, &texbuf, &clut);
	dma_channel_send_normal(DMA_CHANNEL_GIF, texturePacket->data,
		q - texturePacket->data, 0, 0);
	
	dma_wait_fast();
}
