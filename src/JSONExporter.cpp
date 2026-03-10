#include "JSONExporter.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

void JSONExporter::exportForValidation(
    const std::vector<ArbitrageOpportunity>& opportunities,
    const std::string& filePath,
    const std::string& validatorAddress,
    size_t count) const {
    
    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file: " + filePath);
    }

    size_t exportCount = std::min(count, opportunities.size());

    out << "[\n";
    for (size_t i = 0; i < exportCount; ++i) {
        const auto& opp = opportunities[i];
        const Cycle& cycle = opp.cycle;

        // Convert optimal trade size to wei (assuming 18 decimals for simplicity)
        // In production, would need to look up actual token decimals
        // Use string conversion to avoid overflow
        std::ostringstream amountStream;
        amountStream << std::fixed << std::setprecision(0) << (opp.optimalTradeSize * 1e18);
        std::string amountInWei = amountStream.str();
        
        // minOut = amountIn + 1 (require any profit for validation)
        std::ostringstream minOutStream;
        minOutStream << std::fixed << std::setprecision(0) << (opp.optimalTradeSize * 1e18 + 1.0);
        std::string minOutWei = minOutStream.str();

        out << "  {\n";
        out << "    \"rank\": " << (i + 1) << ",\n";
        out << "    \"validatorAddress\": \"" << escapeJson(validatorAddress) << "\",\n";
        
        // tokens array: cycle path
        out << "    \"tokens\": [";
        for (size_t j = 0; j < cycle.tokenPath.size(); ++j) {
            out << "\"" << escapeJson(cycle.tokenPath[j]) << "\"";
            if (j + 1 < cycle.tokenPath.size()) out << ", ";
        }
        out << "],\n";

        // pools array: pool IDs from edges
        out << "    \"pools\": [";
        for (size_t j = 0; j < cycle.edges.size(); ++j) {
            out << "\"" << escapeJson(cycle.edges[j].pool->id()) << "\"";
            if (j + 1 < cycle.edges.size()) out << ", ";
        }
        out << "],\n";

        out << "    \"amountIn\": \"" << amountInWei << "\",\n";
        out << "    \"minOut\": \"" << minOutWei << "\",\n";
        out << "    \"mode\": \"call\",\n";
        
        // Metadata for reference (not used by validator)
        out << "    \"metadata\": {\n";
        out << "      \"profitUsd\": " << opp.profitUsd << ",\n";
        out << "      \"roi\": " << opp.percentageReturn << ",\n";
        out << "      \"optimalTradeSize\": " << opp.optimalTradeSize << "\n";
        out << "    }\n";
        
        out << "  }";
        if (i + 1 < exportCount) out << ",";
        out << "\n";
    }
    out << "]\n";

    out.close();
}

std::string JSONExporter::escapeJson(const std::string& s) const {
    std::ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}
