#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Pool.h"

/**
 * Edge - Represents a directional swap opportunity in the token graph.
 * 
 * Each edge goes from one token to another via a specific pool.
 * For a pool with tokenA and tokenB, we create TWO edges:
 * - tokenA -> tokenB (via the pool)
 * - tokenB -> tokenA (via the same pool)
 */
struct Edge {
    std::string fromTokenId;
    std::string toTokenId;
    std::shared_ptr<Pool> pool;
};

/**
 * Graph - Adjacency list representation of token swap relationships.
 * 
 * Each node is a token address, and each edge is a swappable pool.
 * Used by CycleDetector to find circular arbitrage paths using DFS.
 */
class Graph {
public:
    // Adds a pool to the graph by creating bidirectional edges
    void addPool(const std::shared_ptr<Pool>& pool);

    // Returns all swap opportunities from a given token
    const std::vector<Edge>& getEdgesFrom(const std::string& tokenId) const;
    
    // Returns all tokens that have at least one pool (for cycle detection iteration)
    std::vector<std::string> getAllTokens() const;

private:
    std::unordered_map<std::string, std::vector<Edge>> adjacency_;
};
