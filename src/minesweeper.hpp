/** @file */

#ifndef MINESWEEPER_HPP
#define MINESWEEPER_HPP

#include <vector>

namespace Mines
{
	/** @brief char set that a board will use to represent and display game */
	struct BoardCharSet
	{
		char mineChar = '*'; ///< both representation and display of a mine
		char blankChar = ' '; ///< both representation and display of a clicked blank square
		char filledChar = '#'; ///< both representation and display of a non-clicked square
		char flagChar = '!'; ///< both representation and display of a flagged square
		char badFlagChar = 'X'; ///< both representation and display of a falsely placed flag
	};

	/** @brief represents a board cell that contains a display character, and a hidden actual character beneath it */
	struct BoardCell
	{
		char displayChar = '#'; ///< both representation and display of a display character
		char actualChar = ' '; ///< both representation and display of an actual character
	};

	/**
	 * @brief generate a random integer between minimum and maximum inclusive
	 *
	 * make sure to call srand() at start of program or before calling this function if you want "real" random numbers.
	 * for example: srand(std::chrono::system_clock::now().time_since_epoch().count());
	 *
	 * @param minimum minimum possible integer desired that can be generated
	 * @param maximum maximum possible integer desired that can be generated
	 * @return an integer between minimum and maximum inclusive
	 */
	int RANDOM(int minimum, int maximum);

	/**
	 * @brief initalize entire given board with BoardCharSet.blankChar
	 *
	 * @param board board that you wish to initialize board on
	 * @param boardHeight desired height of the board
	 * @param boardWidth desired width of the board
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	void initializeBoard(std::vector<std::vector<BoardCell>> &board, int boardHeight, int boardWidth, BoardCharSet &boardCharSet);

	/**
	 * @brief generates the mines on the given board, while making sure the clicked location isn't a mine
	 *
	 * function is meant to be called once per game and only once for the first click of a game
	 *
	 * @param board board that you wish to initialize the mines on
	 * @param clickedY y-coordinate of the clicked square (needed so clicked square doesn't generate a mine and result in instant game over)
	 * @param clickedX x-coordinate of the clicked square (needed so clicked square doesn't generate a mine and result in instant game over)
	 * @param boardMines number of mines to be generated
	 * @param boardCharSet BoardCharSet used for the given board
	 * @param keepThreeByThreeSafeSpaceAroundCursor if true, will not place mines in 3x3 area around nor on clicked square
	 */
	void initializeMines(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, int boardMines, BoardCharSet &boardCharSet, bool keepThreeByThreeSafeSpaceAroundCursor = true);

	/**
	 * @brief generates the numbers that indicate the number of mines that are in a given 3x3 area around the square containing said number
	 *
	 * you can omit this function when preparing a board if you wish to play a "blind game" for some reason without numbers
	 *
	 * @param board board that you wish to initialize the numbers on
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	void initializeNumbers(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);

	/**
	 * @brief clears the square clicked on the board along with other adjacent squares that qualify to be cleared in standard minesweeper
	 *
	 * @param board board you wish to clear the square clicked
	 * @param clickedY y-coordinate of the clicked square desired to be cleared
	 * @param clickedX x-coordinate of the clicked square desired to be cleared
	 * @param boardCharSet BoardCharSet used for the given board
	 * @return true if cleared square wasn't (didn't "contain") a mine, false otherwise
	 */
	bool clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);

	/**
	 * @brief places a flag on a board where clicked
	 *
	 * @param board board you wish to flag the square clicked
	 * @param clickedY y-coordinate of the clicked square desired to be flagged
	 * @param clickedX x-coordinate of the clicked square desired to be flagged
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	void flagBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);

	/**
	 * @brief check if n flags that exist in 3x3 area around clicked square (which is assumed to contain a number label from initializeNumbers())
	 * matches the clicked square's number label integer value
	 *
	 * @param board board on which you wish to use this function
	 * @param clickedY y-coordinate of the clicked square desired to be checked with this function
	 * @param clickedX x-coordinate of the clicked square desired to be checked with this function
	 * @param boardCharSet BoardCharSet used for the given board
	 * @return true if number of flags around clicked square match the clicked square's { BoardCell.actualChar - '0' } integer value, false otherwise
	 */
	bool numberOfFlagsAroundNumberCellMatch(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);

	/**
	 * @brief clear 3x3 space around click point. Does not check if click point with n number square value matches n flags around click point.
	 *
	 * @param board board on which you wish to use this function
	 * @param clickedY y-coordinate of the clicked square desired to be checked with this function
	 * @param clickedX x-coordinate of the clicked square desired to be checked with this function
	 * @param boardCharSet BoardCharSet used for the given board
	 * @return true if didn't hit a mine while clearing spaces
	 */
	bool clearBoardWhereClickedAroundNumberCell(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);

	/**
	 * @brief reveals whole board. Reveals bad flags, and hidden safe squares. Does not reveal hidden mines by default
	 *
	 * @param board board on which you wish to use this function
	 * @param boardCharSet BoardCharSet used for the given board
	 * @param revealMines whether to reveal hidden mines or not
	 */
	void revealWholeBoard(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet, bool revealMines = true);

	/**
	 * @brief similar to revealWholeBoard, but only reveals mines
	 *
	 * @param board board on which you wish to use this function
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	void revealOnlyMines(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);

	/**
	 * @brief hides all revealed chars back to the hidden "filled char" state
	 *
	 * @param board board on which you wish to use this function
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	void healBoard(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);

	/**
	 * @brief checks whether all mines have been found by checking if no false flags and no hidden blank squares remain
	 *
	 * @param board board on which you wish to use this function
	 * @param boardCharSet BoardCharSet used for the given board
	 */
	bool haveFoundAllMines(std::vector<std::vector<BoardCell>> &board, int chosenDifficultyBoardMines, BoardCharSet &boardCharSet);
}

#endif
