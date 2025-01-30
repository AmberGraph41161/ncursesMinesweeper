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
		}
	}
	
	void initializeMines(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, int boardMines, BoardCharSet &boardCharSet)
	{
		for(int x = 0; x < boardMines; x++)
		{
			int randomSpotY = RANDOM(0, board.size() - 1);
			int randomSpotX = RANDOM(0, board[0].size() - 1); 

			if(randomSpotY <= clickedY + 1 && randomSpotY >= clickedY - 1
			&& randomSpotX <= clickedX + 1 && randomSpotX >= clickedX - 1
			)
			{
				x--;
			} else
			{
				board[randomSpotY][randomSpotX].actualChar = boardCharSet.mineChar;
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
					board[y][x].actualChar = tempMineCount + '0';
				}
			}
		}
	}

	void clearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet, bool revealWholeBoard)
	{
		//https://en.wikipedia.org/wiki/Flood_fill
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return;
		}
		
		if(board[clickedY][clickedX].displayChar == board[clickedY][clickedX].actualChar)
		{
			return;
		}

		if(board[clickedY][clickedX].actualChar != boardCharSet.mineChar && board[clickedY][clickedX].displayChar != boardCharSet.flagChar)
		{
			board[clickedY][clickedX].displayChar = board[clickedY][clickedX].actualChar;
		} else
		{
			return;
		}

		if(!(board[clickedY][clickedX].actualChar >= '0' && board[clickedY][clickedX].actualChar <= '9') || revealWholeBoard)
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
	}

	void clearBoardWhereClickedDEPRECATED(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet boardCharSet)
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

		//!##
		//#@#
		//###
		if(clickedY - 1 >= 0 && clickedX - 1 >= 0)
		{
			if(board[clickedY - 1][clickedX - 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//#!#
		//#@#
		//###
		if(clickedY - 1 >= 0)
		{
			if(board[clickedY - 1][clickedX].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//##!
		//#@#
		//###
		if(clickedY - 1 >= 0 && clickedX + 1 < board[0].size())
		{
			if(board[clickedY - 1][clickedX + 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//###
		//!@#
		//###
		if(clickedX - 1 >= 0) 
		{
			if(board[clickedY][clickedX - 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//###
		//#@!
		//###
		if(clickedX + 1 < board[0].size()) 
		{
			if(board[clickedY][clickedX + 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//###
		//#@#
		//!##
		if(clickedY + 1 < board.size() && clickedX - 1 >= 0)
		{
			if(board[clickedY + 1][clickedX - 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//###
		//#@#
		//#!#
		if(clickedY + 1 < board.size())
		{
			if(board[clickedY + 1][clickedX].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}
		//###
		//#@#
		//##!
		if(clickedY + 1 < board.size() && clickedX + 1 < board[0].size())
		{
			if(board[clickedY + 1][clickedX + 1].displayChar == boardCharSet.flagChar)
			{
				numberOfFlagsAroundCell++;
			}
		}

		return (board[clickedY][clickedX].actualChar - '0' == numberOfFlagsAroundCell);
	}

	bool numberOfFlagsClearBoardWhereClicked(std::vector<std::vector<BoardCell>> &board, int clickedY, int clickedX, BoardCharSet &boardCharSet)
	{
		if(clickedY >= board.size() || clickedY < 0 || clickedX >= board[0].size() || clickedX < 0)
		{
			return false;
		}

		if(!(board[clickedY][clickedX].actualChar >= '0' && board[clickedY][clickedX].actualChar <= '9'))
		{
			return false;
		}

		bool hitMine = false;

		//!##
		//#@#
		//###
		if(clickedY - 1 >= 0 && clickedX - 1 >= 0)
		{
			if(board[clickedY - 1][clickedX - 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY - 1][clickedX - 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY - 1][clickedX - 1].displayChar = board[clickedY - 1][clickedX - 1].actualChar;
			}
		}
		//#!#
		//#@#
		//###
		if(clickedY - 1 >= 0)
		{
			if(board[clickedY - 1][clickedX].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY - 1][clickedX].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY - 1][clickedX].displayChar = board[clickedY - 1][clickedX].actualChar;
			}
		}
		//##!
		//#@#
		//###
		if(clickedY - 1 >= 0 && clickedX + 1 < board[0].size())
		{
			if(board[clickedY - 1][clickedX + 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY - 1][clickedX + 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY - 1][clickedX + 1].displayChar = board[clickedY - 1][clickedX + 1].actualChar;
			}
		}
		//###
		//!@#
		//###
		if(clickedX - 1 >= 0) 
		{
			if(board[clickedY][clickedX - 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY][clickedX - 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY][clickedX - 1].displayChar = board[clickedY][clickedX - 1].actualChar;
			}
		}
		//###
		//#@!
		//###
		if(clickedX + 1 < board[0].size()) 
		{
			if(board[clickedY][clickedX + 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY][clickedX + 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY][clickedX + 1].displayChar = board[clickedY][clickedX + 1].actualChar;
			}
		}
		//###
		//#@#
		//!##
		if(clickedY + 1 < board.size() && clickedX - 1 >= 0)
		{
			if(board[clickedY + 1][clickedX - 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY + 1][clickedX - 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY + 1][clickedX - 1].displayChar = board[clickedY + 1][clickedX - 1].actualChar;
			}
		}
		//###
		//#@#
		//#!#
		if(clickedY + 1 < board.size())
		{
			if(board[clickedY + 1][clickedX].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY + 1][clickedX].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY + 1][clickedX].displayChar = board[clickedY + 1][clickedX].actualChar;
			}
		}
		//###
		//#@#
		//##!
		if(clickedY + 1 < board.size() && clickedX + 1 < board[0].size())
		{
			if(board[clickedY + 1][clickedX + 1].displayChar != boardCharSet.flagChar)
			{
				if(board[clickedY + 1][clickedX + 1].actualChar == boardCharSet.mineChar)
				{
					hitMine = true;
				}
				board[clickedY + 1][clickedX + 1].displayChar = board[clickedY + 1][clickedX + 1].actualChar;
			}
		}

		return !hitMine;
	}

}
