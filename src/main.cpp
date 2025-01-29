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
	char displayChar = '@';
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

void initializeNumbers(std::vector<std::vector<BoardCell>> &board, BoardCharSet boardCharSet)
{
	for(int y = 0; y < board.size(); y++)
	{
		for(int x = 0; x < board[y].size(); x++)
		{
			int tempMineCount = 0;

			if(board[y][x].actualChar != boardCharSet.mineChar)
			{
				//!##
				//#@#
				//###
				if(y - 1 >= 0 && x - 1 >= 0)
				{
					if(board[y - 1][x - 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//#!#
				//#@#
				//###
				if(y - 1 >= 0)
				{
					if(board[y - 1][x].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//##!
				//#@#
				//###
				if(y - 1 >= 0 && x + 1 < board[0].size())
				{
					if(board[y - 1][x + 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//###
				//!@#
				//###
				if(x - 1 >= 0)
				{
					if(board[y][x - 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//###
				//#@!
				//###
				if(x + 1 < board[0].size())
				{
					if(board[y][x + 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//###
				//#@#
				//!##
				if(y + 1 < board.size() && x - 1 >= 0)
				{
					if(board[y + 1][x - 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//###
				//#@#
				//#!#
				if(y + 1 < board.size())
				{
					if(board[y + 1][x].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
				//###
				//#@#
				//##!
				if(y + 1 < board.size() && x + 1 < board[0].size())
				{
					if(board[y + 1][x + 1].actualChar == boardCharSet.mineChar)
					{
						tempMineCount++;
					}
				}
			}

			if(tempMineCount > 0)
			{
				board[y][x].actualChar = tempMineCount + 48; //char 48 == '0' ascii
			}
		}
	}
}

void clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet boardCharSet)
{
	//function assumes that where you clicked was a 'safe location' (not a mine)

	if(clickedY >= board.size() || clickedX >= board[0].size())
	{
		return;
	}

	int up;
	bool keepGoingUp;
	int down;
	bool keepGoingDown;

	//right (includes 'middle'
	int right = 0;
	while(clickedX + right < board[0].size())
	{
		up = 0;
		down = 0;
		keepGoingUp = true;
		keepGoingDown = true;

		while(keepGoingUp || keepGoingDown)
		{
			if(clickedY + up < board.size())
			{
				if(board[clickedY + up][clickedX + right].actualChar != boardCharSet.mineChar)
				{
					board[clickedY + up][clickedX + right].displayChar = board[clickedY + up][clickedX + right].actualChar;
					up++;
				} else
				{
					keepGoingUp = false;
				}
			} else
			{
				keepGoingUp = false;
			}
			if(clickedY - down >= 0)
			{
				if(board[clickedY - down][clickedX + right].actualChar != boardCharSet.mineChar)
				{
					board[clickedY - down][clickedX + right].displayChar = board[clickedY - down][clickedX + right].actualChar;
					down++;
				} else
				{
					keepGoingDown = false;
				}
			} else
			{
				keepGoingDown = false;
			}
		}
		right++;

		if(clickedX + right < board[0].size())
		{
			if(board[clickedY][clickedX + right].actualChar == boardCharSet.mineChar)
			{
				break;
			}
		}
	}

	//left
	int left = 0;
	while(clickedX - left > 0)
	{
		up = 0;
		down = 0;
		keepGoingUp = true;
		keepGoingDown = true;

		while(keepGoingUp || keepGoingDown)
		{
			if(clickedY + up < board.size())
			{
				if(board[clickedY + up][clickedX - left].actualChar != boardCharSet.mineChar)
				{
					board[clickedY + up][clickedX - left].displayChar = board[clickedY + up][clickedX - left].actualChar;
					up++;
				} else
				{
					keepGoingUp = false;
				}
			} else
			{
				keepGoingUp = false;
			}

			if(clickedY - down >= 0)
			{
				if(board[clickedY - down][clickedX - left].actualChar != boardCharSet.mineChar)
				{
					board[clickedY - down][clickedX - left].displayChar = board[clickedY - down][clickedX - left].actualChar;
					down++;
				} else
				{
					keepGoingDown = false;
				}
			} else
			{
				keepGoingDown = false;
			}

		}
		left++;

		if(clickedX - left > 0)
		{
			if(board[clickedY][clickedX - left].actualChar == boardCharSet.mineChar)
			{
				break;
			}
		}
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
	initializeNumbers(board, boardCharSet);

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

				/*
				for(int y = 0; y < board.size(); y++)
				{
					for(int x = 0; x < board[y].size(); x++)
					{
						mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].actualChar);
					}
				}
				*/
				clearBoardWhereClicked(board, mouseEvent.y - boardTopPadding, mouseEvent.x - boardLeftPadding, boardCharSet);
				for(int y = 0; y < board.size(); y++)
				{
					for(int x = 0; x < board[y].size(); x++)
					{
						mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
					}
				}
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
