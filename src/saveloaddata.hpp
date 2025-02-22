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

void makeSureDatFolderExists();

bool saveNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);
bool loadNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors);
void loadDefaultNumberColors(std::array<Triple, 8> &numberColors);

bool playerNameIsOkay(const std::string &playerName);
bool savePlayerName(const std::string &filePath, const std::string &playerName);
bool loadPlayerName(const std::string &filePath, std::string &playerName);

bool savePlayerScores(const std::string &filePath, std::vector<std::pair<std::string, double>> &playerScores);
bool loadPlayerScores(const std::string &filePath, std::vector<std::pair<std::string, double>> &playerScores);
void sortPlayerScores(std::vector<std::pair<std::string, double>> &playerScores);

#endif
