#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Pool.h"

struct ParseResult {
    std::vector<std::shared_ptr<Pool>> pools;
    std::unordered_map<std::string, std::shared_ptr<Token>> tokens;
};

class JSONParser {
public:
    ParseResult parsePoolsFromFile(const std::string& filePath) const;
};
