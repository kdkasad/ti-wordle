# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
TITLE = TI WORDLE
AUTHOR = Kian Kasad
YEAR = 2022

ICON = icon.png
DESCRIPTION = "Wordle"
COMPRESSED = YES
ARCHIVED = NO

METADATA_CFLAGS = -DPROGNAME="\"$(NAME)\"" -DTITLE="\"$(TITLE)\"" -DAUTHOR="\"$(AUTHOR)\"" -DYEAR="\"$(YEAR)\""

CFLAGS = -Wall -Wextra -pedantic -Oz $(METADATA_CFLAGS)
CXXFLAGS = -Wall -Wextra -pedantic -Oz $(METADATA_CFLAGS)

# ----------------------------

include $(shell cedev-config --makefile)
