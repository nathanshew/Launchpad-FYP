#include "Graph.h"

#include <algorithm>

void Graph::addPool(const std::shared_ptr<Pool>& pool) {
    Edge forward{pool->token0()->id(), pool->token1()->id(), pool};
    Edge backward{pool->token1()->id(), pool->token0()->id(), pool};

    adjacency_[forward.fromTokenId].push_back(forward);
    adjacency_[backward.fromTokenId].push_back(backward);
}

const std::vector<Edge>& Graph::getEdgesFrom(const std::string& tokenId) const {
    static const std::vector<Edge> kEmpty;
    auto it = adjacency_.find(tokenId);
    if (it == adjacency_.end()) {
        return kEmpty;
    }
    return it->second;
}

std::vector<std::string> Graph::getAllTokens() const {
    std::vector<std::string> tokens;
    tokens.reserve(adjacency_.size());
    for (const auto& kv : adjacency_) {
        tokens.push_back(kv.first);
    }
    std::sort(tokens.begin(), tokens.end());
    return tokens;
}
