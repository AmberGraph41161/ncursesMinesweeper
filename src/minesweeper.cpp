#include "minesweeper.hpp"

#include <cstdlib>
#include <vector>

namespace Mines
{
	int RANDOM(int minimum, int maximum)
	{
		return (rand() % (maximum - minimum + 1)) + minimum;
	}

	void initializeBoard(std::vector<std::vector<BoardCell>> &board, int boardHeight, int boardWidth, BoardCharSet &boardCharSet)
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
			tempRow.clear();
		}
	}
	
	void initializeMines(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, int boardMines, BoardCharSet &boardCharSet, bool keepThreeByThreeSafeSpaceAroundCursor)
	{
		for(int x = 0; x < boardMines; x++)
		{
			int randomSpotY = RANDOM(0, board.size() - 1);
			int randomSpotX = RANDOM(0, board[0].size() - 1);
			
			if(keepThreeByThreeSafeSpaceAroundCursor)
			{
				if((randomSpotY >= clickedY - 1 && randomSpotY <= clickedY + 1
				&& randomSpotX >= clickedX - 1 && randomSpotX <= clickedX + 1)
				|| (board[randomSpotY][randomSpotX].actualChar ==  boardCharSet.mineChar)
				)
				{
					x--;
				} else
				{
					board[randomSpotY][randomSpotX].actualChar = boardCharSet.mineChar;
				}
			} else
			{
				if((randomSpotY == clickedY && randomSpotX == clickedX)
				|| (board[randomSpotY][randomSpotX].actualChar == boardCharSet.mineChar)
				)
				{
					x--;
				} else
				{
					board[randomSpotY][randomSpotX].actualChar = boardCharSet.mineChar;
				}
			}
		}
	}

	void initializeNumbers(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet)
	{
		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				int tempMineCount = 0;

				if(board[y][x].actualChar != boardCharSet.mineChar)
				{
					for(int aroundY = -1; aroundY <= 1; aroundY++)
					{
						for(int aroundX = -1; aroundX <= 1; aroundX++)
						{
							if(y + aroundY >= 0 && y + aroundY < board.size()
							&& x + aroundX >= 0 && x + aroundX < board[0].size())
							{
								if(board[y + aroundY][x + aroundX].actualChar == boardCharSet.mineChar)
								{
									tempMineCount++;
								}
							}
						}
					}

					if(tempMineCount > 0)
					{
						board[y][x].actualChar = tempMineCount + '0';
					}
				}
			}
		}
	}

	bool clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet)
	{
		//https://en.wikipedia.org/wiki/Flood_fill
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return true;
		}
		
		if(board[clickedY][clickedX].displayChar == board[clickedY][clickedX].actualChar)
		{
			return true;
		}

		if(board[clickedY][clickedX].displayChar != boardCharSet.flagChar)
		{
			board[clickedY][clickedX].displayChar = board[clickedY][clickedX].actualChar;

			if(board[clickedY][clickedX].actualChar == boardCharSet.mineChar)
			{
				return false;
			}
		}

		if(board[clickedY][clickedX].displayChar != boardCharSet.flagChar
		&& (board[clickedY][clickedX].actualChar < '0' || board[clickedY][clickedX].actualChar > '9'))
		{
			clearBoardWhereClicked(board, clickedY + 1, clickedX, boardCharSet); //up
			clearBoardWhereClicked(board, clickedY - 1, clickedX, boardCharSet); //down
			clearBoardWhereClicked(board, clickedY, clickedX + 1, boardCharSet); //left
			clearBoardWhereClicked(board, clickedY, clickedX - 1, boardCharSet); //right
			clearBoardWhereClicked(board, clickedY + 1, clickedX - 1, boardCharSet); //up left
			clearBoardWhereClicked(board, clickedY + 1, clickedX + 1, boardCharSet); //up right
			clearBoardWhereClicked(board, clickedY - 1, clickedX - 1, boardCharSet); //down left
			clearBoardWhereClicked(board, clickedY - 1, clickedX + 1, boardCharSet); //down right
		}

		return true;
	}

	void flagBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet)
	{
	
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return;
		}
		
		if(board[clickedY][clickedX].displayChar == boardCharSet.filledChar)
		{
			board[clickedY][clickedX].displayChar = boardCharSet.flagChar;
		} else if(board[clickedY][clickedX].displayChar == boardCharSet.flagChar)
		{
			board[clickedY][clickedX].displayChar = boardCharSet.filledChar;
		} else
		{
			return;
		}
	}

	bool numberOfFlagsAroundNumberCellMatch(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet)
	{
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return false;
		}

		if(!(board[clickedY][clickedX].actualChar >= '0' && board[clickedY][clickedX].actualChar <= '9'))
		{
			return false;
		}

		int numberOfFlagsAroundCell = 0;

		for(int y = -1; y <= 1; y++)
		{
			for(int x = -1; x <= 1; x++)
			{
				if(clickedY + y < board.size() && clickedY + y >= 0
				&& clickedX + x < board[0].size() && clickedX + x >= 0)
				{
					if(board[clickedY + y][clickedX + x].displayChar == boardCharSet.flagChar)
					{
						numberOfFlagsAroundCell++;
					}
				}
			}
		}

		return (board[clickedY][clickedX].actualChar - '0' == numberOfFlagsAroundCell);
	}

	bool clearBoardWhereClickedAroundNumberCell(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet)
	{
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return true;
		}

		if(board[clickedY][clickedX].actualChar < '0' || board[clickedY][clickedX].actualChar > '9')
		{
			return true;
		}

		bool hitMine = false;

		for(int y = -1; y <= 1; y++)
		{
			for(int x = -1; x <= 1; x++)
			{
				if(clickedY + y < board.size() && clickedY + y >= 0
				&& clickedX + x < board[0].size() && clickedX + x >= 0)
				{
					if(board[clickedY + y][clickedX + x].actualChar != boardCharSet.mineChar
					&& board[clickedY + y][clickedX + x].displayChar == boardCharSet.flagChar)
					{
						board[clickedY + y][clickedX + x].displayChar = boardCharSet.badFlagChar;
					}

					if(board[clickedY + y][clickedX + x].displayChar == boardCharSet.filledChar)
					{
						if(board[clickedY + y][clickedX + x].actualChar == boardCharSet.mineChar)
						{
							hitMine = true;
						}
						clearBoardWhereClicked(board, clickedY + y, clickedX + x, boardCharSet);
					}

				}
			}
		}

		return !hitMine;
	}

	void revealWholeBoard(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet, bool revealMines)
	{
		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				if(board[y][x].displayChar == boardCharSet.flagChar && board[y][x].actualChar != boardCharSet.mineChar)
				{
					board[y][x].displayChar = boardCharSet.badFlagChar;
					continue;
				}

				if(board[y][x].actualChar == boardCharSet.mineChar && !revealMines)
				{
					continue;
				}

				board[y][x].displayChar = board[y][x].actualChar;
			}
		}
	}

	void revealOnlyMines(std::vector<std::vector<BoardCell>> &board, BoardCharSet &boardCharSet)
	{
		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				if(board[y][x].actualChar == boardCharSet.mineChar)
				{
					board[y][x].displayChar = board[y][x].actualChar;
				}
			}
		}
	}

	bool haveFoundAllMines(std::vector<std::vector<BoardCell>> &board, int chosenDifficultyBoardMines, BoardCharSet &boardCharSet)
	{
		int realFlagCount = 0;
		int fakeFlagCount = 0;
		int realFilledCharCount = 0;
		int fakeFilledCharCount = 0;

		for(int y = 0; y < board.size(); y++)
		{
			for(int x = 0; x < board[y].size(); x++)
			{
				if(board[y][x].displayChar == boardCharSet.flagChar)
				{
					if(board[y][x].actualChar == boardCharSet.mineChar)
					{
						realFlagCount++;
					} else
					{
						fakeFlagCount++;
					}
				} else if(board[y][x].displayChar == boardCharSet.filledChar)
				{
					if(board[y][x].actualChar == boardCharSet.mineChar)
					{
						realFilledCharCount++;
					} else
					{
						fakeFilledCharCount++;
					}
				}
			}
		}

		if(realFlagCount == chosenDifficultyBoardMines || realFilledCharCount == chosenDifficultyBoardMines)
		{
			if(fakeFlagCount == 0 && fakeFilledCharCount == 0)
			{
				return true;
			}
		}

		return false;
	}
}
