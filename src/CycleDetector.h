#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "Config.h"
#include "Graph.h"

/**
 * Cycle - Represents a closed arbitrage path that returns to the starting token.
 * 
 * Example cycle: WETH -> USDC -> DAI -> WETH
 * tokenPath would be: [WETH, USDC, DAI, WETH] (start token repeated at end)
 * edges would contain the 3 pools used for the swaps
 */
struct Cycle {
    std::vector<std::string> tokenPath;  // Closed path, start token repeated at end
    std::vector<Edge> edges;             // Pools used for each swap
};

/**
 * CycleDetector - Finds arbitrage cycles in the token graph using DFS.
 * 
 * Algorithm:
 * 1. Start DFS from every token in the graph
 * 2. Explore all possible swap paths up to maxDepth
 * 3. When we return to the starting token, we've found a cycle
 * 4. Use canonical deduplication to avoid counting the same cycle multiple times
 * 
 * Optimizations:
 * - Uses unordered_set for O(1) visited token checks
 * - Tracks used pool IDs to prevent using the same pool twice in one cycle
 * - Progress logging every 1000 tokens
 */
class CycleDetector {
public:
    /**
     * findCycles - Discovers all unique arbitrage cycles up to maxDepth.
     * 
     * @param graph Token swap graph to search
     * @param maxDepth Maximum number of swaps in a cycle (typically 3-5)
     * @return Vector of unique cycles found
     */
    std::vector<Cycle> findCycles(const Graph& graph, int maxDepth = Config::MAX_CYCLE_DEPTH) const;

private:
    /**
     * dfs - Recursive depth-first search to explore arbitrage paths.
     * 
     * Maintains path state during recursion and backtracks when:
     * - We hit maxDepth
     * - We visit a token already in the path (would create invalid sub-cycle)
     * - We use a pool already in the path (same pool can't be used twice)
     * 
     * @param visitedTokens Optimized O(1) set for checking if token is in current path
     * @param tokenPath Vector tracking path order (needed for cycle output)
     */
    void dfs(
        const Graph& graph,
        const std::string& start,
        const std::string& current,
        int depth,
        int maxDepth,
        std::vector<std::string>& tokenPath,
        std::unordered_set<std::string>& visitedTokens,
        std::vector<Edge>& edgePath,
        std::unordered_set<std::string>& usedPoolIds,
        std::vector<Cycle>& cycles,
        std::unordered_set<std::string>& canonicalKeys) const;

    /**
     * canonicalKey - Generates a unique string key for cycle deduplication.
     * 
     * Problem: The cycle A->B->C->A is the same as B->C->A->B and C->A->B->C
     * (different starting points) and also same as A->C->B->A (reversed direction).
     * 
     * Solution: Try all rotations and reversals, pick the lexicographically smallest
     * serialization as the canonical key. This ensures all equivalent cycles map to
     * the same key.
     * 
     * @param closedTokenPath Cycle path with start token repeated at end
     * @return Canonical string key for deduplication
     */
    std::string canonicalKey(const std::vector<std::string>& closedTokenPath) const;
};
