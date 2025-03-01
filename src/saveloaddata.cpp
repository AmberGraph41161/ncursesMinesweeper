#include "saveloaddata.hpp"

#include <algorithm>
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

static const char SAVELOADDATATOKENDELIM = ',';

bool saveNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors)
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
		write << numberColors[x].red << SAVELOADDATATOKENDELIM << numberColors[x].green << SAVELOADDATATOKENDELIM << numberColors[x].blue;
		if(x < numberColors.size() - 1)
		{
			write << '\n';
		}
	}
	write.close();

	return true;
}

bool loadNumberColors(const std::string &filePath, std::array<Triple, 8> &numberColors)
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
		while(std::getline(lineEater, getlinestring, SAVELOADDATATOKENDELIM))
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

bool playerNameIsOkay(const std::string &playerName)
{
	return (playerName.find(SAVELOADDATATOKENDELIM) != std::string::npos);
}

bool savePlayerName(const std::string &filePath, const std::string &playerName)
{
	std::fstream write;
	write.open(filePath, std::fstream::out | std::fstream::trunc);
	if(write.fail())
	{
		write.close();
		return false;
	}

	write << playerName;
	write.close();

	return true;
}

bool loadPlayerName(const std::string &filePath, std::string &playerName)
{
	std::fstream read;
	read.open(filePath, std::fstream::in);

	if(read.fail())
	{
		read.close();
		return false;
	}

	std::string getlinestring;
	std::getline(read, getlinestring);
	if(!playerNameIsOkay(getlinestring))
	{
		while(getlinestring.find(SAVELOADDATATOKENDELIM) != std::string::npos)
		{
			getlinestring.erase(getlinestring.begin() + getlinestring.find(SAVELOADDATATOKENDELIM));
		}
	}

	playerName = getlinestring;
	return true;
}

bool savePlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores)
{
	std::fstream write;
	write.open(filePath, std::fstream::out | std::fstream::trunc);
	if(write.fail())
	{
		write.close();
		return false;
	}

	for(int x = 0; x < playerScores.size(); x++)
	{
		write << playerScores[x].playerName << SAVELOADDATATOKENDELIM;
		write << playerScores[x].difficulty << SAVELOADDATATOKENDELIM;
		write << playerScores[x].score;

		if(x < playerScores.size() - 1)
		{
			write << '\n';
		}
	}

	write.close();
	return true;
}

bool loadPlayerScores(const std::string &filePath, std::vector<PlayerScore> &playerScores)
{
	std::fstream read;
	read.open(filePath, std::fstream::in);
	if(read.fail())
	{
		read.close();
		return false;
	}

	std::string getlinestring;
	while(std::getline(read, getlinestring))
	{
		std::stringstream lineEater(getlinestring);
		std::vector<std::string> tokens;
		while(std::getline(lineEater, getlinestring, SAVELOADDATATOKENDELIM))
		{
			tokens.push_back(getlinestring);
			if(tokens.size() >= 3)
			{
				break;
			}
		}
		while(tokens.size() < 3)
		{
			tokens.push_back("?");
		}

		try
		{
			playerScores.push_back(PlayerScore(tokens[0], std::stoi(tokens[1]), std::stod(tokens[2])));
		} catch(...)
		{
			continue;
		}
	}
	read.close();

	return true;
}

void sortPlayerScores(std::vector<PlayerScore> &playerScores)
{
	std::sort(playerScores.begin(), playerScores.end(),
	[](const PlayerScore &left, const PlayerScore &right)
	{
		return left.score < right.score;
	});
}
