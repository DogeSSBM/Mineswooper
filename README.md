# Mineswooper
A configurable implementation of minesweeper.

# Config Options
## Board Length
Pass in two comma seperated numbers as an argument for a board with that length.

`./main.out 40,32`

Will result in a 40 tile wide, by 32 tile high board.

The default board length is 30,16 (30 tiles wide, by 16 tiles high).

When on the "New Game" screen, you can resize the board using WASD / arrow keys -

`W` / `Arrow Up`: Decrease Height

`S` / `Arrow Down`: Increase Height

`A` / `Arrow Left`: Decrease Width

`D` / `Arrow Right`: Increase Width

## Number of Mines
Passing a single number as an argument will specify how many tiles will be mines.

`./main.out 50`

Will generate a board with 50 mines.

By default 99 tiles will be mines.

When on the "New Game" screen, scrolling the mouse wheel down/up will increase/decrease the number of mines.

## Generation strategy
Passing `RNG`, `ADJ`, or `SAT` as an argument will change the method used to generate a board configuration.

`./main.out RNG`: This will randomly generate a board and is the default.

`./main.out ADJ`: This will generate a board that is solvable using basic adjacent reasoning. (The solver will concider only individual tiles and their 8 neighbors when attempting to find a suitable configuration.)

`./main.out SAT`: This will generate a board that is solvable using `ADJ` reasoning as well as SAT solving strategies. (Mostly completed but still a work in progress.)

## Cheats
When on the "New Game" screen, you can toggle cheats by pressing the space bar.

## Multiple Arguments
Multiple arguments can be passed in any order.

Any of the following result in a 30 tile wide, by 16 tile high board, with 128 mines, that is solvable using `SAT` strategies -

`./main.out 30,16 SAT 128` == `./main.out SAT 30,16 128` == `./main.out 128 30,16 SAT`

## Argument Types
Each argument type must only be specified a single time

`./main.out SAT RNG`: Invalid, duplicate generation strategy.

`./main.out 12,24 RNG 10 20,20`: Invalid, duplicate length argument.
