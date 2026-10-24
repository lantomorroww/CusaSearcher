#ifndef GAME_H
#define GAME_H

#include <string>
#include <unordered_map>

struct Game {
    std::string title;
    std::string platform;
    std::unordered_map<std::string, std::string> regionCodes;
    std::unordered_map<std::string, std::string> regionLocalization;
};

#endif // GAME_H
