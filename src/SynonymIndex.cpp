#include "SynonymIndex.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool SynonymIndex::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open synonym file: " << filename << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        for (auto& [code, synonyms] : j.items()) {
            std::vector<std::string> synList;
            for (auto& s : synonyms) {
                std::string synonym = s.get<std::string>();
                trim(synonym);
                synonym = toLower(synonym);
                if (!synonym.empty()) synList.push_back(synonym);
            }
            data[code] = synList;
        }
        rebuildReverseIndex();
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void SynonymIndex::rebuildReverseIndex()
{
    reverseIndex.clear();
    for (const auto& [code, synonyms] : data) {
        for (const auto& syn : synonyms) {
            reverseIndex[syn] = code;
        }
    }
}

std::string SynonymIndex::findCodeBySynonym(const std::string& synonym) const
{
    std::string key = synonym;
    trim(key);
    key = toLower(key);
    auto it = reverseIndex.find(key);
    if (it != reverseIndex.end()) return it->second;
    return "";
}

void SynonymIndex::addSynonyms(const std::string& code, const std::vector<std::string>& newSynonyms)
{
    auto& synList = data[code];
    for (auto s : newSynonyms) {
        trim(s);
        s = toLower(s);
        if (!s.empty() && std::find(synList.begin(), synList.end(), s) == synList.end()) {
            synList.push_back(s);
            reverseIndex[s] = code;
        }
    }
}

std::vector<std::string> SynonymIndex::getAllSynonyms() const
{
    std::vector<std::string> result;
    for (const auto& [_, synonyms] : data) {
        result.insert(result.end(), synonyms.begin(), synonyms.end());
    }
    return result;
}

bool SynonymIndex::saveToFile(const std::string& filename) const
{
    json j;
    for (const auto& [code, synonyms] : data) {
        j[code] = synonyms;
    }
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << j.dump(4);
    return true;
}