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
/**
 * DEX Arbitrage Analyzer - Main EntryPoint
 * 
 * Pipeline:
 * 1. Parse Uniswap V2 pool data from JSON file
 * 2. Filter pools by liquidity (optimization to reduce search space)
 * 3. Build token graph from pools
 * 4. Detect arbitrage cycles using DFS
 * 5. Evaluate profitability with trade size optimization
 * 6. Export top 10 opportunities to HTML report
 * 
 * Usage: arbitrage_app.exe [input.json] [output.html] [maxDepth]
 */

int main(int argc, char** argv) {
    // Parse command-line arguments (all optional with defaults)
    std::string inputFile = "v2pools.json";      // Pool snapshot JSON
    std::string outputFile = "report.html";      // Output HTML report
    int maxDepth = 5;                            // Max swaps per cycle (3-5 recommended)

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
            // STEP 1: Parse pool data from JSON file
        JSONParser parser;
        ParseResult parsed = parser.parsePoolsFromFile(inputFile);

        std::cout << "Loaded pools: " << parsed.pools.size() << "\n";
        std::cout << "Loaded unique tokens: " << parsed.tokens.size() << "\n";

        // STEP 2: Filter pools by liquidity to reduce search space
        // Without this, cycle detection on 33k+ tokens is prohibitively slow
        double minReserveUSD = 10000.0;  // Only consider pools with $10k+ liquidity
        std::vector<std::shared_ptr<Pool>> filteredPools;
        for (const auto& pool : parsed.pools) {
            if (pool->reserveUsd() >= minReserveUSD) {
                filteredPools.push_back(pool);
            }
        }
        std::cout << "Filtered to high-liquidity pools: " << filteredPools.size() 
                  << " (>= $" << minReserveUSD << ")\n";
    // STEP 3: Build graph from filtered pools

        Graph graph;
        for (const auto& pool : filteredPools) {
            graph.addPool(pool);
        }
        
        auto tokens = graph.getAllTokens();
        std::cout << "Tokens in filtered graph: " << tokens.size() << "\n";
    // STEP 4: Detect cycles using DFS with canonical deduplication

        CycleDetector detector;
        std::vector<Cycle> cycles = detector.findCycles(graph, maxDepth);
        std::cout << "Detected cycles: " << cycles.size() << "\n";
    // STEP 5: Analyze profitability with trade size optimization

        ArbitrageAnalyzer analyzer(filteredPools);
        std::vector<ArbitrageOpportunity> opportunities = analyzer.analyzeCycles(cycles);
        std::cout << "Profitable cycles: " << opportunities.size() << "\n";
    // STEP 6: Export top 10 to HTML report

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
