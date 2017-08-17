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

#include <string.h>

#include "include/render.h"
#include "include/texture.h"

#include "include/gfont.h"
#include "include/pad.h"

extern u32 consolas_font[];

/* Logo constants. */
#define LOGO_WIDTH  267
#define LOGO_HEIGHT 113
#define LOGO_UV_X   111
#define LOGO_UV_Y   138

/* Arrow constants. */
#define ARROW_WIDTH  48
#define ARROW_HEIGHT 35
#define ARROW_UV_X   349
#define ARROW_UV_Y   84

/* Directional constants. */
#define CONTROL_WIDTH  56
#define CONTROL_HEIGHT 56
#define CONTROL_UV_X   344
#define CONTROL_UV_Y   21

/* X button constants. */
#define XBTN_WIDTH  28
#define XBTN_HEIGHT 28
#define XBTN_UV_X   445
#define XBTN_UV_Y   66

/* Stick constants. */
#define STICK_WIDTH  25
#define STICK_HEIGHT 68
#define STICK_UV_X   303
#define STICK_UV_Y   22

/* Strings pos, introduction. */

/* I hope the GCC will optimize all
 * those values as contants because 
 * reading this is more readable instead
 * of just magical numbers. */
#define EXPL_Y  (LOGO_HEIGHT + 20)
#define WHOS_Y  (EXPL_Y + (FONT_HEIGHT * 4))
#define ARRCI_Y (WHOS_Y + (FONT_HEIGHT))
#define ARRC2_Y (ARRCI_Y + ARROW_HEIGHT - 5)
#define MSGU_X  (ARROW_WIDTH + 10)
#define MSGU_Y  (ARRCI_Y + 5)
#define MSGME_X (MSGU_X)
#define MSGME_Y (MSGU_Y + FONT_HEIGHT + 8 + 5)
#define TIPS_Y  (MSGME_Y + (FONT_HEIGHT * 8))
#define BTNOP_Y (TIPS_Y + FONT_HEIGHT)
#define CTR_X   ( (FONT_WIDTH-5) * 9)
#define CTR_Y   (BTNOP_Y - 20)
#define CTRMS_X (CTR_X + CONTROL_WIDTH + 5)
#define CTRMS_Y (BTNOP_Y)
#define MSGCF_X (CTRMS_X + FONT_WIDTH + 2)
#define MSGCF_Y (CTRMS_Y - 3)
#define MCFCN_X (MSGCF_X + XBTN_WIDTH + 2)
#define MCFCN_Y (CTRMS_Y)

/* Strings pos, images... ingame. */
#define FSTICK_X  (ARROW_WIDTH + 27)
#define FSTICK_Y  (EXPL_Y + FONT_HEIGHT + 5)
#define STICK_PAD (32)
#define CTR2_X    (FONT_WIDTH * 40)
#define CTR2_Y    (EXPL_Y)


/* Game states. */
#define TUTORIAL 1
#define INGAME   2
#define FINISH   3

#define PLAYER   0
#define COMPUTER 1


static u8  curr_state    = TUTORIAL;
static u32 last_key      = PAD_UP;
static u8  curr_layout[] = {1, 3, 5, 7};

static u8  turn = 0;

/* Chooser options. */
static int arrow_y[4];
static int arrow_idx = 0;
static int previous_choose = 0;


/* Main game stuff. */
#define bool int
#define true  1
#define false 0

#define PLAYER_ROW 0
#define PLAYER_RMV 1

static int sticksCount = 16;
static int nBin[] = {0, 1, 10, 11, 100, 101, 110, 111, 1000};
static int playerState = PLAYER_ROW;
static u8 sticksRmv = 0;
static finishFPS = 0;

/* ---------------------------------------------------
 * Game methods
 * ---------------------------------------------------*/

/**
 * Reverses the string order.
 * @p s String to be reversed.
 */
