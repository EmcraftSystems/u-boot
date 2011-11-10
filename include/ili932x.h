/*
 * (C) Copyright 2011
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __ILI932x_H
#define __ILI932x_H

/**
 * Update graphics memory of the ILI932x controller from the framebuffer memory
 * Note that the driver has no ability to autodetect framebuffer changes,
 * so this function must be called every time the framebuffer content
 * is modified.
 */
void ili932x_update(void);

/**
 * Calculate the size of the framebuffer
 */
ulong calc_fbsize (void);

#endif /* __ILI932x_H */
