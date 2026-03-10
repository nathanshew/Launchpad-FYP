#pragma once

#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"

/**
 * JSONExporter - Exports Part 1 arbitrage opportunities in Part 2 validator format.
 * 
 * Creates JSON files compatible with Part 2's validateCycle script.
 * Applies JSON-safe escaping and converts numeric values to proper representations.
 * 
 * Design Principles:
 * - Single Responsibility: Format conversion only (no reporting)
 * - Encapsulation: Private escaping logic
 * - Clear Interface: Simple contract with exportForValidation
 * - Exception Safety: Throws on file I/O errors
 * 
 * Note: This exporter serves a different purpose than formal "reports" so it does
 * not inherit from ExporterBase, which is reserved for report-style exports.
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
     * Numeric amounts are exported as wei strings (integers, no decimals) to preserve
     * precision for on-chain validation where floating-point is not usable.
     * 
     * Contract:
     * - Must write valid JSON to filePath
     * - amountIn and minOut must be integer wei strings
     * - tokens/pools arrays order must match cycle path
     * - throws std::runtime_error on file I/O failure
     * 
     * @param opportunities  - Ranked opportunities from Part 1 analysis
     * @param filePath       - Output JSON file path
     * @param validatorAddress - Deployed Part 2 validator contract address
     * @param count          - Number of top opportunities to export (default: 10)
     * @throws std::runtime_error if file cannot be written
     */
    void exportForValidation(
        const std::vector<ArbitrageOpportunity>& opportunities,
        const std::string& filePath,
        const std::string& validatorAddress,
        size_t count = 10) const;

private:
    /**
     * escapeJson - Escapes string for safe JSON embedding
     * 
     * Handles: quotes, backslashes, control characters via standard escaping
     * Does NOT handle unicode (tokens are hex strings, safe by construction)
     * 
     * @param s - Raw string that may contain JSON special characters
     * @return JSON-escaped string safe for embedding in quoted values
     */
    std::string escapeJson(const std::string& s) const;
};
