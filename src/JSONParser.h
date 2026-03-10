#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Pool.h"
/**
 * ParseResult - Output from JSON parsing containing pools and deduplicated tokens.
 * 
 * tokens map ensures each unique token address maps to a single Token object
 * shared across all pools (saves memory and ensures consistency).
 */

struct ParseResult {
    std::vector<std::shared_ptr<Pool>> pools;
    std::unordered_map<std::string, std::shared_ptr<Token>> tokens;
};
/**
 * JSONParser - Parses Uniswap V2 pool snapshot data from JSON.
 * 
 * Expected JSON format: Array of pool objects with:
 * - id: Pool address
 * - token0, token1: Token objects with id, decimals, symbol
 * - reserve0, reserve1: Pool reserves
 * - reserveUSD: Total liquidity in USD
 * 
 * Handles type variations (string vs number) and missing fields gracefully.
 * Performs token deduplication to create single Token instances.
 */

class JSONParser {
public:
        /**
         * parsePoolsFromFile - Loads and parses pool data from a JSON file.
         * 
         * @param filePath Path to the v2pools.json file
         * @return ParseResult with pools and deduplicated token map
         * @throws std::runtime_error if file can't be opened or JSON is invalid
         */
    ParseResult parsePoolsFromFile(const std::string& filePath) const;
};
