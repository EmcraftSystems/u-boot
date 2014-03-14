#
# (C) Copyright 2010,2011
# Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
#
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
PLATFORM_CPPFLAGS += -DCONFIG_MEM_NVM_BASE=$(CONFIG_MEM_NVM_BASE)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_NVM_LEN=$(CONFIG_MEM_NVM_LEN)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_NVM_UBOOT_OFF=$(if $(CONFIG_MEM_NVM_UBOOT_OFF),$(CONFIG_MEM_NVM_UBOOT_OFF),0x0)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_RAM_BASE=$(CONFIG_MEM_RAM_BASE)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_RAM_LEN=$(CONFIG_MEM_RAM_LEN)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_RAM_BUF_LEN=$(CONFIG_MEM_RAM_BUF_LEN)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_MALLOC_LEN=$(CONFIG_MEM_MALLOC_LEN)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_STACK_LEN=$(CONFIG_MEM_STACK_LEN)
ifdef CONFIG_MEM_RAMCODE_BASE
PLATFORM_CPPFLAGS += -DCONFIG_MEM_RAMCODE_BASE=$(CONFIG_MEM_RAMCODE_BASE)
PLATFORM_CPPFLAGS += -DCONFIG_MEM_RAMCODE_LEN=$(CONFIG_MEM_RAMCODE_LEN)
endif

PLATFORM_CPPFLAGS += -I$(TOPDIR)/cpu/$(CPU)

PLATFORM_RELFLAGS += -g2 -mthumb -mcpu=cortex-m3 -fsigned-char -O2 -fno-builtin-puts -fno-common -ffixed-r8

