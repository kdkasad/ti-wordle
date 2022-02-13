# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
TITLE = TI WORDLE
ICON = icon.png
DESCRIPTION = "Wordle"
COMPRESSED = YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -pedantic -Oz -DPROGNAME="\"$(NAME)\"" -DTITLE="\"$(TITLE)\""
CXXFLAGS = -Wall -Wextra -pedantic -Oz -DPROGNAME="\"$(NAME)\"" -DTITLE="\"$(TITLE)\""

# ----------------------------

include $(shell cedev-config --makefile)
