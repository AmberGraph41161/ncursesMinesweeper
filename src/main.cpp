#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>

#include "minesweeper.hpp"

//https://www.youtube.com/watch?v=YyyhQ0B3huo
#define CUSTOM_COLOR_ONE 21
#define CUSTOM_COLOR_TWO 22
#define CUSTOM_COLOR_THREE 23
#define CUSTOM_COLOR_FOUR 24
#define CUSTOM_COLOR_FIVE 25
#define CUSTOM_COLOR_SIX 26
#define CUSTOM_COLOR_SEVEN 27
#define CUSTOM_COLOR_EIGHT 28
#define CUSTOM_COLOR_NINE 29

/*
init_color(COLOR_RED, 700, 0, 0);
//param 1     : color name
//param 2, 3, 4 : rgb content min = 0, max = 1000

//do RGB256/256, and get first 3 nums after decimal
*/

int main()
{
	srand(std::chrono::system_clock::now().time_since_epoch().count());

	initscr();
	raw(); //pass in ctrl-c ctrl-z and stuff
	keypad(stdscr, true); //allows arrow keys and function keys and mouse events
	noecho();

	mousemask(BUTTON1_CLICKED, NULL);
	MEVENT mouseEvent;
	int clickedX = 0;
	int clickedY = 0;

	int input;

	if(!has_colors() || !can_change_color())
	{
		endwin();
		std::cerr << "Your terminal doesn't have colors!" << std::endl;
		exit(EXIT_FAILURE);
	}
	start_color();
	use_default_colors(); //allows for -1 for transparent background

	init_color(CUSTOM_COLOR_ONE, 0, 0, 996);
	init_color(CUSTOM_COLOR_TWO, 0, 507, 0);
	init_color(CUSTOM_COLOR_THREE, 992, 0, 0);
	init_color(CUSTOM_COLOR_FOUR, 0, 0, 515);
	init_color(CUSTOM_COLOR_FIVE, 515, 0, 0);
	init_color(CUSTOM_COLOR_SIX, 0, 507, 515);
	init_color(CUSTOM_COLOR_SEVEN, 515, 0, 515);
	init_color(CUSTOM_COLOR_EIGHT, 457, 457, 457);
	init_color(CUSTOM_COLOR_NINE, 738, 738, 738);

	init_pair(1, CUSTOM_COLOR_ONE, -1);
	init_pair(2, CUSTOM_COLOR_TWO, -1);
	init_pair(3, CUSTOM_COLOR_THREE, -1);
	init_pair(4, CUSTOM_COLOR_FOUR, -1);
	init_pair(5, CUSTOM_COLOR_FIVE, -1);
	init_pair(6, CUSTOM_COLOR_SIX, -1);
	init_pair(7, CUSTOM_COLOR_SEVEN, -1);
	init_pair(8, CUSTOM_COLOR_EIGHT, -1);
	init_pair(9, CUSTOM_COLOR_NINE, -1);

	const int easyBoardHeight = 9;
	const int easyBoardWidth = 9;
	const int easyBoardMines = 10;
	const int mediumBoardHeight = 16;
	const int mediumBoardWidth = 16;
	const int mediumBoardMines = 40;
	const int hardBoardHeight = 16;
	const int hardBoardWidth = 30;
	const int hardBoardMines = 99;
	const int extremeBoardHeight = 24;
	const int extremeBoardWidth = 30;
	const int extremeBoardMines = 160;

	int chosenDifficultyBoardWidth = mediumBoardWidth;
	int chosenDifficultyBoardHeight = mediumBoardHeight;
	int chosenDifficultyBoardMines = mediumBoardMines;

	int boardLeftPadding = 0;
	int boardTopPadding = 0;

	Mines::BoardCharSet boardCharSet;
	std::vector<std::vector<Mines::BoardCell>> board;
	Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
	bool haveNotInitializedMinesYet = true;

	while(true)
	{
		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				if(board[y][x].displayChar >= '0' && board[y][x].displayChar <= '9')
				{
					attron(COLOR_PAIR(board[y][x].displayChar - '0'));
					mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
					attroff(COLOR_PAIR(board[y][x].displayChar - '0'));
				} else if(board[y][x].displayChar !=  boardCharSet.blankChar)
				{
					attron(COLOR_PAIR(9));
					mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
					attroff(COLOR_PAIR(9));
				} else
				{
					mvprintw(y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
				}
			}
		}

		input = getch();
		if(input == KEY_MOUSE && getmouse(&mouseEvent) == OK)
		{
			if(mouseEvent.bstate &BUTTON1_CLICKED)
			{
				clickedX = mouseEvent.x;
				clickedY = mouseEvent.y;

				//mvprintw(0, 0, "x: %d", clickedX);
				//mvprintw(1, 0, "y: %d", clickedY);

				if(haveNotInitializedMinesYet)
				{
					Mines::initializeMines(board, clickedY, clickedX, chosenDifficultyBoardMines, boardCharSet);
					Mines::initializeNumbers(board, boardCharSet);
					haveNotInitializedMinesYet = false;
				}

				Mines::clearBoardWhereClicked(board, clickedY - boardTopPadding, clickedX - boardLeftPadding, boardCharSet);
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
