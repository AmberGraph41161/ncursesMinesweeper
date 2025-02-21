#ifndef SAVELOADDATA_HPP
#define SAVELOADDATA_HPP

#include <string>
#include <array>

struct Triple
{
	Triple(int r, int g, int b) : red(r), green(g), blue(b) {}

	int red = 0;
	int green = 0;
	int blue = 0;
};

void makeSureDatFolderExists();

bool saveNumberColors(std::string filePath, std::array<Triple, 8> &numberColors);
bool loadNumberColors(std::string filePath, std::array<Triple, 8> &numberColors);
void loadDefaultNumberColors(std::array<Triple, 8> &numberColors);

bool saveScore();
bool getHighScores();

#endif