void reverse(char* s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/**
 * Since itoa is not standard, here it is.
 * @p n Number to be converted.
 * @p s Destination.
 */
void itoa(int n, char* s)
{
    int i, sign;

    if ((sign = n) < 0) 
        n = -n;

    i = 0;
    do
    {
        s[i++] = n % 10 + '0';
    }while ((n /= 10) > 0);

    if (sign < 0)
        s[i++] = '-';
    
    s[i] = '\0';
    reverse(s);
}

/**
 * Performs the sum of all sticks, I think.
 * @p column Given column.
 */
int newSum(int column)
{
	int x;
	int sum = 0;

	for (x=0; x<4; x++)
	{
		if (x == column)
			continue;
		
		sum = sum + nBin[ curr_layout[x] ];
	}
	return sum;
}

/**
 * Checks for a secure combination.
 * @p sum Sum of all sticks.
 */
bool isSecure(int sum)
{
	int x;
	bool answer = true;
	char number[4];

	itoa(sum, number);

	if (sum == 1 || sum == 3)
		answer = true;
	else if (sum == 2)
		answer = false;
	else
	{
		for (x = 0; number[x] != '\0'; x++)
		{
			if ( (number[x]-48) % 2 != 0 )
			{
				answer = false;
				break;
			}
		}
	}

	return answer;
}

/**
 * Draw help command.
 */
void drawcommand()
{
	gputs(470, 50, "Buttons:");

	g2d_fill_rect_textured_uv(522, 70, 522 + CONTROL_WIDTH,
		70 + CONTROL_HEIGHT, CONTROL_UV_X, CONTROL_UV_Y,
		CONTROL_WIDTH, CONTROL_HEIGHT);

	gputs(580, 91, "/");

	g2d_fill_rect_textured_uv(597, 86, 597 + XBTN_WIDTH,
		86 + XBTN_HEIGHT, XBTN_UV_X, XBTN_UV_Y,
		XBTN_WIDTH, XBTN_HEIGHT);
}

/**
 * Finalizes the game, announce the winner and asks the
 * player to try again.
 */
void finish()
{
	/* Player loses. */
	if (turn == COMPUTER)
	{
		set_font_size(32);
		gputs(97, EXPL_Y, "YOU LOSE!!!");
	}

	/* Computer loses. */
	else if (turn == PLAYER)
	{
		set_font_size(32);
		gputs(77, EXPL_Y, "YOU WIN!!!");
	}

	/* Wait for 5 seconds and resets everything. */
	finishFPS++;
	if (finishFPS % 300 == 0)
	{
		curr_state = TUTORIAL;
		curr_layout[0] = 1;
		curr_layout[1] = 3;
		curr_layout[2] = 5;
		curr_layout[3] = 7;
		sticksRmv = 0;
		playerState = PLAYER_ROW;
		sticksCount = 16;
		arrow_idx   = 0;
		last_key    = PAD_UP;
		previous_choose = 0;
		finishFPS = 0;

		g2d_clrscr(0, 0, screen_width, screen_height);
	}
}

/**
 * The game properly starts here.
 */
void ingame()
{
	int i, j;
	int stick_x = FSTICK_X;
	int stick_y = FSTICK_Y;
	int button;

	arrow_y[0] = FSTICK_Y + 5;
	arrow_y[1] = FSTICK_Y + 5 + STICK_HEIGHT;
	arrow_y[2] = FSTICK_Y + 5 + (STICK_HEIGHT * 2);
	arrow_y[3] = FSTICK_Y + 5 + (STICK_HEIGHT * 3);

	/* Checks if there's still a move. */
	if (sticksCount > 0)
	{
		if (turn == PLAYER)
		{
			/* Player choosing the desired row. */
			if (playerState == PLAYER_ROW)
			{
				gputs(0, EXPL_Y, "Choose the row you want!");
				drawcommand();

				/* Reads the user choice. */
				button = readPad(0);

				if (button & PAD_UP)
				{
					last_key = PAD_UP;

					/* Erase current option. */
					g2d_clrscr(0, arrow_y[arrow_idx], ARROW_WIDTH,
						arrow_y[arrow_idx] + ARROW_HEIGHT);

					arrow_idx--;
					arrow_idx = (arrow_idx & 3);
				}

				else if (button & PAD_DOWN)
				{
					last_key = PAD_DOWN;

					/* Erase current option. */
					g2d_clrscr(0, arrow_y[arrow_idx], ARROW_WIDTH,
						arrow_y[arrow_idx] + ARROW_HEIGHT);

					arrow_idx++;
					arrow_idx = (arrow_idx & 3);
				}

				else if (button & PAD_CROSS)
				{
					last_key = PAD_CROSS;
					curr_state = INGAME;

					/* Clear the screen and jumps to the game ;-). */
					g2d_clrscr(0, LOGO_HEIGHT, screen_width, screen_height);

					previous_choose = arrow_idx;
					playerState     = PLAYER_RMV;
					return;
				}

				g2d_fill_rect_textured_uv(0, arrow_y[arrow_idx], ARROW_WIDTH,
					arrow_y[arrow_idx] + ARROW_HEIGHT, ARROW_UV_X, ARROW_UV_Y,
					ARROW_WIDTH, ARROW_HEIGHT);
			}

			/* Player choosing the amount of sticks. */
			else
			{
				gprintf(0, EXPL_Y, "How many sticks you want remove?: %d",
					sticksRmv);
				drawcommand();

				/* Reads the user choice. */
				button = readPad(0);

				if (button & PAD_UP)
				{
					last_key = PAD_UP;

					/* Erase current option. */
					g2d_clrscr(362, EXPL_Y, 394, FONT_HEIGHT);

					sticksRmv++;
					sticksRmv = (sticksRmv & 7);
				}

				else if (button & PAD_DOWN)
				{
					last_key = PAD_DOWN;

					/* Erase current option. */
					g2d_clrscr(362, EXPL_Y, 394, FONT_HEIGHT);

					sticksRmv--;
					sticksRmv = (sticksRmv & 7);
				}

				else if (button & PAD_CROSS)
				{
					last_key = PAD_CROSS;
					curr_state = INGAME;

					/* Clear the screen and jumps to the game ;-). */
					g2d_clrscr(0, LOGO_HEIGHT, screen_width, screen_height);

					previous_choose = arrow_idx;
					arrow_idx = 0;

					curr_layout[previous_choose] -= sticksRmv;
					sticksCount -= sticksRmv;

					playerState = PLAYER_ROW;
					turn = COMPUTER;
					return;
				}

				g2d_fill_rect_textured_uv(0, arrow_y[arrow_idx], ARROW_WIDTH,
					arrow_y[arrow_idx] + ARROW_HEIGHT, ARROW_UV_X, ARROW_UV_Y,
					ARROW_WIDTH, ARROW_HEIGHT);
			}
		}

		/* Computer turn. */
		else if (turn == COMPUTER)
		{
			gputs(0, EXPL_Y, "Thats my turn, can I play?");
			drawcommand();
			
			button = readPad(0);
			if (button & PAD_CROSS)
			{
				int x, y;
				int  sum;
				bool isSec = false;

				for (x = 1; x < 7 && !isSec; x++)
				{
					for (y = 0; y < 4 && !isSec; y++)
					{
		            	if ( (curr_layout[y] == 0) || (curr_layout[y] - x < 0) )
		            		continue;

		            	sum = newSum(y) + nBin[ curr_layout[y]-x ];
		            	
		            	if (isSecure(sum) == true)
		            	{
		            		isSec = true;
		            		curr_layout[y] -= x;
		            		sticksCount -= x;
		            	}
		            }
		    	}

		    	if (isSec == false)
		    	{
		    		for (x = 0; x < 4; x++)
		    		{
		    			if (curr_layout[x] != 0)
		    			{
		    				curr_layout[x] -= 1;
		    				sticksCount--;
		    				break;
		    			}
		    		}
		    	}

		    	g2d_clrscr(0, LOGO_HEIGHT, screen_width, screen_height);
		    	turn = PLAYER;
			}
		}
	
		/* Draws the sticks. */
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < curr_layout[i]; j++)
			{
				g2d_fill_rect_textured_uv(
					stick_x, 
					stick_y,
					stick_x + STICK_WIDTH,
					stick_y + STICK_HEIGHT - 16,
					STICK_UV_X,
					STICK_UV_Y,
					STICK_WIDTH,
					STICK_HEIGHT);

				stick_x += (STICK_WIDTH + STICK_PAD);
			}

			stick_x =  FSTICK_X;
			stick_y += STICK_HEIGHT;
		}
	}
	else
	{
		/* Clear the screen. */
		g2d_clrscr(0, 0, screen_width, screen_height);
		curr_state = FINISH;
	}
}

