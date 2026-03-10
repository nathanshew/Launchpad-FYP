#include "Graph.h"

#include <algorithm>
#include <stdexcept>

void Graph::addPool(const std::shared_ptr<Pool>& pool) {
    if (!pool) {
        throw std::invalid_argument("Cannot add null pool to graph");
    }
    if (!pool->token0() || !pool->token1()) {
        throw std::invalid_argument("Pool in graph must have non-null tokens");
    }

    // Create bidirectional edges: both token0->token1 and token1->token0
    // This allows cycle detection to traverse the graph in both directions
    Edge forward{pool->token0()->id(), pool->token1()->id(), pool};
    Edge backward{pool->token1()->id(), pool->token0()->id(), pool};

    adjacency_[forward.fromTokenId].push_back(forward);
    adjacency_[backward.fromTokenId].push_back(backward);
}

const std::vector<Edge>& Graph::getEdgesFrom(const std::string& tokenId) const {
    // Return empty vector if token has no pools (avoids crashes in DFS)
    static const std::vector<Edge> kEmpty;
    auto it = adjacency_.find(tokenId);
    if (it == adjacency_.end()) {
        return kEmpty;
    }
    return it->second;
}

std::vector<std::string> Graph::getAllTokens() const {
    // Collect all token IDs from the adjacency map keys
    std::vector<std::string> tokens;
    tokens.reserve(adjacency_.size());
    for (const auto& kv : adjacency_) {
        tokens.push_back(kv.first);
    }
    // Sort for deterministic iteration order (helps with reproducible results)
    std::sort(tokens.begin(), tokens.end());
    return tokens;
}
