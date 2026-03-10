#include "CycleDetector.h"

#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<Cycle> CycleDetector::findCycles(const Graph& graph, int maxDepth) const {
    std::vector<Cycle> cycles;
    std::unordered_set<std::string> canonicalKeys;  // Tracks unique cycles via canonical keys

    auto starts = graph.getAllTokens();
    size_t totalTokens = starts.size();
    size_t processed = 0;
    
    // Start DFS from every token in the graph
    // Note: This explores redundant paths, but canonical deduplication eliminates duplicates
    for (const auto& start : starts) {
        processed++;
        // Progress logging for long-running searches
        if (processed % 1000 == 0 || processed == totalTokens) {
            std::cout << "Processed " << processed << "/" << totalTokens 
                      << " tokens, found " << cycles.size() << " unique cycles\n";
        }
        
        // Initialize DFS state for this starting token
        std::vector<std::string> tokenPath{start};
        std::unordered_set<std::string> visitedTokens{start};  // O(1) lookup for visited check
        std::vector<Edge> edgePath;
        std::unordered_set<std::string> usedPoolIds;
        dfs(graph, start, start, 0, maxDepth, tokenPath, visitedTokens, edgePath, usedPoolIds, cycles, canonicalKeys);
    }

    return cycles;
}

void CycleDetector::dfs(
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
    std::unordered_set<std::string>& canonicalKeys) const {
    // Base case: reached maximum depth, stop exploring
    if (depth >= maxDepth) {
        return;
    }

    // Explore all possible swaps from current token
    const auto& edges = graph.getEdgesFrom(current);
    for (const auto& edge : edges) {
        // Skip if we've already used this pool in the current path
        if (usedPoolIds.count(edge.pool->id()) > 0) {
            continue;
        }

        const std::string& next = edge.toTokenId;

        // CYCLE FOUND: We've returned to the starting token!
        if (next == start && depth + 1 >= 2) {  // Need at least 2 swaps for a valid cycle
            std::vector<std::string> closedPath = tokenPath;
            closedPath.push_back(start);  // Close the cycle by adding start token again
            
            // Use canonical key to check if this cycle is truly unique
            auto key = canonicalKey(closedPath);
            if (canonicalKeys.insert(key).second) {  // .second is true if insertion happened
                auto closedEdges = edgePath;
                closedEdges.push_back(edge);
                cycles.push_back(Cycle{closedPath, closedEdges});
            }
            continue;  // Don't recurse past the cycle completion
        }

        // Prune: Can't reach back to start if we're already at max depth
        if (depth + 1 >= maxDepth) {
            continue;
        }

        // Prune: Skip if this token is already in our path (would create invalid sub-cycle)
        // Using unordered_set for O(1) lookup instead of O(n) vector search
        if (visitedTokens.count(next) > 0) {
            continue;
        }

        // RECURSE: Add this edge to the path and explore deeper
        usedPoolIds.insert(edge.pool->id());
        edgePath.push_back(edge);
        tokenPath.push_back(next);
        visitedTokens.insert(next);

        dfs(graph, start, next, depth + 1, maxDepth, tokenPath, visitedTokens, edgePath, usedPoolIds, cycles, canonicalKeys);

        // BACKTRACK: Remove this edge from the path to try other options
        visitedTokens.erase(next);
        tokenPath.pop_back();
        edgePath.pop_back();
        usedPoolIds.erase(edge.pool->id());
    }
}

std::string CycleDetector::canonicalKey(const std::vector<std::string>& closedTokenPath) const {
    // Validate input: need at least 4 elements (e.g., A -> B -> C -> A)
    if (closedTokenPath.size() < 4) {
        return "";
    }

    // Remove the repeated end element to get the cycle loop
    // e.g., [A, B, C, A] becomes [A, B, C]
    std::vector<std::string> loop(closedTokenPath.begin(), closedTokenPath.end() - 1);
    int n = static_cast<int>(loop.size());

    // Helper to join tokens into a string key
    auto serialize = [](const std::vector<std::string>& v) {
        std::ostringstream oss;
        for (size_t i = 0; i < v.size(); ++i) {
            if (i != 0) {
                oss << ">";
            }
            oss << v[i];
        }
        return oss.str();
    };

    std::string best;
    bool initialized = false;

    // Try all rotations and reversals to find the lexicographically smallest key
    // This ensures cycles that are the same but start at different points map to the same key
    for (int rot = 0; rot < n; ++rot) {
        // Forward rotation: [A, B, C] -> [B, C, A] -> [C, A, B]
        std::vector<std::string> fwd;
        fwd.reserve(n);
        for (int i = 0; i < n; ++i) {
            fwd.push_back(loop[(rot + i) % n]);
        }
        std::string fwdKey = serialize(fwd);
        if (!initialized || fwdKey < best) {
            best = fwdKey;
            initialized = true;
        }

        // Reverse rotation: [A, B, C] -> [A, C, B] (handles cycles traversed backwards)
        std::vector<std::string> rev;
        rev.reserve(n);
        for (int i = 0; i < n; ++i) {
            int idx = (rot - i) % n;
            if (idx < 0) {
                idx += n;  // Handle negative modulo
            }
            rev.push_back(loop[idx]);
        }
        std::string revKey = serialize(rev);
        if (revKey < best) {
            best = revKey;
        }
    }

    return best;
}
