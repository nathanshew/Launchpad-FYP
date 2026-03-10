#pragma once

#include <string>
#include <vector>

#include "ExporterBase.h"

/**
 * HTMLExporter - Exports arbitrage opportunities as styled HTML report.
 * 
 * Implements ExporterBase to convert ArbitrageOpportunity data into a 
 * professional HTML table with:
 * - Responsive styling (CSS Grid)
 * - Color-coded profit amounts (green for good)
 * - Monospace formatting for addresses
 * - Summary statistics (total/profitable cycle counts)
 * 
 * Format Details:
 * - UTF-8 encoding with HTML entity escaping
 * - Responsive table design with sticky headers
 * - Self-contained CSS (no external stylesheets)
 * - Sorted by USD profit (descending)
 * 
 * Exception Safety: Basic guarantee (file I/O errors throw std::runtime_error)
 */
class HTMLExporter : public ExporterBase {
public:
    /**
     * Export opportunities as an HTML report.
     * 
     * @see ExporterBase::exportReport
     * @throws std::runtime_error if file cannot be opened or written
     */
    void exportReport(
        const std::vector<ArbitrageOpportunity>& opportunities,
        const std::string& filePath,
        size_t totalCycles,
        size_t profitableCycles) const override;

protected:
    /**
     * Escapes string for safe HTML display (replaces &, <, >, ", ')
     * 
     * @param s Raw string that may contain HTML special characters
     * @return HTML-safe string with entities replaced
     */
    std::string escapeForFormat(const std::string& s) const override;

private:
    /**
     * formatCyclePath - Formats token path for display
     * 
     * Joins token addresses with " -> " separator
     * 
     * @return String like "0xabc... -> 0xdef... -> 0xabc..."
     */
    std::string formatCyclePath(const Cycle& cycle) const;

    /**
     * formatPoolIds - Formats pool address list for display
     * 
     * Joins pool IDs with " | " separator
     * 
     * @return String like "0x123... | 0x456... | 0x789..."
     */
    std::string formatPoolIds(const Cycle& cycle) const;
};
