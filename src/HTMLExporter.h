#pragma once

#include <string>
#include <vector>

#include "ArbitrageAnalyzer.h"

class HTMLExporter {
public:
    void exportReport(
        const std::vector<ArbitrageOpportunity>& opportunities,
        const std::string& filePath,
        size_t totalCycles,
        size_t profitableCycles) const;

private:
    std::string escapeHtml(const std::string& s) const;
    std::string formatCyclePath(const Cycle& cycle) const;
    std::string formatPoolIds(const Cycle& cycle) const;
};
