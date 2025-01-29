#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ncurses.h>

static char mineChar = '*';
static char blankChar = ' ';

int RANDOM(int minimum, int maximum)
{
	return (rand() % (maximum - minimum + 1)) + minimum;
}

void initializeBoard(std::vector<std::vector<char>> &board, int boardHeight, int boardWidth, int boardMines)
{
	for(int y = 0; y < boardHeight; y++)
	{
		std::vector<char> tempRow;
		for(int x = 0; x < boardWidth; x++)
		{
			if(RANDOM(0, 1))
			{
				tempRow.push_back(mineChar);
			} else
			{
				tempRow.push_back(blankChar);
			}
		}
		board.push_back(tempRow);
	}
}

int main()
{
	srand(std::chrono::system_clock::now().time_since_epoch().count());

	initscr();
	if(!has_colors())
	{
		endwin();
		std::cerr << "Your terminal doesn't have colors!" << std::endl;
		exit(EXIT_FAILURE);
	}
	start_color();
	init_pair(1, COLOR_CYAN, 0);
	init_pair(2, COLOR_GREEN, 0);
	init_pair(3, COLOR_RED, 0);

	const int easyBoardWidth = 9;
	const int easyBoardHeight = 9;
	const int easyBoardMines = 10;
	const int mediumBoardWidth = 16;
	const int mediumBoardHeight = 16;
	const int mediumBoardMines = 40;
	const int hardBoardWidth = 30;
	const int hardBoardHeight = 16;
	const int hardBoardMines = 99;
	const int extremeBoardWidth = 30;
	const int extremeBoardHeight = 24;
	const int extremeBoardMines = 160;

	int boardLeftPadding = 10;
	int boardTopPadding= 20;

	std::vector<std::vector<char>> board;
	initializeBoard(board, mediumBoardHeight, mediumBoardWidth, mediumBoardMines);

	for(int y = 0; y < board.size(); y++)
	{
		for(int x = 0; x < board[y].size(); x++)
		{
			mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x]);
		}
	}

	getch();

	endwin();
	return 0;
}
