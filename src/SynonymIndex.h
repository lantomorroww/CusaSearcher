#ifndef SYNONYMINDEX_H
#define SYNONYMINDEX_H

#include <string>
#include <unordered_map>
#include <vector>

class SynonymIndex
{
public:
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    std::string findCodeBySynonym(const std::string& synonym) const;
    void addSynonyms(const std::string& code, const std::vector<std::string>& newSynonyms);
    std::vector<std::string> getAllSynonyms() const;

private:
    std::unordered_map<std::string, std::vector<std::string>> data;
    std::unordered_map<std::string, std::string> reverseIndex;

    void rebuildReverseIndex();
};

#endif 
