# 
# Copyright(C) 2016 Davidson Francis <davidsondfgl@gmail.com>
#
# This file is part of NIM - PS2 Edition.
#
# NIM is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# NIM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NIM. If not, see <http://www.gnu.org/licenses/>.
#

EE_BIN = nim.elf
EE_OBJS = render.o texture.o consolas.o consolas_tc.o gfont.o pad.o nim.o
EE_LIBS = -lpacket -ldma -lgraph -ldraw -lc -lmf -lpad

all: $(EE_BIN)
	$(EE_STRIP) --strip-all $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
