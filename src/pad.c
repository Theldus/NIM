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

/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id$
# Pad demo app
# Quick and dirty, little or no error checks etc..
# Distributed as is
*/

/**
 * This is the sample rpc/pad.c provided in PS2SDK slightly modified
 * to my needs, so all the credits goes to PS2DEV.
 *
 * Until now I can't find any doc talking about in how to use properly
 * joypads in PS2SDK so there are some things here I can't understand.
 *
 * I don't want to have full support and detect the most of existing
 * controllers, I assume the user will always use Dual Shock and for
 * my purposes I don't care about L1,L2... and Analog Buttons.
 *
 * This will make the things easier to me, ;-).
 */

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>

#include <libpad.h>

/*
 * Global var's
 */

/**
 * pad_dma_buf is provided by the user, one buf for each pad
 * contains the pad's current state.
 */
static char padBuf[256] __attribute__((aligned(64)));

/*
 * Local functions
 */

/**
 * loadModules, loads the internal modules
 * present on PS2 rom.
 */
static void loadModules(void)
{
	int ret;

	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	if (ret < 0)
	{
		printf("sifLoadModule sio failed: %d\n", ret);
		SleepThread();
	}

	ret = SifLoadModule("rom0:PADMAN", 0, NULL);

	if (ret < 0)
	{
		printf("sifLoadModule pad failed: %d\n", ret);
		SleepThread();
	}
}

/**
 * waitPadReady, waits for the joypad it's ready
 * @p port pad port
 * @p slot pad slot
 */
static void waitPadReady(int port, int slot)
{
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;

	while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1))
	{
		if (state != lastState)
		{
			padStateInt2String(state, stateString);
			printf("Please wait, pad(%d,%d) is in state %s\n",
				port, slot, stateString);
		}

		lastState = state;
		state = padGetState(port, slot);
	}

    /* Were the pad ever 'out of sync'? */
	if (lastState != -1)
		printf("Pad OK!\n");
}


/**
 * Initializes the joypad, basically we check
 * for the supported modes and configure the
 * actuators.
 * @p port pad port
 * @p slot pad slot
 */
static int initializePad(int port, int slot)
{
	int modes;
	int i;

	waitPadReady(port, slot);

	/*
	 * How many different modes can this device operate in?
	 * i.e. get # entrys in the modetable.
	 */
	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
	printf("The device has %d modes\n", modes);

	/*
	 * If modes == 0, this is not a Dual shock controller
	 * (it has no actuator engines).
	 */
	if (modes == 0)
	{
		printf("This is a digital controller?\n");
		return (1);
	}

	/* Verify that the controller has a DUAL SHOCK mode. */
	i = 0;
	do
	{
		if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
			break;
		
		i++;
	} while (i < modes);
	
	if (i >= modes)
	{
		printf("This is no Dual Shock controller\n");
		return (1);
	}

	printf("Enabling dual shock functions\n");

	/* When using MMODE_LOCK, user cant change mode with Select button. */
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);
	waitPadReady(port, slot);

	return (1);
}

void setupPad(int port)
{
	int ret;

	SifInitRpc(0);
	loadModules();
	padInit(0);

	if ((ret = padPortOpen(port, 0, padBuf)) == 0)
	{
    	printf("padOpenPort failed: %d\n", ret);
    	SleepThread();
    }

    if (!initializePad(port, 0))
    {
    	printf("pad initalization failed!\n");
    	SleepThread();
    }
}

int readPad(int port)
{
	int ret;
	int slot = 0;
	struct padButtonStatus buttons;

	static u32 old_pad = 0;
	u32 new_pad;
	u32 paddata;

	ret = padGetState(port, slot);

	/* We have to wait to pad have the state 'ready' before to do anything. */
	while ((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1))
	{
		if ( ret == PAD_STATE_DISCONN )
			printf("Pad(%d, %d) is disconnected\n", port, slot);
		
		ret = padGetState(port, slot);
	}

	ret = padRead(port, slot, &buttons);

	if (ret != 0)
	{
		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata & ~old_pad;
		old_pad = paddata;

		return (new_pad);
	}
	else
		return (0);
}