void tutorial()
{
	int button;

	/* Draws the graphical stuff. */
	set_font_size(0);
	gputs(0, EXPL_Y, "The NIM game consists of removing the sticks from the"
					 " table, the amount you want, a single row at a time. The"
					 " last to remove the sticks LOSES!! Good luck!!!");

	gputs(0, WHOS_Y, "Who starts:");
	
	/* Checks for player input. */
	button = readPad(0);

	/* Setup the 'chooser' */
	arrow_y[0] = ARRCI_Y;
	arrow_y[1] = ARRC2_Y;

	if (button & PAD_UP)
	{
		last_key = PAD_UP;

		/* Erase 2 option. */
		g2d_clrscr(0, arrow_y[arrow_idx], ARROW_WIDTH, arrow_y[arrow_idx]
			+ ARROW_HEIGHT);

		arrow_idx--;
		arrow_idx = (arrow_idx & 1);
	}

	else if (button & PAD_DOWN)
	{
		last_key = PAD_DOWN;

		/* Erase 1 option. */
		g2d_clrscr(0, arrow_y[arrow_idx], ARROW_WIDTH, arrow_y[arrow_idx]
			+ ARROW_HEIGHT);

		arrow_idx++;
		arrow_idx = (arrow_idx & 1);
	}

	else if (button & PAD_CROSS)
	{
		last_key = PAD_CROSS;
		curr_state = INGAME;
 
		/* Clear the screen and jumps to the game ;-). */
		g2d_clrscr(0, LOGO_HEIGHT, screen_width, screen_height);

		previous_choose = arrow_idx;
		turn = arrow_idx;
		arrow_idx = 0;

		return;
	}

	/* Draws the 'arrow'. */
	g2d_fill_rect_textured_uv(0, arrow_y[arrow_idx], ARROW_WIDTH,
		arrow_y[arrow_idx] + ARROW_HEIGHT, ARROW_UV_X, ARROW_UV_Y,
		ARROW_WIDTH, ARROW_HEIGHT);

	set_font_size(8);
	gputs(MSGU_X, MSGU_Y,   "YOU");
	gputs(MSGME_X, MSGME_Y, "ME");

	set_font_size(0);
	gputs(0, TIPS_Y, "Tips:");
	gputs(0, BTNOP_Y, "Navigate ");

	g2d_fill_rect_textured_uv(CTR_X, CTR_Y, CTR_X + CONTROL_WIDTH,
		CTR_Y + CONTROL_HEIGHT, CONTROL_UV_X, CONTROL_UV_Y,
		CONTROL_WIDTH, CONTROL_HEIGHT);

	gputs(CTRMS_X, CTRMS_Y, "/ ");

	g2d_fill_rect_textured_uv(MSGCF_X, MSGCF_Y, MSGCF_X + XBTN_WIDTH,
		MSGCF_Y + XBTN_HEIGHT, XBTN_UV_X, XBTN_UV_Y, XBTN_WIDTH,
		XBTN_HEIGHT);

	gputs(MCFCN_X, MCFCN_Y, "to confirm");
}

int main()
{
	/* Setup the graphical environment. */
	render_setup();

	/* Setup textures. */
	loadTexture( (void *)&consolas_font, 512, 256 );
	setupTexture(512, 256);
	g2d_set_fill_color(0, 0, 0);

	/* Setup pad. */
	setupPad(0);

	while(1)
	{
		beginFrame();

		g2d_fill_rect_textured_uv(0, 0, LOGO_WIDTH, LOGO_HEIGHT, LOGO_UV_X,
			LOGO_UV_Y, LOGO_WIDTH, LOGO_HEIGHT);

		if (curr_state == TUTORIAL)
			tutorial();

		else if (curr_state == INGAME)
			ingame();

		else if (curr_state == FINISH)
			finish();

		endFrame();
	}

	/* Frees everything. */
	render_exit();

	return (0);
}
