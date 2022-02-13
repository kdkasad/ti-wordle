# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
ICON = icon.png
DESCRIPTION = "Wordle"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -pedantic -Oz -DPROGNAME="\"$(NAME)\""
CXXFLAGS = -Wall -Wextra -pedantic -Oz -DPROGNAME="\"$(NAME)\""

# ----------------------------

include $(shell cedev-config --makefile)
