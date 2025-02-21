#include "saveloaddata.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>

void makeSureDatFolderExists()
{
	if(!std::filesystem::exists("dat/"))
	{
		std::filesystem::create_directory(std::filesystem::path("dat/"));
	}
}

static const char saveloadNumberColorsTokenDelim = ',';

bool saveNumberColors(std::string filePath, std::array<Triple, 8> &numberColors)
{
	std::fstream write;
	write.open(filePath, std::fstream::out | std::fstream::trunc);
	if(write.fail())
	{
		write.close();
		return false;
	}

	for(int x = 0; x < numberColors.size(); x++)
	{
		write << numberColors[x].red << ',' << numberColors[x].green << ',' << numberColors[x].blue;
		if(x < numberColors.size() - 1)
		{
			write << '\n';
		}
	}
	write.close();

	return true;
}

bool loadNumberColors(std::string filePath, std::array<Triple, 8> &numberColors)
{
	std::fstream read;
	read.open(filePath, std::fstream::in);
	if(read.fail())
	{
		read.close();
		return false;
	}

	bool didNotHaveToLoadDefaultNumberColors = true;
	const std::array<Triple, 8> defaultNumberColors =
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

	int numberColorsPointer = 0;
	std::string getlinestring;
	while(std::getline(read, getlinestring) && numberColorsPointer < numberColors.size())
	{
		std::stringstream lineEater(getlinestring);
		std::vector<std::string> tokens;
		while(std::getline(lineEater, getlinestring, ','))
		{
			tokens.push_back(getlinestring);
			if(tokens.size() >= 3)
			{
				break;
			}
		}
		while(tokens.size() < 3)
		{
			tokens.push_back("-1");
		}

		try
		{
			numberColors[numberColorsPointer].red = std::stoi(tokens[0]);
			numberColors[numberColorsPointer].green = std::stoi(tokens[1]);
			numberColors[numberColorsPointer].blue = std::stoi(tokens[2]);
		} catch(...)
		{
			numberColors[numberColorsPointer] = defaultNumberColors[numberColorsPointer];
			didNotHaveToLoadDefaultNumberColors = false;
		}

		numberColorsPointer++;
	}
	read.close();

	while(numberColorsPointer < numberColors.size())
	{
		numberColors[numberColorsPointer] = defaultNumberColors[numberColorsPointer];
		numberColorsPointer++;
	}

	return didNotHaveToLoadDefaultNumberColors;
}

void loadDefaultNumberColors(std::array<Triple, 8> &numberColors)
{
	numberColors =
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
}

bool saveScore();
bool getHighScores();
