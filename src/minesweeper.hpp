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
	void clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet, bool revealWholeBoard = false);
	void clearBoardWhereClickedDEPRECATED(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet boardCharSet);
	void flagBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool numberOfFlagsAroundNumberCellMatch(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
	bool numberOfFlagsClearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet);
}

#endif
