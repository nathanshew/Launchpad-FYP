#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Pool.h"

struct Edge {
    std::string fromTokenId;
    std::string toTokenId;
    std::shared_ptr<Pool> pool;
};

class Graph {
public:
    void addPool(const std::shared_ptr<Pool>& pool);

    const std::vector<Edge>& getEdgesFrom(const std::string& tokenId) const;
    std::vector<std::string> getAllTokens() const;

private:
    std::unordered_map<std::string, std::vector<Edge>> adjacency_;
};
