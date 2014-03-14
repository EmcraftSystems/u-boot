# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

# This should be done with .incbin and Position-Independent-Code

SPIFILIB_VERSION ?= M3

spifilib-envm.bin: spifilib-envm
	$(OBJCOPY) -O binary $< $@

spifilib-dram.bin: spifilib-dram
	$(OBJCOPY) -O binary $< $@

spifilib-envm: spifilib.o spifi_drv_$(SPIFILIB_VERSION).lib spifilib-envm.lds
	$(LD) $(LDLAGS) -Map spifilib.map -Tspifilib-envm.lds -nostdlib -o $@ spifilib.o spifi_drv_$(SPIFILIB_VERSION).lib

spifilib-dram: spifilib.o spifi_drv_$(SPIFILIB_VERSION).lib spifilib-dram.lds
	$(LD) $(LDLAGS) -Map spifilib.map -Tspifilib-dram.lds -nostdlib -o $@ spifilib.o spifi_drv_$(SPIFILIB_VERSION).lib

spifilib.o: spifilib.h spifilib.c
	$(CC) $(CFLAGS) -fshort-wchar -fshort-enums -c $+

clean:
	$(RM) spifilib.o spifilib-envm spifilib-dram spifilib-envm.bin spifilib-dram.bin
