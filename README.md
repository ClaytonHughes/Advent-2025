# Advent-2025
Advent of Code 2025... on the GameBoy?

I had attempted to use the Classic Gameboy to solve some Advent of Code puzzles.

I would have liked to work with the input as provided, but my first input was 19k... which isn't going to fit in 16k of ROM. My options are:

* Read the input from the buttons, entered by a TAS script.
* Read from the serial port?
* Divide the input into pieces, and compute the halves separately and combine them.
(This could work for puzzle 1 part 1, but there's no promise it would for all inputs.)
* Pre-process the input into something that fits more reasonably into the space I have.

Since I'm doing this mostly from my phone with the help of https://gbdev.io/rgbds-live/ and gbdev's documentation, I'm just going to pre-process the data, as boring a solution as that may be.
I'll try to also write a method for processing the original input as-is, to use with the example inputs given in the problems.
