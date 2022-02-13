/**
 * TI Wordle - src/palette.h
 *
 * Copyright (C) 2022 Kian Kasad
 *
 * This file is made available under a modified BSD license. See the provided
 * LICENSE file for more information.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __PALETTE_H__
#define __PALETTE_H__

#include <graphx.h>

#define RGB(r, g, b) gfx_RGBTo1555(r, g, b)

const uint16_t wordle_colors[] = {
	RGB(0x6a, 0xaa, 0x64), /* #6aaa64 */
	RGB(0x53, 0x8d, 0x4e), /* #538d4e */
	RGB(0xc9, 0xb4, 0x58), /* #c9b458 */
	RGB(0xb5, 0x9f, 0x3b), /* #b59f3b */
	RGB(0xd8, 0xd8, 0xd8), /* #d8d8d8 */
	RGB(0x86, 0x88, 0x8a), /* #86888a */
	RGB(0x93, 0x95, 0x98), /* #939598 */
	RGB(0x21, 0x21, 0x21), /* #212121 */
	RGB(0xff, 0xff, 0xff), /* #ffffff */
	RGB(0xed, 0x4a, 0x4a), /* #ed4a4a */
	RGB(0x00, 0x00, 0x00), /* transparent color slot */
};

enum wordle_color_idx {
	GREEN,
	DARKGREEN,
	YELLOW,
	DARKYELLOW,
	LIGHTGRAY,
	GRAY,
	DARKGRAY,
	BLACK,
	WHITE,
	RED,
	TRANSPARENT,
};

#endif /* __PALETTE_H__ */
