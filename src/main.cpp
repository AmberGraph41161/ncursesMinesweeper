#include <iostream>
#include <chrono>
#include <cstdlib>
#include <ncurses.h>
#include <vector>
#include <array>

#include "minesweeper.hpp"
#include "saveloaddata.hpp"

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

void drawBoard(WINDOW* window, std::vector<std::vector<Mines::BoardCell>> &board, int boardTopPadding, int boardLeftPadding, Mines::BoardCharSet boardCharSet)
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

	std::array<Triple, 8> numberColors =
	{
		Triple(0, 0, 996),
		Triple(0, 507, 0),
		Triple(992, 0, 0),
		Triple(0, 0, 515),
		Triple(515, 0, 0),
		Triple(0, 507, 515),
		Triple(515, 0, 515),
		Triple(457, 457, 457),
	};
	makeSureDatFolderExists();
	loadNumberColors("dat/numberColors.txt", numberColors);

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
	//int backgroundColor = backgroundColorGray;
	//int backgroundColor = backgroundColorWhite;
	int backgroundColor = backgroundColorBlack;

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
	const int changeColorsDifficulty = 6;

	int customBoardMenuCursor = 1;

	int changeColorsMenuSelectedColorToModify = 0;
	int changeColorsMenuSelectedNumber = '1';
	std::vector<std::vector<Mines::BoardCell>> changeColorsMenuSampleBoard;

	int chosenDifficulty = easyBoardDifficulty;
	int chosenDifficultyBoardHeight = 0;
	int chosenDifficultyBoardWidth = 0;
	int chosenDifficultyBoardMines = 0;

	bool initialSmileyFacesHasBeenPrinted = false;
	std::array<std::string, 9> smileyFaces =
	{
		":o", //0 left pressed
		":3", //1 left held to flag
		":]", //2 left released
		":c", //3 right pressed to flag
		";p", //4 right released after flag
		"X(", //5 hit a mine
		":?", //6 clicked keyboard not mouse
		":)", //7 default (nothing clicked/pressed)
		":D", //8 game won
	};

	int boardLeftPadding = 0;
	int boardTopPadding = 0;
	int smileyFaceInformationTopPadding = 0;
	int smileyFaceInformationLeftPadding = 0;
	int gameTimeElapsedInformationTopPadding = 0;
	int gameTimeElapsedInformationLeftPadding = 0;
	int chosenDifficultyInformationTopPadding = 0;
	int chosenDifficultyInformationLeftPadding = 0;
	int gameOverInformationTopPadding = 0;
	int gameOverInformationLeftPadding = 0;
	int gameWonInformationTopPadding = 0;
	int gameWonInformationLeftPadding = 0;

	Mines::BoardCharSet boardCharSet;
	std::vector<std::vector<Mines::BoardCell>> board;
	bool haveNotInitializedMinesYet = true;

	std::chrono::time_point<std::chrono::system_clock> gameStartTime;
	std::chrono::time_point<std::chrono::system_clock> gameEndTime;
	std::chrono::duration<double> gameTimeElapsed;

	int screenHeight = 0;
	int screenWidth = 0;
	int previousScreenHeight = 0;
	int previousScreenWidth = 0;
	bool gameOver = false;
	bool gameWon = false;
	bool startMenu = true;
	bool customBoardMenu = false;
	bool changeColorsMenu = false;
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
			attron(COLOR_PAIR(5));
			mvprintw(6, 0, "6. changeColorsMenu");
			attroff(COLOR_PAIR(5));

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
					break;

				case '6':
					chosenDifficulty = changeColorsDifficulty;
					break;

				default:
					chosenDifficultyBoardHeight = 0;
					chosenDifficultyBoardWidth = 0;
					chosenDifficultyBoardMines = 0;
					chosenDifficulty = -1;
					break;
			}

			if(chosenDifficulty == customBoardDifficulty)
			{
				startMenu = false;
				customBoardMenu = true;
			} else if(chosenDifficulty == changeColorsDifficulty)
			{
				Mines::initializeBoard(changeColorsMenuSampleBoard, 2, 9, boardCharSet);
				for(int x = 1; x <= 8; x++)
				{
					changeColorsMenuSampleBoard[0][x].actualChar = x + '0';
				}
				for(int x = 0; x < 9; x++)
				{
					changeColorsMenuSampleBoard[1][x].actualChar = boardCharSet.mineChar;
				}
				Mines::revealWholeBoard(changeColorsMenuSampleBoard, boardCharSet, true);
				startMenu = false;
				changeColorsMenu = true;
			} else if(chosenDifficulty != -1)
			{
				Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
				startMenu = false;
				gameplayMenu = true;
				gameStartTime = std::chrono::high_resolution_clock::now();
				nodelay(stdscr, TRUE);
			}
		} else if(customBoardMenu)
		{
			getmaxyx(stdscr, screenHeight, screenWidth);
			attron(COLOR_PAIR(6));
			mvprintw(0, 0, "[press '\\' to go back] ");
			attroff(COLOR_PAIR(6));
			attron(COLOR_PAIR(1));
			printw("[screenHeight: %05d, screenWidth: %05d]", screenHeight, screenWidth);
			attroff(COLOR_PAIR(1));

			mvprintw(1, 0, "press 'h' to modify customBoardHeight");
			mvprintw(2, 0, "press 'w' to modify customBoardWidth");
			mvprintw(3, 0, "press 'm' to modify customBoardMines");

			mvprintw(4, 0, "customBoardHeight: %03d", chosenDifficultyBoardHeight);
				customBoardMenuCursor == 1 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(5, 0, "customBoardWidth: %03d", chosenDifficultyBoardWidth);
				customBoardMenuCursor == 2 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(6, 0, "customBoardMines: %03d", chosenDifficultyBoardMines);
				customBoardMenuCursor == 3 ? printw(" < (enter to confirm)") : printw("                     ");

			input = getch();
			if(input == 'q' || input == '`')
			{
				break;
			}

			if(input == '\\') 
			{
				chosenDifficulty = -1;
				customBoardMenu = false;
				startMenu = true;
				continue;
			}

			if(input == 'h')
			{
				customBoardMenuCursor = 1;
			} else if(input == 'w')
			{
				customBoardMenuCursor = 2;
			} else if(input == 'm')
			{
				customBoardMenuCursor = 3;
			} else if(input == '\n')
			{
				customBoardMenuCursor++;
				if(customBoardMenuCursor > 3)
				{
					customBoardMenuCursor = 0;
				}
			}

			if(customBoardMenuCursor != 0 && input >= '0' && input <= '9')
			{
				if(customBoardMenuCursor == 1)
				{
					if((chosenDifficultyBoardHeight * 10) + (input - '0') <= 999)
					{
						chosenDifficultyBoardHeight *= 10;
						chosenDifficultyBoardHeight += input - '0';
					}
				} else if(customBoardMenuCursor == 2)
				{
					if((chosenDifficultyBoardWidth * 10) + (input - '0') <= 999)
					{
						chosenDifficultyBoardWidth *= 10;
						chosenDifficultyBoardWidth += input - '0';
					}
				} else if(customBoardMenuCursor == 3)
				{
					if((chosenDifficultyBoardMines * 10) + (input - '0') <= 999)
					{
						chosenDifficultyBoardMines *= 10;
						chosenDifficultyBoardMines += input - '0';
					}
				}

			} else if(input == KEY_BACKSPACE)
			{
				if(customBoardMenuCursor == 1)
				{
					if((chosenDifficultyBoardHeight * 10) + (input - '0') > 0)
					{
						chosenDifficultyBoardHeight /= 10;
					}
				} else if(customBoardMenuCursor == 2)
				{
					if((chosenDifficultyBoardWidth * 10) + (input - '0') > 0)
					{
						chosenDifficultyBoardWidth /= 10;
					}
				} else if(customBoardMenuCursor == 3)
				{
					if((chosenDifficultyBoardMines * 10) + (input - '0') > 0)
					{
						chosenDifficultyBoardMines /= 10;
					}
				}
			}
		} else if(changeColorsMenu)
		{
			attron(COLOR_PAIR(6));
			mvprintw(0, 0, "changeColorsMenu [hit '\\' to go back]");
			attroff(COLOR_PAIR(6));
			mvprintw(1, 0, "press '!' to reveal all mines");
			mvprintw(2, 0, "press '@' to reveal entire board");
			mvprintw(3, 0, "press '#' to heal board");
			mvprintw(4, 0, "press '0'-'8' to select number color to modify");

			mvprintw(6, 0, "selected number color to modify: %c", changeColorsMenuSelectedNumber);
			mvprintw(7, 0, "press 'r' to modify selected number R: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].red);
				changeColorsMenuSelectedColorToModify == 1 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(8, 0, "press 'g' to modify selected number G: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].green);
				changeColorsMenuSelectedColorToModify == 2 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(9, 0, "press 'b' to modify selected number B: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue);
				changeColorsMenuSelectedColorToModify == 3 ? printw(" < (enter to confirm)") : printw("                     ");
			drawBoard(stdscr, changeColorsMenuSampleBoard, 11, 0, boardCharSet);

			input = getch();
			if(input == ERR)
			{
				continue;
			}
			if(input == 'q' || input == '`')
			{
				break;
			}

			if(input == '\\')
			{
				makeSureDatFolderExists();
				saveNumberColors("dat/numberColors.txt", numberColors);
				saveNumberColors("dat/testing.txt", numberColors);

				chosenDifficulty = -1;
				changeColorsMenu = false;
				startMenu = true;
				board.clear();
				continue;
			}

			if(input == 'r')
			{
				changeColorsMenuSelectedColorToModify = 1;
			} else if(input == 'g')
			{
				changeColorsMenuSelectedColorToModify = 2;
			} else if(input == 'b')
			{
				changeColorsMenuSelectedColorToModify = 3;
			} else if(input == '\n')
			{
				changeColorsMenuSelectedColorToModify++;
				if(changeColorsMenuSelectedColorToModify > 3)
				{
					changeColorsMenuSelectedColorToModify = 0;
				}
			}

			if(changeColorsMenuSelectedColorToModify == 0 && input >= '1' && input <= '8')
			{
				changeColorsMenuSelectedNumber = input;
			} else if(changeColorsMenuSelectedColorToModify)
			{
				if(input >= '0' && input <= '9')
				{
					if(changeColorsMenuSelectedColorToModify == 1)
					{
						if((numberColors[changeColorsMenuSelectedNumber - '0' - 1].red * 10) + (input - '0') <= 999)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].red *= 10;
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].red += input - '0';
						}
					} else if(changeColorsMenuSelectedColorToModify == 2)
					{
						if((numberColors[changeColorsMenuSelectedNumber - '0' - 1].green * 10) + (input - '0') <= 999)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].green *= 10;
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].green += input - '0';
						}
					} else if(changeColorsMenuSelectedColorToModify == 3)
					{
						if((numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue * 10) + (input - '0') <= 999)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue *= 10;
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue += input - '0';
						}
					}
				} else if(input == KEY_BACKSPACE)
				{
					if(changeColorsMenuSelectedColorToModify == 1)
					{
						if(numberColors[changeColorsMenuSelectedNumber - '0' - 1].red > 0)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].red /= 10;
						}
					} else if(changeColorsMenuSelectedColorToModify == 2)
					{
						if(numberColors[changeColorsMenuSelectedNumber - '0' - 1].green > 0)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].green /= 10;
						}
					} else if(changeColorsMenuSelectedColorToModify == 3)
					{
						if(numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue > 0)
						{
							numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue /= 10;
						}
					}
				}
			}

			init_color(CUSTOM_COLOR_ONE, numberColors[0].red, numberColors[0].green, numberColors[0].blue);
			init_color(CUSTOM_COLOR_TWO, numberColors[1].red, numberColors[1].green, numberColors[1].blue);
			init_color(CUSTOM_COLOR_THREE, numberColors[2].red, numberColors[2].green, numberColors[2].blue);
			init_color(CUSTOM_COLOR_FOUR, numberColors[3].red, numberColors[3].green, numberColors[3].blue);
			init_color(CUSTOM_COLOR_FIVE, numberColors[4].red, numberColors[4].green, numberColors[4].blue);
			init_color(CUSTOM_COLOR_SIX, numberColors[5].red, numberColors[5].green, numberColors[5].blue);
			init_color(CUSTOM_COLOR_SEVEN, numberColors[6].red, numberColors[6].green, numberColors[6].blue);
			init_color(CUSTOM_COLOR_EIGHT, numberColors[7].red, numberColors[7].green, numberColors[7].blue);
			init_pair(1, CUSTOM_COLOR_ONE, backgroundColor);
			init_pair(2, CUSTOM_COLOR_TWO, backgroundColor);
			init_pair(3, CUSTOM_COLOR_THREE, backgroundColor);
			init_pair(4, CUSTOM_COLOR_FOUR, backgroundColor);
			init_pair(5, CUSTOM_COLOR_FIVE, backgroundColor);
			init_pair(6, CUSTOM_COLOR_SIX, backgroundColor);
			init_pair(7, CUSTOM_COLOR_SEVEN, backgroundColor);
			init_pair(8, CUSTOM_COLOR_EIGHT, backgroundColor);

			if(input == 'z')
			{
				int backgroundColor = backgroundColorWhite;

				init_pair(1, CUSTOM_COLOR_ONE, backgroundColor);
				init_pair(2, CUSTOM_COLOR_TWO, backgroundColor);
				init_pair(3, CUSTOM_COLOR_THREE, backgroundColor);
				init_pair(4, CUSTOM_COLOR_FOUR, backgroundColor);
				init_pair(5, CUSTOM_COLOR_FIVE, backgroundColor);
				init_pair(6, CUSTOM_COLOR_SIX, backgroundColor);
				init_pair(7, CUSTOM_COLOR_SEVEN, backgroundColor);
				init_pair(8, CUSTOM_COLOR_EIGHT, backgroundColor);
				init_pair(9, CUSTOM_COLOR_NINE, backgroundColor);
				continue;
			} else if(input == 'x')
			{
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
				continue;
			} else if(input == 'c')
			{
				int backgroundColor = backgroundColorBlack;

				init_pair(1, CUSTOM_COLOR_ONE, backgroundColor);
				init_pair(2, CUSTOM_COLOR_TWO, backgroundColor);
				init_pair(3, CUSTOM_COLOR_THREE, backgroundColor);
				init_pair(4, CUSTOM_COLOR_FOUR, backgroundColor);
				init_pair(5, CUSTOM_COLOR_FIVE, backgroundColor);
				init_pair(6, CUSTOM_COLOR_SIX, backgroundColor);
				init_pair(7, CUSTOM_COLOR_SEVEN, backgroundColor);
				init_pair(8, CUSTOM_COLOR_EIGHT, backgroundColor);
				init_pair(9, CUSTOM_COLOR_NINE, backgroundColor);
				continue;
			}

			if(input == '!')
			{
				//Mines::revealWholeBoard(board, boardCharSet);
				Mines::revealOnlyMines(changeColorsMenuSampleBoard, boardCharSet);
			} else if(input == '@')
			{
				Mines::revealWholeBoard(changeColorsMenuSampleBoard, boardCharSet, false);
			} else if(input == '#')
			{
				Mines::healBoard(changeColorsMenuSampleBoard, boardCharSet);
			}
		} else if(pauseMenu)
		{

		} else if(gameplayMenu)
		{
			if(!gameOver && !gameWon)
			{
				gameEndTime = std::chrono::high_resolution_clock::now();
				gameTimeElapsed = gameEndTime - gameStartTime;
			}

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
			gameTimeElapsedInformationTopPadding = boardTopPadding + 0;
			gameTimeElapsedInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;
			chosenDifficultyInformationTopPadding = boardTopPadding + 1;
			chosenDifficultyInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;
			gameOverInformationTopPadding = boardTopPadding + 2;
			gameOverInformationLeftPadding = chosenDifficultyBoardWidth + boardLeftPadding + 1;
			gameWonInformationTopPadding = boardTopPadding + 2;
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
			mvprintw(gameTimeElapsedInformationTopPadding, gameTimeElapsedInformationLeftPadding, "Elapsed Time: %f", gameTimeElapsed.count());

			input = getch();
			if(input == ERR)
			{
				continue;
			}

			if(input == 'q' || input == '`')
			{
				break;
			}

			if(gameOver || gameWon)
			{
				if(input == 'r')
				{
					gameplayMenu = false;
				} else if(input == '1')
				{
					//Mines::revealWholeBoard(board, boardCharSet);
					Mines::revealOnlyMines(board, boardCharSet);
				} else if(input == '2')
				{
					Mines::revealWholeBoard(board, boardCharSet, false);
				}
				continue;
			}

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
							mvprintw(gameOverInformationTopPadding + 0, gameOverInformationLeftPadding, "+=================================+");
							mvprintw(gameOverInformationTopPadding + 1, gameOverInformationLeftPadding, "|            game over!            |");
							mvprintw(gameOverInformationTopPadding + 2, gameOverInformationLeftPadding, "| press 'r' to restart             |");
							mvprintw(gameOverInformationTopPadding + 3, gameOverInformationLeftPadding, "| press '1' to reveal all mines    |");
							mvprintw(gameOverInformationTopPadding + 4, gameOverInformationLeftPadding, "| press '2' to reveal entire board |");
							mvprintw(gameOverInformationTopPadding + 5, gameOverInformationLeftPadding, "+---------------------------------+");
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

			if(!haveNotInitializedMinesYet && !gameOver && Mines::haveFoundAllMines(board, chosenDifficultyBoardMines, boardCharSet))
			{
				gameWon = true;
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[8].c_str());
				drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
				mvprintw(gameWonInformationTopPadding + 0, gameWonInformationLeftPadding, "+==========================+");
				mvprintw(gameWonInformationTopPadding + 1, gameWonInformationLeftPadding, "|         you won!         |");
				mvprintw(gameWonInformationTopPadding + 2, gameWonInformationLeftPadding, "| press 'r' to play again! |");
				mvprintw(gameWonInformationTopPadding + 3, gameWonInformationLeftPadding, "+--------------------------+");
			}
		} else
		{
			//reset game here
			gameOver = false;
			gameWon = false;

			startMenu = true;
			haveNotInitializedMinesYet = true;
			initialSmileyFacesHasBeenPrinted = false;
			
			//customBoardMenu = false;
			//pauseMenu = false;
			//gameplayMenu = false;

			board.clear();
			nodelay(stdscr, FALSE);
			raw(); //calling this too just in case...? might be unnecessary
		}
	}
	//getch();

	endwin();
	return 0;
}
