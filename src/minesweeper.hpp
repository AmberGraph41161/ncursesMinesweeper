#ifndef MINESWEEPER_HPP
#define MINESWEEPER_HPP

#include <vector>

namespace Mines
{
	struct BoardCharSet
	{
		char mineChar = '*';
		char blankChar = ' ';
		char filledChar = '#';
		char flagChar = '!';
		char badFlagChar = 'X';
	};

	struct BoardCell
	{
		char displayChar = '#';
		char actualChar = ' ';
	};

	int RANDOM(int minimum, int maximum);
	void initializeBoard(std::vector<std::vector<BoardCell>> &board, int boardHeight, int boardWidth, BoardCharSet &boardCharSet);
	void initializeMines(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, int boardMines, BoardCharSet &boardCharSet, bool keepThreeByThreeSafeSpaceAroundCursor = true);
	void initializeNumbers(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);
	bool clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet); //returns true if didn't hit mine
	void flagBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool numberOfFlagsAroundNumberCellMatch(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool clearBoardWhereClickedAroundNumberCell(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet); //returns true if didn't hit mine
	void revealWholeBoard(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet, bool revealMines = true);
	void revealOnlyMines(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);
	bool haveFoundAllMines(std::vector<std::vector<BoardCell>> &board, int chosenDifficultyBoardMines, BoardCharSet &boardCharSet);
}

#endif
