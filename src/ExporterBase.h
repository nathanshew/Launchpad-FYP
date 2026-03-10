#pragma once

#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"

/**
 * ExporterBase - Abstract base class for cycle opportunity exporters.
 * 
 * Defines the common interface for exporting arbitrage opportunities in different formats.
 * Derived classes implement format-specific export logic.
 * 
 * Design Principles:
 * - Single Responsibility: Derived classes handle one format each
 * - Encapsulation: Protected methods for shared helper functionality
 * - Clear Interface: Public export method contract
 * - Exception Safety: Subclasses must handle their own I/O exceptions
 */
class ExporterBase {
public:
    virtual ~ExporterBase() = default;

    /**
     * exportReport - Generates and writes opportunity data in format-specific manner.
     * 
     * Contract:
     * - Must write output to filePath
     * - Must include all opportunities with full metadata
     * - Must throw std::runtime_error on file I/O failure
     * - Should be const (no state mutation)
     * 
     * @param opportunities Vector of analyzed arbitrage opportunities
     * @param filePath      Output file path
     * @param totalCycles   Total cycles detected (for reporting context)
     * @param profitableCycles Number of profitable cycles found
     * @throws std::runtime_error if file cannot be written
     */
    virtual void exportReport(
        const std::vector<ArbitrageOpportunity>& opportunities,
        const std::string& filePath,
        size_t totalCycles,
        size_t profitableCycles) const = 0;

protected:
    /**
     * Protected helper methods for subclasses
     */
    
    /**
     * escapeForFormat - Escapes special characters for the output format.
     * 
     * Default implementation escapes nothing (override in subclasses).
     * 
     * @param input Raw string that may contain format-special characters
     * @return Escaped string safe for format
     */
    virtual std::string escapeForFormat(const std::string& input) const {
        return input;
    }
};
