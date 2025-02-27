#include <iostream>
#include <chrono>
#include <vector>
#include <array>

#include <ncurses.h>

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
#define CUSTOM_COLOR_ELEVEN 31

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
			} else if(board[y][x].displayChar != boardCharSet.blankChar)
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

void drawBoardNumbers(WINDOW* window, int boardTopPadding, int boardLeftPadding, int chosenDifficultyBoardHeight, int chosenDifficultyBoardWidth, bool fillWithZeros = false)
{
	const int drawBoardNumberHeightPadding = 1;
	const int drawBoardNumbersWidthPadding = 1;

	//yeah, this is kinda gross lol
	int heightFill = std::to_string(chosenDifficultyBoardHeight).size();

	std::string heightFormatString;
	std::string widthFormatString;
	if(fillWithZeros)
	{
		heightFormatString = "%0" + std::to_string(heightFill) + "d";
		widthFormatString = "%0d";
	} else
	{
		heightFormatString = "%" + std::to_string(heightFill) + "d";
		widthFormatString = "%d";
	}

	for(int y = 0; y < chosenDifficultyBoardHeight; y++)
	{
		mvwprintw(window, boardTopPadding + y, boardLeftPadding - heightFill - drawBoardNumbersWidthPadding, heightFormatString.c_str(), y);
	}

	for(int x = 0; x < chosenDifficultyBoardWidth; x++)
	{
		mvwprintw(window, boardTopPadding + chosenDifficultyBoardHeight + drawBoardNumberHeightPadding, boardLeftPadding + x, widthFormatString.c_str(), x % 10);
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
	init_color(CUSTOM_COLOR_NINE, 738, 738, 738); //non-number and non-blankchar char foreground color
	init_color(CUSTOM_COLOR_TEN, 600, 300, 300); //keyboardCursor foreground color
	init_color(CUSTOM_COLOR_ELEVEN, 300, 300, 300);

	const int backgroundColorWhite = COLOR_WHITE;
	const int backgroundColorGray = CUSTOM_COLOR_ELEVEN;
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

	const std::string numberColorsSaveFilePath = "dat/numberColors.txt";
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
	if(loadNumberColors(numberColorsSaveFilePath, numberColors))
	{
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
	}

	const std::string playerScoresSaveFilePath = "dat/playerScores.txt";
	std::vector<PlayerScore> playerScores;
	loadPlayerScores(playerScoresSaveFilePath, playerScores);

	const std::string playerNameSaveFilePath = "dat/playerName.txt";
	std::string playerName = "player0";
	loadPlayerName(playerNameSaveFilePath, playerName);

	bool allowKeyboardCursorBlink = false;
	bool allowKeyboardCursorWrapAroundBoard = true;
	bool allowRainbowKeyboardCursor = true;
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
	int keyboardCursorTopPadding = 0;
	int keyboardCursorLeftPadding = 0;

	int keyboardCursorJumpValueMultiplierThisFrame = 0;
	int keyboardCursorY = 0;
	int keyboardCursorX = 0;
	int keyboardCursorR = 999;
	int keyboardCursorG = 0;
	int keyboardCursorB = 0;
	const char keyboardCursorChar = '+';
	bool showKeyboardCursor = true;
	
	const double keyboardCursorBlinkFirstThreshold = 0.25;
	const double keyboardCursorBlinkSecondThreshold = keyboardCursorBlinkFirstThreshold * 2;
	std::chrono::time_point<std::chrono::system_clock> keyboardCursorBlinkStart = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::system_clock> keyboardCursorBlinkEnd;
	std::chrono::duration<double> keyboardCursorBlinkTimeElapsed;

	Mines::BoardCharSet boardCharSet;
	std::vector<std::vector<Mines::BoardCell>> board;
	bool haveNotInitializedMinesYet = true;

	std::chrono::time_point<std::chrono::system_clock> gameStartTime = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::system_clock> gameEndTime = gameStartTime;
	std::chrono::duration<double> gameTimeElapsed;

	int screenHeight = 0;
	int screenWidth = 0;
	int previousScreenHeight = 0;
	int previousScreenWidth = 0;
	bool gameOver = false;
	bool gameWon = false;
	bool savedPlayerScore = false;
	bool startMenu = true;
	bool customBoardMenu = false;
	bool changeColorsMenu = false;
	bool gameplayMenu = false;
	bool areYouSureYouWantToQuitMenu = false;
	bool pressedQOnceToQuitCurrentGame = false;
	while(true)
	{
		if(areYouSureYouWantToQuitMenu)
		{
			if(gameplayMenu)
			{
				nodelay(stdscr, FALSE);
			}

			getmaxyx(stdscr, screenHeight, screenWidth);
			clear();
			const std::string areYouSureYouWantToQuitMenuMessage0 = "ARE YOU SURE YOU WANT TO QUIT?";
			const std::string areYouSureYouWantToQuitMenuMessage1 = "press 'q' again to quit";
			const std::string areYouSureYouWantToQuitMenuMessage2 = "press any other key to go back";
			mvprintw((screenHeight / 2) - 1, (screenWidth / 2) - (areYouSureYouWantToQuitMenuMessage0.size() / 2), "%s", areYouSureYouWantToQuitMenuMessage0.c_str());
			mvprintw((screenHeight / 2), (screenWidth / 2) - (areYouSureYouWantToQuitMenuMessage1.size() / 2), "%s", areYouSureYouWantToQuitMenuMessage1.c_str());
			mvprintw((screenHeight / 2) + 1, (screenWidth / 2) - (areYouSureYouWantToQuitMenuMessage2.size() / 2), "%s", areYouSureYouWantToQuitMenuMessage2.c_str());
			input = getch();
			clear();

			if(input == 'q')
			{
				break;
			}

			if(gameplayMenu)
			{
				nodelay(stdscr, TRUE);
			}
			areYouSureYouWantToQuitMenu = false;
			continue;
		} else if(startMenu)
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
			if(input == 'q')
			{
				areYouSureYouWantToQuitMenu = true;
				continue;
			} else if(input == '~')
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
			mvprintw(0, 0, "[press 'q' to go back] ");
			attroff(COLOR_PAIR(6));
			attron(COLOR_PAIR(1));
			printw("[screenHeight: %05d, screenWidth: %05d]", screenHeight, screenWidth);
			attroff(COLOR_PAIR(1));

			mvprintw(1, 0, "press 'h' to modify customBoardHeight");
			mvprintw(2, 0, "press 'w' to modify customBoardWidth");
			mvprintw(3, 0, "press 'm' to modify customBoardMines");

			mvprintw(5, 0, "customBoardHeight: %03d", chosenDifficultyBoardHeight);
				customBoardMenuCursor == 1 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(6, 0, "customBoardWidth: %03d", chosenDifficultyBoardWidth);
				customBoardMenuCursor == 2 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(7, 0, "customBoardMines: %03d", chosenDifficultyBoardMines);
				customBoardMenuCursor == 3 ? printw(" < (enter to confirm)") : printw("                     ");

			if(((chosenDifficultyBoardWidth * chosenDifficultyBoardHeight) - 9) >= chosenDifficultyBoardMines)
			{
				mvprintw(9, 0, "looks good! press 'r' to start!");
				mvprintw(10, 0, "                                                                              ");
			} else
			{
				mvprintw(9, 0, "your math doesn't check out....");
				mvprintw(10, 0, "make sure that (customBoardHeight * customBoardWidth) - 9 >=  customBoardMines");
			}

			input = getch();
			if(input == 'q')
			{
				chosenDifficulty = -1;
				customBoardMenu = false;
				startMenu = true;
				continue;
			} else if(input == '~')
			{
				break;
			}

			if(input == 'r'
			&& ((chosenDifficultyBoardHeight * chosenDifficultyBoardWidth) - 9) >= chosenDifficultyBoardMines)
			{
				clear();
				customBoardMenu = false;
				gameplayMenu = true;
				Mines::initializeBoard(board, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth, boardCharSet);
				gameStartTime = std::chrono::high_resolution_clock::now();
				nodelay(stdscr, TRUE);
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

			attron(COLOR_PAIR(6));
			mvprintw(0, 0, "changeColorsMenu [hit 'q' to go back]");
			attroff(COLOR_PAIR(6));
			mvprintw(1, 0, "press '!' to reveal all mines");
			mvprintw(2, 0, "press '@' to reveal entire board");
			mvprintw(3, 0, "press '#' to heal board");
			mvprintw(4, 0, "press 'D' to reset colors to default");
			mvprintw(5, 0, "press 'S' to save changes to file %s", numberColorsSaveFilePath.c_str());
			mvprintw(6, 0, "press '0'-'8' to select number color to modify");

			mvprintw(8, 0, "selected number color to modify: %c", changeColorsMenuSelectedNumber);
			mvprintw(9, 0, "press 'r' to modify selected number R: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].red);
				changeColorsMenuSelectedColorToModify == 1 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(10, 0, "press 'g' to modify selected number G: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].green);
				changeColorsMenuSelectedColorToModify == 2 ? printw(" < (enter to confirm)") : printw("                     ");
			mvprintw(11, 0, "press 'b' to modify selected number B: %03d value (between 0-999)",
				numberColors[changeColorsMenuSelectedNumber - '0' - 1].blue);
				changeColorsMenuSelectedColorToModify == 3 ? printw(" < (enter to confirm)") : printw("                     ");
			drawBoard(stdscr, changeColorsMenuSampleBoard, 15, 0, boardCharSet);

			input = getch();
			if(input == ERR)
			{
				continue;
			}

			if(input == 'q')
			{
				chosenDifficulty = -1;
				changeColorsMenu = false;
				startMenu = true;
				board.clear();
				continue;
			} else if(input == '~')
			{
				break;
			}

			if(input == 'D')
			{
				loadDefaultNumberColors(numberColors);
				continue;
			}


			if(input == 'S')
			{
				makeSureDatFolderExists();
				if(saveNumberColors(numberColorsSaveFilePath, numberColors))
				{
					mvprintw(13, 0, "saved changes to file \"%s\"!", numberColorsSaveFilePath.c_str());
				}
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
		} else if(gameplayMenu)
		{
			if(!gameOver && !gameWon)
			{
				gameEndTime = std::chrono::high_resolution_clock::now();
				gameTimeElapsed = gameEndTime - gameStartTime;
			}

			if(gameWon && !savedPlayerScore)
			{
				playerScores.push_back(PlayerScore(playerName, chosenDifficulty, gameTimeElapsed.count()));
				sortPlayerScores(playerScores);

				makeSureDatFolderExists();
				savePlayerScores(playerScoresSaveFilePath, playerScores);

				savedPlayerScore = true;
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
			keyboardCursorTopPadding = boardTopPadding + keyboardCursorY;
			keyboardCursorLeftPadding = boardLeftPadding + keyboardCursorX;

			if(!initialSmileyFacesHasBeenPrinted)
			{
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[7].c_str());
				initialSmileyFacesHasBeenPrinted = true;
			}

			drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
			drawBoardNumbers(stdscr, boardTopPadding, boardLeftPadding, chosenDifficultyBoardHeight, chosenDifficultyBoardWidth);

			if(allowRainbowKeyboardCursor && showKeyboardCursor)
			{
				if(keyboardCursorR == 999 && keyboardCursorG < 999 && keyboardCursorB == 0)
				{
					keyboardCursorG++;
				} else if(keyboardCursorG == 999 && keyboardCursorR > 0)
				{
					keyboardCursorR--;
				} else if(keyboardCursorG == 999 && keyboardCursorB < 999)
				{
					keyboardCursorB++;
				} else if(keyboardCursorB == 999 && keyboardCursorG > 0)
				{
					keyboardCursorG--;
				} else if(keyboardCursorB == 999 && keyboardCursorR < 999)
				{
					keyboardCursorR++;
				} else if(keyboardCursorR == 999 && keyboardCursorB > 0)
				{
					keyboardCursorB--;
				}
				init_color(CUSTOM_COLOR_TEN, keyboardCursorR, keyboardCursorG, keyboardCursorB);
				init_pair(10, CUSTOM_COLOR_TEN, backgroundColor);
			}
			attron(COLOR_PAIR(10));
			attron(A_BOLD);
			if(allowKeyboardCursorBlink)
			{
				if(keyboardCursorBlinkTimeElapsed.count() >= keyboardCursorBlinkSecondThreshold)
				{
					keyboardCursorBlinkStart = std::chrono::high_resolution_clock::now();
				} else if(keyboardCursorBlinkTimeElapsed.count() <= keyboardCursorBlinkFirstThreshold)
				{
					mvprintw(keyboardCursorTopPadding, keyboardCursorLeftPadding, "%c", keyboardCursorChar);
				}
				keyboardCursorBlinkEnd = std::chrono::high_resolution_clock::now();
				keyboardCursorBlinkTimeElapsed = keyboardCursorBlinkEnd - keyboardCursorBlinkStart;
			} else if(showKeyboardCursor && !gameOver)
			{
				mvprintw(keyboardCursorTopPadding, keyboardCursorLeftPadding, "%c", keyboardCursorChar);
			}
			attroff(A_BOLD);
			attroff(COLOR_PAIR(10));

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

			if(keyboardCursorJumpValueMultiplierThisFrame)
			{
				mvprintw(0, 0, "%d", keyboardCursorJumpValueMultiplierThisFrame);
			} else
			{
				mvprintw(0, 0, "   ");
			}
			if(pressedQOnceToQuitCurrentGame)
			{
				mvprintw(1, 0, "press 'q' again to quit current game");
			} else
			{
				mvprintw(1, 0, "                                    ");
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
			if(!haveNotInitializedMinesYet && !gameOver && Mines::haveFoundAllMines(board, chosenDifficultyBoardMines, boardCharSet))
			{
				gameWon = true;
				nodelay(stdscr, FALSE);
				mvprintw(smileyFaceInformationTopPadding, smileyFaceInformationLeftPadding, "%s", smileyFaces[8].c_str());
				drawBoard(stdscr, board, boardTopPadding, boardLeftPadding, boardCharSet);
				mvprintw(gameWonInformationTopPadding + 0, gameWonInformationLeftPadding, "+==========================+");
				mvprintw(gameWonInformationTopPadding + 1, gameWonInformationLeftPadding, "|         you won!         |");
				mvprintw(gameWonInformationTopPadding + 2, gameWonInformationLeftPadding, "| press 'r' to play again! |");
				mvprintw(gameWonInformationTopPadding + 3, gameWonInformationLeftPadding, "+--------------------------+");
			}

			input = getch();
			if(input == ERR)
			{
				continue;
			}
			if(input == '~')
			{
				break;
			}

			if(input == 'q')
			{
				if(pressedQOnceToQuitCurrentGame)
				{
					pressedQOnceToQuitCurrentGame = false;
					gameplayMenu = false;
				} else
				{
					pressedQOnceToQuitCurrentGame = true;
				}
			} else
			{
				pressedQOnceToQuitCurrentGame = false;
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

			if((input == KEY_MOUSE && getmouse(&mouseEvent) == OK))
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
									nodelay(stdscr, FALSE);
								}
							}
						} else
						{
							if(!Mines::clearBoardWhereClicked(board, clickedY, clickedX, boardCharSet))
							{
								gameOver = true;
								nodelay(stdscr, FALSE);
							}
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
				if(input >= '0' && input <= '9')
				{
					if(keyboardCursorJumpValueMultiplierThisFrame * 10 < 999)
					{
						keyboardCursorJumpValueMultiplierThisFrame *= 10;
						keyboardCursorJumpValueMultiplierThisFrame += (input - '0');
					}
				} else
				{
					switch(input)
					{
						case ' ':
						{
							if(haveNotInitializedMinesYet)
							{
								Mines::initializeMines(board, keyboardCursorY, keyboardCursorX, chosenDifficultyBoardMines, boardCharSet);
								Mines::initializeNumbers(board, boardCharSet);
								haveNotInitializedMinesYet = false;
							}

							if(board[keyboardCursorY][keyboardCursorX].displayChar == board[keyboardCursorY][keyboardCursorX].actualChar
							&& board[keyboardCursorY][keyboardCursorX].actualChar >= '0' && board[keyboardCursorY][keyboardCursorX].actualChar <= '9')
							{
								if(Mines::numberOfFlagsAroundNumberCellMatch(board, keyboardCursorY, keyboardCursorX, boardCharSet))
								{
									if(!Mines::clearBoardWhereClickedAroundNumberCell(board, keyboardCursorY, keyboardCursorX, boardCharSet))
									{
										gameOver = true;
									}
								}
							} else
							{
								if(!Mines::clearBoardWhereClicked(board, keyboardCursorY, keyboardCursorX, boardCharSet))
								{
									gameOver = true;
								}
							}
							break;
						}

						case 'f':
						{
							Mines::flagBoardWhereClicked(board, keyboardCursorY, keyboardCursorX, boardCharSet);
							break;
						}

						case 's':
						{
							showKeyboardCursor ? showKeyboardCursor = false : showKeyboardCursor = true;
							break;
						}

						case 'b':
						{
							allowKeyboardCursorBlink ? allowKeyboardCursorBlink = false : allowKeyboardCursorBlink = true;
							break;
						}

						case 'h':
						{
							if(keyboardCursorJumpValueMultiplierThisFrame == 0)
							{
								keyboardCursorX--;
							} else
							{
								keyboardCursorX -= keyboardCursorJumpValueMultiplierThisFrame;
								keyboardCursorJumpValueMultiplierThisFrame = 0;
							}
							break;
						}
						case 'j':
						{
							if(keyboardCursorJumpValueMultiplierThisFrame == 0)
							{
								keyboardCursorY++;
							} else
							{
								keyboardCursorY += keyboardCursorJumpValueMultiplierThisFrame;
								keyboardCursorJumpValueMultiplierThisFrame = 0;
							}
							break;
						}
						case 'k':
						{
							if(keyboardCursorJumpValueMultiplierThisFrame == 0)
							{
								keyboardCursorY--;
							} else
							{
								keyboardCursorY -= keyboardCursorJumpValueMultiplierThisFrame;
								keyboardCursorJumpValueMultiplierThisFrame = 0;
							}
							break;
						}
						case 'l':
						{
							if(keyboardCursorJumpValueMultiplierThisFrame == 0)
							{
								keyboardCursorX++;
							} else
							{
								keyboardCursorX += keyboardCursorJumpValueMultiplierThisFrame;
								keyboardCursorJumpValueMultiplierThisFrame = 0;
							}
							break;
						}
					}
				}

				if(keyboardCursorY >= chosenDifficultyBoardHeight)
				{
					if(allowKeyboardCursorWrapAroundBoard)
					{
						keyboardCursorY = keyboardCursorY % chosenDifficultyBoardHeight;
					} else
					{
						keyboardCursorY = chosenDifficultyBoardHeight - 1;
					}
				} else if(keyboardCursorY < 0)
				{
					if(allowKeyboardCursorWrapAroundBoard)
					{
						keyboardCursorY = chosenDifficultyBoardHeight + (keyboardCursorY % chosenDifficultyBoardHeight);
					} else
					{
						keyboardCursorY = 0;
					}
				} else if(keyboardCursorX >= chosenDifficultyBoardWidth)
				{
					if(allowKeyboardCursorWrapAroundBoard)
					{
						keyboardCursorX = keyboardCursorX % chosenDifficultyBoardWidth;
					} else
					{
						keyboardCursorX = chosenDifficultyBoardWidth - 1;
					}
				} else if(keyboardCursorX < 0)
				{
					if(allowKeyboardCursorWrapAroundBoard)
					{
						keyboardCursorX = chosenDifficultyBoardWidth + (keyboardCursorX % chosenDifficultyBoardWidth);
					} else
					{
						keyboardCursorX = 0;
					}
				}
			}
		} else
		{
			//reset game here
			gameOver = false;
			gameWon = false;
			savedPlayerScore = false;
			keyboardCursorJumpValueMultiplierThisFrame = 0;

			startMenu = true;
			haveNotInitializedMinesYet = true;
			initialSmileyFacesHasBeenPrinted = false;

			board.clear();
			nodelay(stdscr, FALSE);
		}
	}

	//save data?
	savePlayerScores(playerScoresSaveFilePath, playerScores);

	endwin();
	return 0;
}
