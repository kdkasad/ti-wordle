<!--
    TI Wordle - README.md

    Copyright (C) 2022 Kian Kasad

    This file is made available under a modified BSD license. See the provided
    LICENSE file for more information.

    SPDX-License-Identifier: BSD-2-Clause-Patent
-->

# TI Wordle

A clone of [Wordle](https://powerlanguage.co.uk/wordle/) for TI-84/83 graphing
calculators.

## Building

To build TI Wordle, install the [CE-Programming
toolchain](https://github.com/CE-Programming/toolchain/), then run `make` in
the root directory of this repository. The resulting binary will be built as
`bin/WORDLE.8xp`.

## Installing

Use a connection program to upload the program to a calculator using a USB cable. Some options include:

 - [TI Connect™](https://education.ti.com/en/products/computer-software/ti-connect-sw) (Windows)
 - [TILP](http://lpg.ticalc.org/prj_tilp/) (Linux/Windows/MacOS/UNIX)

## Usage

Once TI Wordle has been started on the calculator, you can start typing words
to guess. You don't need to press the `alpha` or `A-lock` buttons; TI Wordle
will automatically translate keypresses to letters.

### Special keys

| Key     | Function                                                |
| ---     | ---                                                     |
| `2nd`   | Quits the program                                       |
| `del`   | Deletes one letter in the current line (like backspace) |
| `clear` | Clears the current line                                 |
| `enter` | Submits the current line as a guess                     |

### UI

Once you press `enter` to submit a line, the letters will become highlighted
according to the same rules followed by original Wordle, along with [a few
tweaks](#invalid-guesses).

#### Valid guesses

If the typed word is a valid word, any letters that are in the "puzzle word"
(i.e. the one you're trying to guess) will be highlighted. They'll turn green
if they occur in the same position in the puzzle word and yellow if they occur
in the puzzle word but in a different postion.

> Note: this part is the same as original Wordle

#### Invalid guesses

If the typed word is not in the list of valid words, the entire line will be
highlighted red, and the user can continue to edit it.

## Inconsistencies with original Wordle

In order to make the game fit in the RAM space of a TI-84 calculator, the list
of all possible valid words had to be removed. Instead, the validity of a word
is determined by whether or not it occurs in the list of puzzle words (see
above for definition). This narrows the list of possible words from 10,638 to
only 2,315. Still, I feel that this is adequate enough to yield an enjoyable
playing experience.
