#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>
#include <vector>

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

void drawBoard(WINDOW* window, std::vector<std::vector<Mines::BoardCell>> &board, int &boardTopPadding, int &boardLeftPadding, Mines::BoardCharSet boardCharSet)
{
	for(int y = 0; y < board.size(); y++)
	{
		for(int x = 0; x < board[y].size(); x++)
		{
			if(board[y][x].displayChar >= '0' && board[y][x].displayChar <= '9')
			{
				attron(COLOR_PAIR(board[y][x].displayChar - '0'));
				mvwprintw(window, y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
				attroff(COLOR_PAIR(board[y][x].displayChar - '0'));
			} else if(board[y][x].displayChar !=  boardCharSet.blankChar)
			{
				attron(COLOR_PAIR(9));
				mvwprintw(window, y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
				attroff(COLOR_PAIR(9));
			} else
			{
				mvwprintw(window, y + boardTopPadding, x + boardLeftPadding, "%c", board[y][x].displayChar);
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

	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED | BUTTON3_PRESSED | BUTTON3_RELEASED, NULL);
	mouseinterval(0);
	MEVENT mouseEvent;
	int clickedY = 0;
	int clickedX = 0;

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

	bool allowLeftButtonDownToFlag = true;
	int mouseLeftButtonDownToFlagThresholdInTenthsOfSeconds = 2;

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

	int chosenDifficultyBoardHeight = 1;
	int chosenDifficultyBoardWidth = 1;
	int chosenDifficultyBoardMines = 1;

	std::vector<std::string> smileyFaces =
	{
		":o", //0 left pressed
		":3", //1 left held to flag
		":D", //2 left released
		";p", //3 right pressed to flag
		":p", //4 right released after flag
		"X[", //5 hit a mine
		":?", //6 clicked keyboard not mouse
		":)", //7 default (nothing clicked/pressed)
	};

	int boardLeftPadding = 0;
	int boardTopPadding = 0;
	int smileyFaceInformationTopPadding = 0;
	int smileyFaceInformationLeftPadding = 1;
	int chosenDifficultyInformationTopPadding = 1;
	int chosenDifficultyInformationLeftPadding = 1;
	int gameOverInformationTopPadding = 2;
	int gameOverInformationLeftPadding = 1;
	int gameWonInformationTopPadding = 2;
	int gameWonInformationLeftPadding = 1;

	Mines::BoardCharSet boardCharSet;
	std::vector<std::vector<Mines::BoardCell>> board;
	bool haveNotInitializedMinesYet = true;

	bool gameOver = false;
	bool startMenu = true;
	bool pauseMenu = false;
	bool gameplayMenu = false;
	while(true)
	{
		if(startMenu)
		{
			clear();
			mvprintw(0, 0, "Choose Difficulty (press number keys on keyboard)");
			attron(COLOR_PAIR(2));
			mvprintw(1, 0, "1. Easy");
			attroff(COLOR_PAIR(2));
			attron(COLOR_PAIR(1));
			mvprintw(2, 0, "2. Medium");
			attroff(COLOR_PAIR(1));
			attron(COLOR_PAIR(3));
			mvprintw(3, 0, "3. Hard");
			attroff(COLOR_PAIR(3));
			attron(COLOR_PAIR(6));
			mvprintw(4, 0, "4. Extreme");
			attroff(COLOR_PAIR(6));

			input = getch();
			clear();

			if(input == KEY_MOUSE && getmouse(&mouseEvent) == OK && (mouseEvent.bstate &BUTTON1_PRESSED))
			{
				clickedY = mouseEvent.y;
				clickedX = mouseEvent.x;

				switch(clickedY)
				{
					case 1:
						input = '1';
						break;

					case 2:
						input = '2';
						break;

					case 3:
						input = '3';
						break;

					case 4:
						input = '4';
						break;

					default:
						input = '1';
						break;
				}
			}

			switch(input)
			{
				case '1':
					chosenDifficultyBoardHeight = easyBoardHeight;
					chosenDifficultyBoardWidth = easyBoardWidth;
					chosenDifficultyBoardMines = easyBoardMines;
					mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyBoardWidth + chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
					attron(COLOR_PAIR(2));
					printw("easy");
					attroff(COLOR_PAIR(2));
					break;

				case '2':
					chosenDifficultyBoardHeight = mediumBoardHeight;
					chosenDifficultyBoardWidth = mediumBoardWidth;
					chosenDifficultyBoardMines = mediumBoardMines;
					mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyBoardWidth + chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
					attron(COLOR_PAIR(1));
					printw("medium");
					attroff(COLOR_PAIR(1));
					break;
				
				case '3':
					chosenDifficultyBoardHeight = hardBoardHeight;
					chosenDifficultyBoardWidth = hardBoardWidth;
					chosenDifficultyBoardMines = hardBoardMines;
					mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyBoardWidth + chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
					attron(COLOR_PAIR(3));
					printw("hard");
					attroff(COLOR_PAIR(3));
					break;

				case '4':
					chosenDifficultyBoardHeight = extremeBoardHeight;
					chosenDifficultyBoardWidth = extremeBoardWidth;
					chosenDifficultyBoardMines = extremeBoardMines;
					mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyBoardWidth + chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
					attron(COLOR_PAIR(6));
					printw("extreme");
					attroff(COLOR_PAIR(6));
					break;

				default:
					chosenDifficultyBoardHeight = easyBoardHeight;
					chosenDifficultyBoardWidth = easyBoardWidth;
					chosenDifficultyBoardMines = easyBoardMines;
					mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyBoardWidth + chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
					attron(COLOR_PAIR(2));
					printw("easy");
					attroff(COLOR_PAIR(2));
					break;
			}
			
			mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[7].c_str());
			Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
			startMenu = false;
			gameplayMenu = true;
		} else if(pauseMenu)
		{

		} else if(gameplayMenu)
		{
			drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);

			input = getch();
			if(input == KEY_MOUSE && getmouse(&mouseEvent) == OK)
			{
				clickedY = mouseEvent.y - boardTopPadding;
				clickedX = mouseEvent.x - boardLeftPadding;

				if(clickedY < 0 || clickedY >= board.size() || clickedX < 0 || clickedX >= board[0].size())
				{
					continue;
				}

				if(mouseEvent.bstate &BUTTON1_PRESSED)
				{
					mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[0].c_str());

					bool mouseLeftButtonDownToFlagFailed = true;

					if(allowLeftButtonDownToFlag)
					{
						halfdelay(mouseLeftButtonDownToFlagThresholdInTenthsOfSeconds);

						int tempInput = getch();
						if(tempInput == ERR)
						{
							mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[1].c_str());

							Mines::flagBoardWhereClicked(board, clickedY, clickedX, boardCharSet);
							mouseLeftButtonDownToFlagFailed = false;
						}

						raw(); //turn halfdelay off
					}

					if(mouseLeftButtonDownToFlagFailed)
					{
						mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[2].c_str());

						if(haveNotInitializedMinesYet)
						{
							Mines::initializeMines(board, clickedY, clickedX, chosenDifficultyBoardMines, boardCharSet);
							Mines::initializeNumbers(board, boardCharSet);
							haveNotInitializedMinesYet = false;
						}

						if(board[clickedY][clickedX].displayChar == board[clickedY][clickedX].actualChar
						&& board[clickedY][clickedX].actualChar >= '0' && board[clickedY][clickedX].actualChar <= '9')
						{
							if(Mines::numberOfFlagsAroundNumberCellMatch(board, clickedY, clickedX, boardCharSet))
							{
								if(!Mines::clearBoardWhereClickedAroundNumberCell(board, clickedY, clickedX, boardCharSet))
								{
									gameOver = true;
								}
							}
						} else
						{
							if(!Mines::clearBoardWhereClicked(board, clickedY, clickedX, boardCharSet))
							{
								gameOver = true;
							}
						}

						if(gameOver)
						{
							//hit a mine, so failed game logic goes here Thursday, January 30, 2025, 00:30:26
							mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[5].c_str());
							drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
							mvprintw(gameOverInformationTopPadding + 0, chosenDifficultyBoardWidth + gameOverInformationLeftPadding, "+----------+");
							mvprintw(gameOverInformationTopPadding + 1, chosenDifficultyBoardWidth + gameOverInformationLeftPadding, "|game over!|");
							mvprintw(gameOverInformationTopPadding + 2, chosenDifficultyBoardWidth + gameOverInformationLeftPadding, "+----------+");
							getch();
							break;
						}
					}

				}

				if(mouseEvent.bstate &BUTTON3_RELEASED)
				{
					mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[4].c_str());
				}

				if(mouseEvent.bstate &BUTTON3_PRESSED)
				{
					mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[3].c_str());

					Mines::flagBoardWhereClicked(board, clickedY, clickedX, boardCharSet);
				}
			} else
			{
				mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[6].c_str());
			}

			if(input == '`' || input == 'q')
			{
				break;
			}

			//temp debug controls as of Friday, January 31, 2025, 10:55:51
			if(input == 'r')
			{
				Mines::revealWholeBoard(board, boardCharSet);
			} else if (input == 'R')
			{
				Mines::revealWholeBoard(board, boardCharSet, false);
			}

			if(!haveNotInitializedMinesYet && !gameOver && Mines::haveFoundAllMines(board, chosenDifficultyBoardMines, boardCharSet))
			{
				mvprintw(smileyFaceInformationTopPadding, chosenDifficultyBoardWidth + smileyFaceInformationLeftPadding, "%s", smileyFaces[5].c_str());
				drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
				mvprintw(gameWonInformationTopPadding + 0, chosenDifficultyBoardWidth + gameWonInformationLeftPadding, "+--------+");
				mvprintw(gameWonInformationTopPadding + 1, chosenDifficultyBoardWidth + gameWonInformationLeftPadding, "|you won!|");
				mvprintw(gameWonInformationTopPadding + 2, chosenDifficultyBoardWidth + gameWonInformationLeftPadding, "+--------+");
				getch();
				break;
			}
		}
	}
	getch();

	endwin();
	return 0;
}
