# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
ICON = icon.png
DESCRIPTION = "Wordle"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -pedantic -Oz
CXXFLAGS = -Wall -Wextra -pedantic -Oz

# ----------------------------

include $(shell cedev-config --makefile)
