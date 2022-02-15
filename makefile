#
# TI Wordle - makefile
#
# Copyright (C) 2022 Kian Kasad
#
# This file is made available under a modified BSD license. See the provided
# LICENSE file for more information.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

NAME = WORDLE
TITLE = TI WORDLE
VERSION = v1.1.2
AUTHOR = Kian Kasad
YEAR = 2022

ICON = icon.png
DESCRIPTION = "Wordle"
COMPRESSED = YES
ARCHIVED = YES

METADATA_CFLAGS = -DPROGNAME="\"$(NAME)\"" \
		  -DTITLE="\"$(TITLE)\"" \
		  -DVERSION="\"$(VERSION)\"" \
		  -DAUTHOR="\"$(AUTHOR)\"" \
		  -DYEAR="\"$(YEAR)\""

CFLAGS = -Wall -Wextra -pedantic -Oz $(METADATA_CFLAGS)
CXXFLAGS = -Wall -Wextra -pedantic -Oz $(METADATA_CFLAGS)

# ----------------------------

include $(shell cedev-config --makefile)
