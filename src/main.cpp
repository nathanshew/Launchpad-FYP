#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"
#include "CycleDetector.h"
#include "Graph.h"
#include "HTMLExporter.h"
#include "JSONParser.h"

int main(int argc, char** argv) {
    std::string inputFile = "v2pools.json";
    std::string outputFile = "report.html";
    int maxDepth = 5;

    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc >= 3) {
        outputFile = argv[2];
    }
    if (argc >= 4) {
        maxDepth = std::stoi(argv[3]);
    }

    try {
        JSONParser parser;
        ParseResult parsed = parser.parsePoolsFromFile(inputFile);

        std::cout << "Loaded pools: " << parsed.pools.size() << "\n";
        std::cout << "Loaded unique tokens: " << parsed.tokens.size() << "\n";

        // Filter pools by liquidity to reduce search space
        double minReserveUSD = 10000.0; // Only consider pools with $10k+ liquidity
        std::vector<std::shared_ptr<Pool>> filteredPools;
        for (const auto& pool : parsed.pools) {
            if (pool->reserveUsd() >= minReserveUSD) {
                filteredPools.push_back(pool);
            }
        }
        std::cout << "Filtered to high-liquidity pools: " << filteredPools.size() 
                  << " (>= $" << minReserveUSD << ")\n";

        Graph graph;
        for (const auto& pool : filteredPools) {
            graph.addPool(pool);
        }
        
        auto tokens = graph.getAllTokens();
        std::cout << "Tokens in filtered graph: " << tokens.size() << "\n";

        CycleDetector detector;
        std::vector<Cycle> cycles = detector.findCycles(graph, maxDepth);
        std::cout << "Detected cycles: " << cycles.size() << "\n";

        ArbitrageAnalyzer analyzer(filteredPools);
        std::vector<ArbitrageOpportunity> opportunities = analyzer.analyzeCycles(cycles);
        std::cout << "Profitable cycles: " << opportunities.size() << "\n";

        size_t topN = std::min<size_t>(10, opportunities.size());
        std::vector<ArbitrageOpportunity> top(opportunities.begin(), opportunities.begin() + topN);

        HTMLExporter exporter;
        exporter.exportReport(top, outputFile, cycles.size(), opportunities.size());

        std::cout << "Wrote HTML report: " << outputFile << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
