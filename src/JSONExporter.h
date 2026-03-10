#pragma once

#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"

/**
 * JSONExporter - Exports Part 1 arbitrage opportunities in Part 2 validator format.
 * 
 * Creates JSON files compatible with Part 2's validateCycle script.
 */
class JSONExporter {
public:
    /**
     * exportForValidation - Exports opportunities in Part 2 input format.
     * 
     * Output format per opportunity:
     * {
     *   "validatorAddress": "0x...",
     *   "tokens": ["0xTokenA", "0xTokenB", ...],
     *   "pools": ["0xPoolAB", "0xPoolBC", ...],
     *   "amountIn": "1000000000000000000",
     *   "minOut": "1010000000000000000",
     *   "mode": "call"
     * }
     * 
     * @param opportunities - Ranked opportunities from Part 1
     * @param filePath - Output JSON file path
     * @param validatorAddress - Deployed Part 2 validator contract address
     * @param count - Number of top opportunities to export (default: all)
     */
    void exportForValidation(
        const std::vector<ArbitrageOpportunity>& opportunities,
        const std::string& filePath,
        const std::string& validatorAddress,
        size_t count = 10) const;

private:
    std::string escapeJson(const std::string& s) const;
};
