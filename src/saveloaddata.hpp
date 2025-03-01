#ifndef SAVELOADDATA_HPP
#define SAVELOADDATA_HPP

#include <string>
#include <array>
#include <vector>

struct Triple
{
	Triple(int r, int g, int b) : red(r), green(g), blue(b) {}

	int red = 0;
	int green = 0;
	int blue = 0;
};

struct PlayerScore
{
	PlayerScore(std::string playerName, int difficulty, int height, int width, int mines, double time)
	{
		this->playerName = playerName;
		this->difficulty = difficulty;
		this->height = height;
		this->width = width;
		this->mines = mines;
		this->time = time;
	}

	std::string playerName;
	int difficulty;
	int height;
	int width;
	int mines;
	double time;
};

void makeSureDatFolderExists();

bool saveNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);
bool loadNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);
void loadDefaultNumberColors(std::array<Triple, 8> &numberColors);

bool savePlayerName(const std::string &filePath, const std::string &playerName);
bool loadPlayerName(const std::string &filePath, std::string &playerName, const int playerNameSizeLimit = 40);

bool savePlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores);
bool loadPlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores);
void sortPlayerScores(std::vector<PlayerScore> &playerScores);

#endif
