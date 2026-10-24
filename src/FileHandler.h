#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>
#include "Game.h"

std::vector<Game> loadGamesFromFile(const std::string& filename);

#endif // FILEHANDLER_H
