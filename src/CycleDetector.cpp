#include "CycleDetector.h"

#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<Cycle> CycleDetector::findCycles(const Graph& graph, int maxDepth) const {
    std::vector<Cycle> cycles;
    std::unordered_set<std::string> canonicalKeys;

    auto starts = graph.getAllTokens();
    size_t totalTokens = starts.size();
    size_t processed = 0;
    
    for (const auto& start : starts) {
        processed++;
        if (processed % 1000 == 0 || processed == totalTokens) {
            std::cout << "Processed " << processed << "/" << totalTokens 
                      << " tokens, found " << cycles.size() << " unique cycles\n";
        }
        
        std::vector<std::string> tokenPath{start};
        std::unordered_set<std::string> visitedTokens{start};
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
    if (depth >= maxDepth) {
        return;
    }

    const auto& edges = graph.getEdgesFrom(current);
    for (const auto& edge : edges) {
        if (usedPoolIds.count(edge.pool->id()) > 0) {
            continue;
        }

        const std::string& next = edge.toTokenId;

        if (next == start && depth + 1 >= 2) {
            std::vector<std::string> closedPath = tokenPath;
            closedPath.push_back(start);
            auto key = canonicalKey(closedPath);
            if (canonicalKeys.insert(key).second) {
                auto closedEdges = edgePath;
                closedEdges.push_back(edge);
                cycles.push_back(Cycle{closedPath, closedEdges});
            }
            continue;
        }

        if (depth + 1 >= maxDepth) {
            continue;
        }

        if (visitedTokens.count(next) > 0) {
            continue;
        }

        usedPoolIds.insert(edge.pool->id());
        edgePath.push_back(edge);
        tokenPath.push_back(next);
        visitedTokens.insert(next);

        dfs(graph, start, next, depth + 1, maxDepth, tokenPath, visitedTokens, edgePath, usedPoolIds, cycles, canonicalKeys);

        visitedTokens.erase(next);
        tokenPath.pop_back();
        edgePath.pop_back();
        usedPoolIds.erase(edge.pool->id());
    }
}

std::string CycleDetector::canonicalKey(const std::vector<std::string>& closedTokenPath) const {
    if (closedTokenPath.size() < 4) {
        return "";
    }

    std::vector<std::string> loop(closedTokenPath.begin(), closedTokenPath.end() - 1);
    int n = static_cast<int>(loop.size());

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

    for (int rot = 0; rot < n; ++rot) {
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

        std::vector<std::string> rev;
        rev.reserve(n);
        for (int i = 0; i < n; ++i) {
            int idx = (rot - i) % n;
            if (idx < 0) {
                idx += n;
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
