#include <graphx.h>
#include <string.h>
#include <tice.h>

#include "palette.h"
#include "words.h"

#ifdef DEBUG
#include <debug.h>
#define debug(...) dbg_sprintf(dbgout, __VA_ARGS__)
#else
#define debug(...) (void *)0
#endif

/* Length of an array */
#define LEN(x) (sizeof(x) / sizeof(*x))

/* Default letter height in graphx library */
#define DEFAULT_LETTER_HEIGHT 8

/* UI sizes */
#define BOX_SIZE            26
#define BOX_MARGIN          4
#define FONT_SCALE          3
#define LETTER_HEIGHT       (FONT_SCALE * DEFAULT_LETTER_HEIGHT)
#define SMALL_FONT_SCALE    2
#define SMALL_LETTER_HEIGHT (SMALL_FONT_SCALE * DEFAULT_LETTER_HEIGHT)

/* Define word parameters */
#define WORD_LENGTH 5
#define WORDS       6

/* Coordinate of top-left corner of grid */
#define TOP ((LCD_HEIGHT - WORDS * BOX_SIZE - (WORDS - 1) * BOX_MARGIN - LETTER_HEIGHT) * 2 / 3 + LETTER_HEIGHT)
#define LEFT ((LCD_WIDTH - WORD_LENGTH * BOX_SIZE - (WORD_LENGTH - 1) * BOX_MARGIN) / 2)

/* color definitions for letter states */
#define COLOR_NONE    LIGHTGRAY /* unsubmitted letters and empty boxes */
#define COLOR_INVALID RED       /* invalid words */
#define COLOR_CORRECT GREEN     /* correct letter, correct place */
#define COLOR_PRESENT YELLOW    /* correct letter, incorrect plalce */
#define COLOR_ABSENT  DARKGRAY  /* incorrect letter */

/* function declarations */
static int16_t binsearch(const char *word, const char **list, int16_t start, int16_t end);
static void clear_status_message(void);
static void draw_boxes(void);
static void draw_letter(uint8_t i, uint8_t j, char c);
static void fill_box(uint8_t i, uint8_t j);
static void handle_correct_guess(void);
static void handle_incorrect_guess(void);
static void handle_invalid_guess(void);
static void handle_key(char c);
static inline bool is_valid(const char *word);
static void print_status_message(const char *msg);
static void print_title(void);
static void setup(void);

/* keysym to character mapping */
static const char *const charmap = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";

/* color of each box */
static uint8_t boxes[WORDS][WORD_LENGTH];
static char letters[WORDS][WORD_LENGTH];

/* puzzle word */
static const char *word;

/* current position */
static uint8_t y, x;

void setup(void)
{
	for (uint8_t i = 0; i < WORDS; i++) {
		for (uint8_t j = 0; j < WORD_LENGTH; j++) {
			boxes[i][j] = COLOR_NONE;
			letters[i][j] = ' ';
		}
	}

	srand(rtc_Time());

	/* pick random word */
	word = puzzle_words[randInt(0, LEN(puzzle_words) - 1)];

	x = y = 0;
}

void fill_box(uint8_t i, uint8_t j)
{
	gfx_SetColor(boxes[i][j]);
	gfx_FillRectangle(
			LEFT + j * (BOX_SIZE + BOX_MARGIN),
			TOP + i * (BOX_SIZE + BOX_MARGIN),
			BOX_SIZE,
			BOX_SIZE
			);
}

void draw_boxes(void)
{
	for (uint8_t i = 0; i < WORDS; i++) {
		for (uint8_t j = 0; j < WORD_LENGTH; j++) {
			fill_box(i, j);
		}
	}
}

void draw_letter(uint8_t i, uint8_t j, char c)
{
	/* use black if box is light gray, otherwise use white */
	gfx_SetTextFGColor((boxes[i][j] == LIGHTGRAY) * BLACK + (boxes[i][j] != LIGHTGRAY) * WHITE);

	/* position cursor in row i box j */
	gfx_SetTextXY(
			LEFT + j * (BOX_SIZE + BOX_MARGIN) + (BOX_SIZE - gfx_GetCharWidth(c)) / 2 + 1,
			TOP + i * (BOX_SIZE + BOX_MARGIN) + (BOX_SIZE - LETTER_HEIGHT) / 2 + 1
		     );
	gfx_PrintChar(c);
}

void handle_correct_guess(void)
{
	for (uint8_t i = 0; i < WORD_LENGTH; i++)
		boxes[y][i] = COLOR_CORRECT;

	/* draw new boxes */
	for (uint8_t i = 0; i < WORD_LENGTH; i++) {
		fill_box(y, i);
		draw_letter(y, i, letters[y][i]);
	}

	y = WORDS;
	x = 0;
}

void handle_invalid_guess(void)
{
	print_status_message("Not in word list.");

	for (uint8_t i = 0; i < WORD_LENGTH; i++)
		boxes[y][i] = COLOR_INVALID;

	/* draw new boxes */
	for (uint8_t i = 0; i < WORD_LENGTH; i++) {
		fill_box(y, i);
		draw_letter(y, i, letters[y][i]);
	}
}

