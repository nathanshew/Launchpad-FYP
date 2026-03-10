#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "Graph.h"

struct Cycle {
    std::vector<std::string> tokenPath;  // Closed path, start token repeated at end.
    std::vector<Edge> edges;
};

class CycleDetector {
public:
    std::vector<Cycle> findCycles(const Graph& graph, int maxDepth = 5) const;

private:
    void dfs(
        const Graph& graph,
        const std::string& start,
        const std::string& current,
        int depth,
        int maxDepth,
        std::vector<std::string>& tokenPath,
        std::vector<Edge>& edgePath,
        std::unordered_set<std::string>& usedPoolIds,
        std::vector<Cycle>& cycles,
        std::unordered_set<std::string>& canonicalKeys) const;

    std::string canonicalKey(const std::vector<std::string>& closedTokenPath) const;
};
