#include "JSONParser.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace {
// Helper functions to parse JSON fields that may be strings or numbers
// Some pool data has numbers as strings, so we handle both cases


int parseInt(const nlohmann::json& j, const char* key, int fallback = 18) {
    if (!j.contains(key) || j[key].is_null()) {
        return fallback;
    }
    const auto& v = j[key];
    if (v.is_string()) {
        return std::stoi(v.get<std::string>());  // Handle "18" as string
    }
    if (v.is_number_integer()) {
        return v.get<int>();  // Handle 18 as number
    }
    return fallback;
}

double parseDouble(const nlohmann::json& j, const char* key, double fallback = 0.0) {
    if (!j.contains(key) || j[key].is_null()) {
        return fallback;
    }
    const auto& v = j[key];
    if (v.is_string()) {
        return std::stod(v.get<std::string>());
    }
    if (v.is_number()) {
        return v.get<double>();
    }
    return fallback;
}

std::optional<double> parseOptionalDouble(const nlohmann::json& j, const char* key) {
    if (!j.contains(key) || j[key].is_null()) {
        return std::nullopt;
    }
    return parseDouble(j, key, 0.0);
}

}  // namespace

ParseResult JSONParser::parsePoolsFromFile(const std::string& filePath) const {
    std::ifstream in(filePath);
    if (!in) {
        throw std::runtime_error("Failed to open JSON file: " + filePath);
    }

    nlohmann::json root;
    in >> root;
    if (!root.is_array()) {
        throw std::runtime_error("Expected top-level JSON array");
    }

    ParseResult result;
    // Parse each pool from the JSON array

    for (const auto& item : root) {
            // Validate required fields
        if (!item.contains("id") || !item.contains("token0") || !item.contains("token1")) {
            continue;  // Skip malformed pool entries
        }

        std::string poolId = item["id"].get<std::string>();
        const auto& t0j = item["token0"];
        const auto& t1j = item["token1"];

        std::string t0Id = t0j.value("id", "");
        std::string t1Id = t1j.value("id", "");
        if (t0Id.empty() || t1Id.empty()) {
            continue;  // Skip if token IDs are missing
            // Token deduplication: reuse existing Token objects if we've seen them before
            // This saves memory and ensures token metadata is consistent across pools
        }

        auto getOrCreateToken = [&result](const std::string& id, int decimals, const std::string& symbol) {
            auto it = result.tokens.find(id);
            if (it != result.tokens.end()) {
                return it->second;  // Reuse existing token
                        // First time seeing this token, create new one
            }
            auto token = std::make_shared<Token>(id, decimals, symbol);
            result.tokens.emplace(id, token);
            return token;
        };

        auto token0 = getOrCreateToken(
            t0Id,
            parseInt(t0j, "decimals", 18),  // Default to 18 if not specified
            t0j.contains("symbol") && t0j["symbol"].is_string() ? t0j["symbol"].get<std::string>() : "");

        auto token1 = getOrCreateToken(
            t1Id,
            parseInt(t1j, "decimals", 18),
            t1j.contains("symbol") && t1j["symbol"].is_string() ? t1j["symbol"].get<std::string>() : "");
    // Parse reserve amounts

        double reserve0 = parseDouble(item, "reserve0", 0.0);
        double reserve1 = parseDouble(item, "reserve1", 0.0);
        double reserveUsd = parseDouble(item, "reserveUSD", 0.0);
    // Skip pools with zero reserves (invalid/inactive pools)

        if (reserve0 <= 0.0 || reserve1 <= 0.0) {
            continue;
        }

        auto pool = std::make_shared<Pool>(
            poolId,
            token0,
            token1,
            reserve0,
            reserve1,
            reserveUsd,
            parseOptionalDouble(item, "token0Price"),
            parseOptionalDouble(item, "token1Price"));

        result.pools.push_back(pool);
    }

    return result;
}