void handle_incorrect_guess(void)
{
	uint8_t freq[26] = { 0 };

	/* count occurrences of each letter in puzzle word */
	for (uint8_t i = 0; i < WORD_LENGTH; i++)
		freq[word[i] - 'A']++;

	/* highlight boxes */
	for (uint8_t i = 0; i < WORD_LENGTH; i++) {
		if (letters[y][i] == word[i]) {
			boxes[y][i] = COLOR_CORRECT;
			freq[letters[y][i] - 'A']--;
		}
	}
	for (uint8_t i = 0; i < WORD_LENGTH; i++) {
		if (boxes[y][i] == COLOR_CORRECT) {
			continue;
		} else if (freq[letters[y][i] - 'A'] > 0) {
			boxes[y][i] = COLOR_PRESENT;
			freq[letters[y][i] - 'A']--;
		} else {
			boxes[y][i] = COLOR_ABSENT;
		}
	}

	/* draw new boxes */
	for (uint8_t i = 0; i < WORD_LENGTH; i++) {
		fill_box(y, i);
		draw_letter(y, i, letters[y][i]);
	}

	x = 0;
	y++;

	/* if game over, reveal puzzle word */
	if (y == WORDS) {
		char msg[] = "The word was:  \0\0\0\0\0";
		strcat(msg, word);
		print_status_message(msg);
	}
}

void print_status_message(const char *msg)
{
	clear_status_message();
	gfx_SetTextFGColor(BLACK);
	gfx_SetTextScale(SMALL_FONT_SCALE, SMALL_FONT_SCALE);
	gfx_PrintStringXY(msg,
			(LCD_WIDTH - gfx_GetStringWidth(msg)) / 2,
			TOP / 2 - SMALL_LETTER_HEIGHT / 2
			);
	gfx_SetTextScale(FONT_SCALE, FONT_SCALE);
}

void clear_status_message(void)
{
	gfx_SetColor(WHITE);
	gfx_FillRectangle(0,
			TOP / 2 - LETTER_HEIGHT / 2,
			LCD_WIDTH,
			LETTER_HEIGHT);
}

void print_title(void)
{
	clear_status_message();
	gfx_SetTextFGColor(BLACK);
	gfx_PrintStringXY(TITLE,
			(LCD_WIDTH - gfx_GetStringWidth(TITLE)) / 2,
			TOP / 2 - LETTER_HEIGHT / 2
			);
}

#define HKEY_ENTER '\x01'
#define HKEY_DEL   '\x02'
#define HKEY_CLEAR '\x03'
void handle_key(char c)
{
	if (y == WORDS)
		return;

	if (c == HKEY_ENTER) { /* enter is pressed */
		if (x != WORD_LENGTH)
			return;

		if (!strncmp(letters[y], word, WORD_LENGTH))
			handle_correct_guess();
		else if (!is_valid(letters[y]))
			handle_invalid_guess();
		else
			handle_incorrect_guess();
	} else if (c == HKEY_DEL) {
		if (x == 0)
			return;

		x--;
		letters[y][x] = ' ';

		/* if previous guess was wrong, clear the red boxes since the
		 * user has acknowledged the message. also clear the status
		 * message and reinstate the title. */
		if (boxes[y][0] == COLOR_INVALID) {
			clear_status_message();
			print_title();
			for (uint8_t i = 0; i < WORD_LENGTH; i++) {
				boxes[y][i] = COLOR_NONE;
				fill_box(y, i);
				draw_letter(y, i, letters[y][i]);
			}
		} else {
			fill_box(y, x);
			draw_letter(y, x, letters[y][x]);
		}
	} else if (c == HKEY_CLEAR) {
		/* if previous guess was wrong, clear the red boxes since the
		 * user has acknowledged the message. also clear the status
		 * message and reinstate the title. */
		if (boxes[y][0] == COLOR_INVALID) {
			clear_status_message();
			print_title();
		}

		for (x = 0; x < WORD_LENGTH; x++) {
			letters[y][x] = ' ';
			boxes[y][x] = COLOR_NONE;
			fill_box(y, x);
		}
		x = 0;
	} else { /* letter A-Z */
		if (x == WORD_LENGTH)
			return;

		letters[y][x] = c;
		draw_letter(y, x, c);
		x++;
	}
}

bool is_valid(const char *word)
{
	/*
	 * For now, use the list of puzzle words as the list of valid words. The
	 * full list of valid words doesn't fit in the memory of the calculator.
	 *
	 * TODO: implement a way to compress the full list of valid words.
	 */
	return binsearch(word, puzzle_words, 0, LEN(puzzle_words)) != -1;
}

int16_t binsearch(const char *word, const char **list, int16_t left, int16_t right)
{
	int16_t middle;
	int cmp;

	if (left >= right)
		return -1;

	middle = (left + right) / 2;

	cmp = strncmp(word, list[middle], WORD_LENGTH);
	if (cmp == 0)
		return middle;
	else if (cmp < 0)
		return binsearch(word, list, left, middle);
	else /* if (cmp > 0) */
		return binsearch(word, list, middle + 1, right);
}

int main(void)
{
	setup();

	gfx_Begin();

	/* set up palette */
	gfx_SetPalette(wordle_colors, sizeof(wordle_colors), 0);

	/* text config */
	gfx_SetTextConfig(gfx_text_noclip);
	gfx_SetTextScale(FONT_SCALE, FONT_SCALE);
	gfx_SetMonospaceFont(0);
	gfx_SetTextBGColor(TRANSPARENT);
	gfx_SetTextTransparentColor(TRANSPARENT);

	/* clear screen */
	gfx_FillScreen(WHITE);

	print_title();
	draw_boxes();

	sk_key_t c;
	while ((c = os_GetCSC()) != sk_2nd) {
		if (c == sk_Enter)
			handle_key(HKEY_ENTER);
		else if (c == sk_Clear)
			handle_key(HKEY_CLEAR);
		else if (c == sk_Del)
			handle_key(HKEY_DEL);
		else if (charmap[c] >= 'A' && charmap[c] <= 'Z')
			handle_key(charmap[c]); /* keypress is a letter */
	}

	gfx_End();
}
