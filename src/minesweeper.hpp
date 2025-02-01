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
		char actualChar = '_';
		bool cellClicked = false;
	};

	int RANDOM(int minimum, int maximum);
	void initializeBoard(std::vector<std::vector<BoardCell>> &board, int boardHeight, int boardWidth, BoardCharSet &boardCharSet);
	void initializeMines(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, int boardMines, BoardCharSet &boardCharSet);
	void initializeNumbers(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet);
	bool clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet); //returns true if didn't hit mine
	void clearBoardWhereClickedDEPRECATED(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet boardCharSet);
	void flagBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool numberOfFlagsAroundNumberCellMatch(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool clearBoardWhereClickedAroundNumberCell(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet); //returns true if didn't hit mine
	void revealWholeBoard(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet, bool revealMines = true);
}

#endif
