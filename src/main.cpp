#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"
#include "CycleDetector.h"
#include "Config.h"
#include "Graph.h"
#include "HTMLExporter.h"
#include "JSONExporter.h"
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
    // Parse command-line arguments (all optional with defaults from Config)
    std::string inputFile = Config::DEFAULT_INPUT_FILE;      // Pool snapshot JSON
    std::string outputFile = Config::DEFAULT_HTML_OUTPUT;    // Output HTML report
    int maxDepth = Config::MAX_CYCLE_DEPTH;                  // Max swaps per cycle

    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc >= 3) {
        outputFile = argv[2];
    }
    if (argc >= 4) {
        try {
            maxDepth = std::stoi(argv[3]);
        } catch (const std::exception&) {
            std::cerr << "Invalid maxDepth argument: '" << argv[3] << "'. Expected integer.\n";
            return 1;
        }
        if (maxDepth < Config::MIN_CYCLE_LENGTH) {
            std::cerr << "Invalid maxDepth argument: must be >= "
                      << Config::MIN_CYCLE_LENGTH << ".\n";
            return 1;
        }
    }

    try {
            // STEP 1: Parse pool data from JSON file
        JSONParser parser;
        ParseResult parsed = parser.parsePoolsFromFile(inputFile);

        std::cout << "Loaded pools: " << parsed.pools.size() << "\n";
        std::cout << "Loaded unique tokens: " << parsed.tokens.size() << "\n";

        // STEP 2: Filter pools by liquidity to reduce search space
        // Without this, cycle detection on 33k+ tokens is prohibitively slow
        std::vector<std::shared_ptr<Pool>> filteredPools;
        for (const auto& pool : parsed.pools) {
            if (pool->reserveUsd() >= Config::MIN_RESERVE_USD) {
                filteredPools.push_back(pool);
            }
        }
        std::cout << "Filtered to high-liquidity pools: " << filteredPools.size() 
                  << " (>= $" << Config::MIN_RESERVE_USD << ")\n";
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
        size_t topN = std::min<size_t>(Config::REPORT_TOP_N, opportunities.size());
        std::vector<ArbitrageOpportunity> top(opportunities.begin(), opportunities.begin() + topN);

        HTMLExporter exporter;
        exporter.exportReport(top, outputFile, cycles.size(), opportunities.size());

        std::cout << "Wrote HTML report: " << outputFile << "\n";

        // STEP 7: Export cycles for Part 2 validation
        std::string validatorAddress = Config::PLACEHOLDER_VALIDATOR_ADDRESS;
        std::string jsonOutputFile = Config::DEFAULT_JSON_OUTPUT;
        
        JSONExporter jsonExporter;
        jsonExporter.exportForValidation(top, jsonOutputFile, validatorAddress, Config::REPORT_TOP_N);
        std::cout << "Wrote Part 2 validation input: " << jsonOutputFile << "\n";
        std::cout << "Update Config::PLACEHOLDER_VALIDATOR_ADDRESS before Part 2 validation.\n";
        
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
