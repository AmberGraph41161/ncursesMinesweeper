#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>
#include <array>
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
#define CUSTOM_COLOR_TEN 30

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
	curs_set(0); //0 invisible, 1 terminal-specific normal,2 terminal-specific very visible

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
	init_color(CUSTOM_COLOR_TEN, 300, 300, 300);

	const int backgroundColorWhite = COLOR_WHITE;
	const int backgroundColorGray = CUSTOM_COLOR_TEN;
	const int backgroundColorBlack = COLOR_BLACK;
	const int backgroundColorTransparent = -1; 
	int backgroundColor = backgroundColorGray;

	init_pair(1, CUSTOM_COLOR_ONE, backgroundColor);
	init_pair(2, CUSTOM_COLOR_TWO, backgroundColor);
	init_pair(3, CUSTOM_COLOR_THREE, backgroundColor);
	init_pair(4, CUSTOM_COLOR_FOUR, backgroundColor);
	init_pair(5, CUSTOM_COLOR_FIVE, backgroundColor);
	init_pair(6, CUSTOM_COLOR_SIX, backgroundColor);
	init_pair(7, CUSTOM_COLOR_SEVEN, backgroundColor);
	init_pair(8, CUSTOM_COLOR_EIGHT, backgroundColor);
	init_pair(9, CUSTOM_COLOR_NINE, backgroundColor);

	bool allowLeftButtonDownToFlag = true;
	int mouseLeftButtonDownToFlagThresholdInTenthsOfSeconds = 2;

	const int easyBoardDifficulty = 1;
	const int easyBoardHeight = 9;
	const int easyBoardWidth = 9;
	const int easyBoardMines = 10;
	const int mediumBoardDifficulty = 2;
	const int mediumBoardHeight = 16;
	const int mediumBoardWidth = 16;
	const int mediumBoardMines = 40;
	const int hardBoardDifficulty = 3;
	const int hardBoardHeight = 16;
	const int hardBoardWidth = 30;
	const int hardBoardMines = 99;
	const int extremeBoardDifficulty = 4;
	const int extremeBoardHeight = 24;
	const int extremeBoardWidth = 30;
	const int extremeBoardMines = 160;
	const int customBoardDifficulty = 5;

	int customBoardMenuCursor = 0;
	int customBoardMenuInputBuffer = 0;

	int chosenDifficulty = easyBoardDifficulty;
	int chosenDifficultyBoardHeight = 0;
	int chosenDifficultyBoardWidth = 0;
	int chosenDifficultyBoardMines = 0;

	bool initialSmileyFacesHasBeenPrinted = false;
	std::vector<std::string> smileyFaces =
	{
		":o", //0 left pressed
		":3", //1 left held to flag
		":D", //2 left released
		";p", //3 right pressed to flag
		":p", //4 right released after flag
		"X(", //5 hit a mine
		":?", //6 clicked keyboard not mouse
		":)", //7 default (nothing clicked/pressed)
	};

	int boardLeftPadding = 0;
	int boardTopPadding = 0;
	int smileyFaceInformationTopPadding = 0;
	int smileyFaceInformationLeftPadding = 0;
	int chosenDifficultyInformationTopPadding = 0;
	int chosenDifficultyInformationLeftPadding = 0;
	int gameOverInformationTopPadding = 0;
	int gameOverInformationLeftPadding = 0;
	int gameWonInformationTopPadding = 0;
	int gameWonInformationLeftPadding = 0;

	Mines::BoardCharSet boardCharSet;
	std::vector<std::vector<Mines::BoardCell>> board;
	bool haveNotInitializedMinesYet = true;

	int screenHeight = 0;
	int screenWidth = 0;
	int previousScreenHeight = 0;
	int previousScreenWidth = 0;
	bool gameOver = false;
	bool startMenu = true;
	bool customBoardMenu = false;
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
			attron(COLOR_PAIR(7));
			mvprintw(5, 0, "5. Custom");
			attroff(COLOR_PAIR(7));

			input = getch();
			if(input == 'q' || input == '`')
			{
				break;
			}
			clear();

			if(input == KEY_MOUSE && getmouse(&mouseEvent) == OK && (mouseEvent.bstate &BUTTON1_PRESSED))
			{
				clickedY = mouseEvent.y;
				clickedX = mouseEvent.x;

				input = clickedY + '0';
			}

			switch(input)
			{
				case '1':
					chosenDifficultyBoardHeight = easyBoardHeight;
					chosenDifficultyBoardWidth = easyBoardWidth;
					chosenDifficultyBoardMines = easyBoardMines;
					chosenDifficulty = easyBoardDifficulty;
					break;

				case '2':
					chosenDifficultyBoardHeight = mediumBoardHeight;
					chosenDifficultyBoardWidth = mediumBoardWidth;
					chosenDifficultyBoardMines = mediumBoardMines;
					chosenDifficulty = mediumBoardDifficulty;
					break;
				
				case '3':
					chosenDifficultyBoardHeight = hardBoardHeight;
					chosenDifficultyBoardWidth = hardBoardWidth;
					chosenDifficultyBoardMines = hardBoardMines;
					chosenDifficulty = hardBoardDifficulty;
					break;

				case '4':
					chosenDifficultyBoardHeight = extremeBoardHeight;
					chosenDifficultyBoardWidth = extremeBoardWidth;
					chosenDifficultyBoardMines = extremeBoardMines;
					chosenDifficulty = extremeBoardDifficulty;
					break;

				case '5':
					chosenDifficulty = customBoardDifficulty;
					startMenu = false;
					customBoardMenu = true;
					continue;

				default:
					chosenDifficultyBoardHeight = easyBoardHeight;
					chosenDifficultyBoardWidth = easyBoardWidth;
					chosenDifficultyBoardMines = easyBoardMines;
					chosenDifficulty = easyBoardDifficulty;
					break;
			}

			Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
			startMenu = false;
			gameplayMenu = true;
		} else if(customBoardMenu)
		{
			switch(customBoardMenuCursor)
			{
				case 0:
					chosenDifficultyBoardHeight = customBoardMenuInputBuffer;
					mvprintw(0, 0, "Enter customBoardHeight: %d <   ", chosenDifficultyBoardHeight);
					break;
				
				case 1:
					chosenDifficultyBoardWidth = customBoardMenuInputBuffer;
					mvprintw(0, 0, "Enter customBoardWidth: %d <   ", chosenDifficultyBoardWidth);
					break;
				
				case 2:
					chosenDifficultyBoardMines = customBoardMenuInputBuffer;
					mvprintw(0, 0, "Enter customBoardMines: %d <   ", chosenDifficultyBoardMines);
					break;

				case 3:
					mvprintw(0, 0, "customBoardHeight: %d <        ", chosenDifficultyBoardHeight);
					mvprintw(1, 0, "customBoardWidth: %d <         ", chosenDifficultyBoardWidth);
					mvprintw(2, 0, "customBoardMines: %d <         ", chosenDifficultyBoardMines);
					if(((chosenDifficultyBoardHeight * chosenDifficultyBoardWidth) - 9) >= chosenDifficultyBoardMines)
					{
						mvprintw(3, 0, "Continue? (y, n)");
					} else
					{
						mvprintw(3, 0, "The following:");
						mvprintw(4, 0, "    ((chosenDifficultyBoardHeight * chosenDifficultyBoardWidth) - 9) = %d",
							((chosenDifficultyBoardHeight * chosenDifficultyBoardWidth) - 9));
						mvprintw(5, 0, "Must be equal to or exceed: ");
						mvprintw(6, 0, "    chosenDifficultyBoardMines = %d", chosenDifficultyBoardMines);
						mvprintw(7, 0, "Please re-enter values...");
					}
					break;

				default:
					customBoardMenuCursor > 3 ? customBoardMenuCursor = 0 : customBoardMenuCursor = 3;
					continue;
			}

			input = getch();
			if(input == 'q' || input == '`')
			{
				break;
			}

			if(input >= '0' && input <= '9')
			{
				if((customBoardMenuInputBuffer * 10) + (input - '0') <= 999)
				{
					customBoardMenuInputBuffer *= 10;
					customBoardMenuInputBuffer += input - '0';
				}
			} else if(input == KEY_BACKSPACE && customBoardMenuInputBuffer > 0)
			{
				customBoardMenuInputBuffer /= 10;
			} else if(input == KEY_DOWN || input == KEY_RIGHT)
			{
				customBoardMenuCursor++;
				if(customBoardMenuCursor == 0)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardHeight;
				} else if(customBoardMenuCursor == 1)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardWidth;
				} else if(customBoardMenuCursor == 2)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardMines;
				} else
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardHeight;
				}
				clear();
			} else if(input == KEY_UP || input == KEY_LEFT)
			{
				customBoardMenuCursor--;
				if(customBoardMenuCursor == 0)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardHeight;
				} else if(customBoardMenuCursor == 1)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardWidth;
				} else if(customBoardMenuCursor == 2)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardMines;
				}
				clear();
			} else if(input == 'y')
			{
				if(customBoardMenuCursor == 3
				&& ((chosenDifficultyBoardHeight * chosenDifficultyBoardWidth) - 9) >= chosenDifficultyBoardMines)
				{
					clear();
					customBoardMenu = false;
					gameplayMenu = true;
					Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
				}
			} else if(customBoardMenuCursor == 3)
			{
				customBoardMenuCursor++;
				if(customBoardMenuCursor == 0)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardHeight;
				} else if(customBoardMenuCursor == 1)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardWidth;
				} else if(customBoardMenuCursor == 2)
				{
					customBoardMenuInputBuffer = chosenDifficultyBoardMines;
				}
				clear();
			}
		} else if(pauseMenu)
		{

		} else if(gameplayMenu)
		{
			//center information padding
			getmaxyx(stdscr, screenHeight, screenWidth);
			if(screenHeight != previousScreenHeight || screenWidth != previousScreenWidth)
			{
				clear();
				initialSmileyFacesHasBeenPrinted = false;
			}
			previousScreenHeight = screenHeight;
			previousScreenWidth = screenWidth;

			boardTopPadding = (screenHeight / 2) - (chosenDifficultyBoardHeight / 2);
			boardLeftPadding = (screenWidth / 2) - (chosenDifficultyBoardWidth / 2);
			smileyFaceInformationTopPadding = boardTopPadding - 2;
			smileyFaceInformationLeftPadding = (chosenDifficultyBoardWidth / 2) + boardLeftPadding - 1;
			chosenDifficultyInformationTopPadding = boardTopPadding + 0;
			chosenDifficultyInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;
			gameOverInformationTopPadding = boardTopPadding + 1;
			gameOverInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;
			gameWonInformationTopPadding = boardTopPadding + 1;
			gameWonInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;

			if(!initialSmileyFacesHasBeenPrinted)
			{
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[7].c_str());
				initialSmileyFacesHasBeenPrinted = true;
			}
			drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
			mvprintw(chosenDifficultyInformationTopPadding, chosenDifficultyInformationLeftPadding, "Chosen Difficulty: ");
			switch(chosenDifficulty)
			{
				case easyBoardDifficulty:
					attron(COLOR_PAIR(2));
					printw("easy");
					attroff(COLOR_PAIR(2));
					break;
				case mediumBoardDifficulty:
					attron(COLOR_PAIR(1));
					printw("medium");
					attroff(COLOR_PAIR(1));
					break;
				case hardBoardDifficulty:
					attron(COLOR_PAIR(3));
					printw("hard");
					attroff(COLOR_PAIR(3));
					break;
				case extremeBoardDifficulty:
					attron(COLOR_PAIR(6));
					printw("hard");
					attroff(COLOR_PAIR(6));
					break;
				default:
					attron(COLOR_PAIR(7));
					printw("custom");
					attroff(COLOR_PAIR(7));
					break;
			}

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
					mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[0].c_str());

					bool mouseLeftButtonDownToFlagFailed = true;

					if(allowLeftButtonDownToFlag)
					{
						halfdelay(mouseLeftButtonDownToFlagThresholdInTenthsOfSeconds);

						int tempInput = getch();
						if(tempInput == ERR)
						{
							mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[1].c_str());

							Mines::flagBoardWhereClicked(board, clickedY, clickedX, boardCharSet);
							mouseLeftButtonDownToFlagFailed = false;
						}

						raw(); //turn halfdelay off
					}

					if(mouseLeftButtonDownToFlagFailed)
					{
						mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[2].c_str());

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
							mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[5].c_str());
							drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
							mvprintw(gameOverInformationTopPadding + 0, gameOverInformationLeftPadding, "+============================+");
							mvprintw(gameOverInformationTopPadding + 1, gameOverInformationLeftPadding, "|         game over!         |");
							mvprintw(gameOverInformationTopPadding + 2, gameOverInformationLeftPadding, "| click anywhere to restart. |");
							mvprintw(gameOverInformationTopPadding + 3, gameOverInformationLeftPadding, "+----------------------------+");
							getch();
							break;
						}
					}
				}

				if(mouseEvent.bstate &BUTTON3_RELEASED)
				{
					mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[4].c_str());
				}

				if(mouseEvent.bstate &BUTTON3_PRESSED)
				{
					mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[3].c_str());

					Mines::flagBoardWhereClicked(board, clickedY, clickedX, boardCharSet);
				}
			} else
			{
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[6].c_str());
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
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[5].c_str());
				drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
				mvprintw(gameWonInformationTopPadding + 0, gameWonInformationLeftPadding, "+==============================+");
				mvprintw(gameWonInformationTopPadding + 1, gameWonInformationLeftPadding, "|           you won!           |");
				mvprintw(gameWonInformationTopPadding + 2, gameWonInformationLeftPadding, "| click anywhere to play again |");
				mvprintw(gameWonInformationTopPadding + 3, gameWonInformationLeftPadding, "+------------------------------+");
				getch();
				break;
			}
		}
	}
	//getch();

	endwin();
	return 0;
}
