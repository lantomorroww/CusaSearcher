#include "FileHandler.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<Game> loadGamesFromFile(const std::string& filename) {
    std::vector<Game> games;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open " << filename << std::endl;
        return games;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "JSON error: " << e.what() << std::endl;
        return games;
    }

    if (!data.is_array()) {
        std::cerr << "JSON root is not an array" << std::endl;
        return games;
    }

    for (const auto& item : data) {
        Game game;

        if (item.contains("title") && item["title"].is_string()) {
            game.title = item["title"].get<std::string>();
        } else {
            std::cerr << "Missing 'title' in JSON object, skipping entry." << std::endl;
            continue;
        }

        if (item.contains("platform") && item["platform"].is_string()) {
            game.platform = item["platform"].get<std::string>();
        } else {
            game.platform = "Unknown";
        }

        std::unordered_map<std::string, std::string> tempCodes;
        std::unordered_map<std::string, std::string> tempLoc;
        std::string fallbackCode;

        if (item.contains("regionCodes") && item["regionCodes"].is_object()) {
            for (auto it = item["regionCodes"].begin(); it != item["regionCodes"].end(); ++it) {
                std::string region = it.key();
                if (!it.value().is_string()) continue;

                std::string value = it.value().get<std::string>();
                trim(value);
                
                bool looksLikeCode = (value.length() >= 5
                                      && value != "О" && value != "С"
                                      && value != "---" && value != "C");

                if (looksLikeCode) {
                    
                    std::string code;
                    std::string suffix;
                    size_t pos = value.find('(');
                    if (pos != std::string::npos) {
                        code = value.substr(0, pos);
                        trim(code);
                        size_t endPos = value.find(')', pos);
                        if (endPos != std::string::npos) {
                            suffix = value.substr(pos + 1, endPos - pos - 1);
                            trim(suffix);
                        }
                    } else {
                        code = value;
                    }

                    if (!code.empty()
                        && code != "---"
                        && code != "C" && code != "С"
                        && code.length() >= 5) {
                        tempCodes[region] = code;
                        if (!suffix.empty()) {
                            tempLoc[region] = suffix;
                        }
                        if (fallbackCode.empty()) {
                            fallbackCode = code;
                        }
                    }
                } else {
                    // Это маркер локализации (О, С, ---)
                    if (value == "О") {
                        tempLoc[region] = "full";
                    } else if (value == "С") {
                        tempLoc[region] = "subtitles";
                    } else if (value == "---") {
                        tempLoc[region] = "none";
                    } else {
                        tempLoc[region] = value;
                    }
                }
            }
        }

        if (item.contains("regionLocalization") && item["regionLocalization"].is_object()) {
            for (auto it = item["regionLocalization"].begin(); it != item["regionLocalization"].end(); ++it) {
                std::string region = it.key();
                if (it.value().is_string()) {
                    std::string loc = it.value().get<std::string>();
                    trim(loc);
                    if (!loc.empty()) {
                        tempLoc[region] = loc;
                    }
                }
            }
        }

        if (fallbackCode.empty() && !tempCodes.empty()) {
            fallbackCode = tempCodes.begin()->second;
        }
        for (const auto& [region, loc] : tempLoc) {
            if (tempCodes.find(region) == tempCodes.end() && !fallbackCode.empty()) {
                tempCodes[region] = fallbackCode;
            }
        }

        game.regionCodes = tempCodes;
        game.regionLocalization = tempLoc;
        games.push_back(game);
    }

    std::cout << "Loaded " << games.size() << " games." << std::endl;
    return games;
}
