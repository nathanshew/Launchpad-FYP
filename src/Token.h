#pragma once

#include <string>

/**
 * Token - Represents an ERC-20 token with its metadata.
 * 
 * This is an immutable data class that stores token properties:
 * - id: The Ethereum address (e.g., 0xabc...)
 * - decimals: Precision for token amounts (typically 18)
 * - symbol: Human-readable name (e.g., "WETH", "USDC")
 * 
 * Tokens are shared across multiple pools via shared_ptr to avoid duplication.
 */
class Token {
public:
    Token(std::string id, int decimals, std::string symbol = "");

    const std::string& id() const;
    int decimals() const;
    const std::string& symbol() const;

private:
    std::string id_;
    int decimals_;
    std::string symbol_;
};
