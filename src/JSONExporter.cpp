#include "JSONExporter.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {

std::string toWeiString(double amount, int decimals) {
    if (!std::isfinite(amount) || amount < 0.0) {
        throw std::invalid_argument("Amount must be finite and non-negative for wei conversion");
    }
    if (decimals < 0 || decimals > 18) {
        throw std::invalid_argument("Token decimals out of supported range [0, 18]");
    }

    // Use long double so large notional trade sizes can still be represented for JSON export.
    long double scale = std::pow(10.0L, static_cast<long double>(decimals));
    long double scaled = std::floor(static_cast<long double>(amount) * scale + 1e-9L);
    if (!std::isfinite(static_cast<double>(scaled))) {
        throw std::overflow_error("Converted wei amount is out of range");
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << scaled;
    return oss.str();
}

std::string addOneUnit(const std::string& nonNegativeInteger) {
    std::string out = nonNegativeInteger;
    int i = static_cast<int>(out.size()) - 1;
    while (i >= 0 && out[static_cast<size_t>(i)] == '9') {
        out[static_cast<size_t>(i)] = '0';
        --i;
    }
    if (i < 0) {
        return "1" + out;
    }
    out[static_cast<size_t>(i)] = static_cast<char>(out[static_cast<size_t>(i)] + 1);
    return out;
}

}  // namespace

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

        if (cycle.edges.empty()) {
            throw std::runtime_error("Cycle must include at least one edge for validation export");
        }
        int startTokenDecimals = cycle.edges.front().pool->isToken0(cycle.edges.front().fromTokenId)
            ? cycle.edges.front().pool->token0()->decimals()
            : cycle.edges.front().pool->token1()->decimals();

        // Convert to smallest units using the start token's decimals.
        std::string amountInWei = toWeiString(opp.optimalTradeSize, startTokenDecimals);
        std::string minOutWei = addOneUnit(amountInWei);

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
