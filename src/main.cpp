#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ncurses.h>

struct BoardCharSet
{
	char mineChar = '*';
	char blankChar = ' ';
	char filledChar = '@';
};

struct BoardCell
{
	char displayChar = '!';
	char actualChar = '!';
	bool cellClicked = false;
};

int RANDOM(int minimum, int maximum)
{
	return (rand() % (maximum - minimum + 1)) + minimum;
}

void initializeBoard(std::vector<std::vector<BoardCell>> &board, int boardHeight, int boardWidth, int boardMines, BoardCharSet boardCharSet)
{
	for(int y = 0; y < boardHeight; y++)
	{
		std::vector<BoardCell> tempRow;
		for(int x = 0; x < boardWidth; x++)
		{
			BoardCell tempBoardCell;
			tempBoardCell.actualChar = boardCharSet.blankChar;
			tempRow.push_back(tempBoardCell);
		}
		board.push_back(tempRow);
	}

	for(int x = 0; x < boardMines - 1; x++)
	{
		BoardCell tempBoardCell;
		tempBoardCell.actualChar = boardCharSet.mineChar;
		board[RANDOM(0, boardHeight - 1)][RANDOM(0, boardWidth - 1)] = tempBoardCell;
	}
}

int main()
{
	srand(std::chrono::system_clock::now().time_since_epoch().count());

	initscr();
	raw(); //pass in ctrl-c ctrl-z and stuff
	keypad(stdscr, true); //allows arrow keys and function keys and mouse events
	noecho();

	mousemask(BUTTON1_CLICKED, NULL);
	MEVENT mouseEvent;

	int input;

	if(!has_colors())
	{
		endwin();
		std::cerr << "Your terminal doesn't have colors!" << std::endl;
		exit(EXIT_FAILURE);
	}
	start_color();
	use_default_colors(); //allows for -1 for transparent background
	init_pair(1, COLOR_CYAN, 0);
	init_pair(2, COLOR_GREEN, 0);
	init_pair(3, COLOR_RED, 0);
	init_pair(9, COLOR_BLACK, -1);

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
	int boardTopPadding = 20;

	BoardCharSet boardCharSet;
	std::vector<std::vector<BoardCell>> board;
	initializeBoard(board, mediumBoardHeight, mediumBoardWidth, mediumBoardMines, boardCharSet);

	while(true)
	{
		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
			}
		}

		input = getch();
		if(input == KEY_MOUSE && getmouse(&mouseEvent) == OK)
		{
			if(mouseEvent.bstate &BUTTON1_CLICKED)
			{
				mvprintw(0, 0, "x: %d", mouseEvent.x);
				mvprintw(1, 0, "y: %d", mouseEvent.y);

				for(int y = 0; y < board.size(); y++)
				{
					for(int x = 0; x < board[y].size(); x++)
					{
						mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].actualChar);
					}
				}
				break;
			}
		}

		if(input == '`' || input == 'q')
		{
			break;
		}
	}
	getch();

	endwin();
	return 0;
}
