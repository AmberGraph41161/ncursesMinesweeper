/** @file */

#ifndef SAVELOADDATA_HPP
#define SAVELOADDATA_HPP

#include <string>
#include <array>
#include <vector>

/** @brief badly named color struct that contains three integer values, r, g, and b */
struct Triple
{
	/**
	 * @brief constructs Triple
	 *
	 * @param r initialization red value
	 * @param g initialization green value
	 * @param b initialization blue value
	 */
	Triple(int r, int g, int b) : red(r), green(g), blue(b) {}

	int red = 0; ///< red value of Triple
	int green = 0; ///< green value of Triple
	int blue = 0; ///< blue value of Triple
};

/** @brief player score information about a single game that was played and won */
struct PlayerScore
{
	/**
	 * @brief constructs a PlayerScore
	 *
	 * @param playerName name of player
	 * @param difficulty difficulty rating of game
	 * @param height height of board
	 * @param width width of board
	 * @param mines number of mines on board
	 * @param date arbitrary string representing date game was played
	 * @param time elapsed time when game won
	 */
	PlayerScore(std::string playerName, int difficulty, int height, int width, int mines, std::string date, double time)
	{
		this->playerName = playerName;
		this->difficulty = difficulty;
		this->height = height;
		this->width = width;
		this->mines = mines;
		this->date = date;
		this->time = time;
	}

	std::string playerName; ///< playerName name of player
	int difficulty; ///< difficulty difficulty rating of game
	int height; ///< height height of board
	int width; ///< width width of board
	int mines; ///< mines number of mines on board
	std::string date; ///< date arbitrary string representing date game was played
	double time; ///< time elapsed time when game won
};

/** @brief creates "dat/" directory if does not exist */
void makeSureDatFolderExists();

/**
 * @brief saves provided number color values to file path
 *
 * @param filePath relative file path to the binary in which you would like to save data
 * @param numberColors number colors that you would like to save to file
 * @return false on write error, true on write success
 */
bool saveNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);

/**
 * @brief loads saved number color values from provided file path
 *
 * @param filePath relative file path to the binary in which you would like to load data
 * @param numberColors number colors array that you would like the saved data to be loaded into
 * @return false on read fail of any form, true on success
 */
bool loadNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);

/** @brief stores default hard-coded number color values into provided numberColors variable */
void loadDefaultNumberColors(std::array<Triple, 8> &numberColors);

/**
 * @brief saves provided playerName to file
 *
 * @param filePath relative file path to the binary in which you would like to save the provided player name
 * @param playerName player name you would like to save
 * @return false on write error, true on success
 */
bool savePlayerName(const std::string &filePath, const std::string &playerName);

/**
 * @brief loads saved player name to provided playerName variable
 *
 * @param filePath relative file path to the binary in which you would like to load the player name
 * @param playerName variable to which the data read will be stored into
 * @param playerNameSizeLimit string size limit of what will be stored into playerName
 * @return false on read error of any sort, true on success
 */
bool loadPlayerName(const std::string &filePath, std::string &playerName, const int playerNameSizeLimit = 40);

/**
 * @brief save player scores to file path
 *
 * @param filePath relative file path to the binary in which you would like to save player scores
 * @param playerScores playerScores you would like to save
 * @return false on write error, true on success
 */
bool savePlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores);

/**
 * @brief load player scores from file path
 *
 * @param filePath relative file path to the binary in which you would like to load the player scores
 * @param playerScores variable to which the data read will be stroed into
 * @return false on read error, true on success
 */
bool loadPlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores);

/** @brief sort playerScores from best times on top, to worst times on bottom */
void sortPlayerScores(std::vector<PlayerScore> &playerScores);

#endif
