#include <graphx.h>
#include <string.h>
#include <tice.h>

#include "words.h"

#define LEN(x) (sizeof(x) / sizeof(*x))

#define DEFAULT_LETTER_HEIGHT 8

#define BOX_SIZE      20
#define BOX_MARGIN    4
#define FONT_SCALE    2    /* results in 16px letter height */
#define LETTER_HEIGHT (FONT_SCALE * DEFAULT_LETTER_HEIGHT)

#define WORD_LENGTH 5
#define WORDS       6

#define GREEN  4
#define RED    160
#define BLACK  0
#define BROWN  32
#define GRAY   BLACK /* 181 for light gray */
#define YELLOW 228
#define WHITE  255

#define TOP ((LCD_HEIGHT - WORDS * BOX_SIZE - (WORDS - 1) * BOX_MARGIN) / 2)
#define LEFT ((LCD_WIDTH - WORD_LENGTH * BOX_SIZE - (WORD_LENGTH - 1) * BOX_MARGIN) / 2)

/* function declarations */
static int16_t binsearch(const char *word, const char **list, int16_t start, int16_t end);
static void draw_boxes(void);
static void draw_letter(uint8_t y, uint8_t x, char c);
static void fill_box(uint8_t y, uint8_t x);
static void handle_key(char c);
static void setup(void);
static inline bool is_valid(const char *word);

/* keysym to character mapping */
const char *charmap = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";

/* color of each box */
static uint8_t boxes[WORDS][WORD_LENGTH];
static char letters[WORDS][WORD_LENGTH];

/* puzzle word */
static const char *word;

void setup(void)
{
	rtc_Enable(0);

	for (uint8_t y = 0; y < WORDS; y++) {
		for (uint8_t x = 0; x < WORD_LENGTH; x++) {
			boxes[y][x] = GRAY;
			letters[y][x] = ' ';
		}
	}

	srand(rtc_Time());
	rtc_Disable();

	/* pick random word */
	word = puzzle_words[randInt(0, LEN(puzzle_words) - 1)];
}

void fill_box(uint8_t y, uint8_t x)
{
	gfx_SetColor(boxes[y][x]);
	gfx_FillRectangle(
			LEFT + x * (BOX_SIZE + BOX_MARGIN),
			TOP + y * (BOX_SIZE + BOX_MARGIN),
			BOX_SIZE,
			BOX_SIZE
			);
}

void draw_boxes(void)
{
	for (uint8_t y = 0; y < WORDS; y++) {
		for (uint8_t x = 0; x < WORD_LENGTH; x++) {
			fill_box(y, x);
		}
	}
}

void draw_letter(uint8_t y, uint8_t x, char c)
{
	gfx_SetTextFGColor(WHITE);
	gfx_SetTextBGColor(boxes[y][x]);
	gfx_SetTextTransparentColor(boxes[y][x]);

	/* position cursor in box (x, y) */
	gfx_SetTextXY(
			LEFT + x * (BOX_SIZE + BOX_MARGIN) + (BOX_SIZE - gfx_GetCharWidth(c)) / 2,
			TOP + y * (BOX_SIZE + BOX_MARGIN) + (BOX_SIZE - LETTER_HEIGHT) / 2
		     );
	gfx_PrintChar(c);
}

#define HKEY_ENTER '\x01'
#define HKEY_DEL   '\x02'
#define HKEY_CLEAR '\x03'
void handle_key(char c)
{
	static uint8_t x = 0;
	static uint8_t y = 0;
	static bool last_guess_invalid = false;

	if (c == HKEY_ENTER) { /* enter is pressed */
		if (x == WORD_LENGTH) {
			if (!strncmp(letters[y], word, WORD_LENGTH)) {
				/* correct guess */

				for (uint8_t i = 0; i < WORD_LENGTH; i++)
					boxes[y][i] = GREEN;

				/* draw new boxes */
				for (uint8_t i = 0; i < WORD_LENGTH; i++) {
					fill_box(y, i);
					draw_letter(y, i, letters[y][i]);
				}

				y = WORDS;
				x = 0;

			} else if (!is_valid(letters[y])) {
				/* invalid word */

				for (uint8_t i = 0; i < WORD_LENGTH; i++)
					boxes[y][i] = RED;
				last_guess_invalid = true;

				/* draw new boxes */
				for (uint8_t i = 0; i < WORD_LENGTH; i++) {
					fill_box(y, i);
					draw_letter(y, i, letters[y][i]);
				}

			} else {
				/* valid word, incorrect guess */

				uint8_t freq[26] = { 0 };

				/* count occurrences of each letter in puzzle word */
				for (uint8_t i = 0; i < WORD_LENGTH; i++)
					freq[word[i] - 'A']++;

				/* highlight boxes */
				for (uint8_t i = 0; i < WORD_LENGTH; i++) {
					if (letters[y][i] == word[i]) {
						boxes[y][i] = GREEN;
						freq[letters[y][i] - 'A']--;
					} else if (freq[letters[y][i] - 'A'] > 0) {
						boxes[y][i] = YELLOW;
						freq[letters[y][i] - 'A']--;
					}
				}

				/* draw new boxes */
				for (uint8_t i = 0; i < WORD_LENGTH; i++) {
					fill_box(y, i);
					draw_letter(y, i, letters[y][i]);
				}

				x = 0;
				y++;
			}

		}
	} else if (c == HKEY_DEL) {
		if (x == 0)
			return;

		x--;
		letters[y][x] = ' ';

		if (last_guess_invalid) {
			last_guess_invalid = false;
			for (uint8_t i = 0; i < WORD_LENGTH; i++) {
				boxes[y][i] = GRAY;
				fill_box(y, i);
				draw_letter(y, i, letters[y][i]);
			}
		} else {
			fill_box(y, x);
			draw_letter(y, x, letters[y][x]);
		}
	} else if (c == HKEY_CLEAR) {
		last_guess_invalid = false;
		for (x = 0; x < WORD_LENGTH; x++) {
			letters[y][x] = ' ';
			boxes[y][x] = GRAY;
			fill_box(y, x);
		}
		x = 0;
	} else {
		/* letter A-Z */

		if (x == WORD_LENGTH)
			return;
		if (y == WORDS)
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

	/* text config */
	gfx_SetTextConfig(gfx_text_noclip);
	gfx_SetTextScale(2, 2);
	gfx_SetMonospaceFont(0);

	/* clear screen */
	gfx_FillScreen(255);

	draw_boxes();

	sk_key_t c;
	while ((c = os_GetCSC()) != sk_2nd) {
		if (c == sk_Enter) {
			handle_key(HKEY_ENTER);
		} else if (c == sk_Clear) {
			handle_key(HKEY_CLEAR);
		} else if (c == sk_Del) {
			handle_key(HKEY_DEL);
		} else if (charmap[c] >= 'A' && charmap[c] <= 'Z') {
			/* keypress is a letter */
			handle_key(charmap[c]);
		}
	}

	gfx_End();
}
