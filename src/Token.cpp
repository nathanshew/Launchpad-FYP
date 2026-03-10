#include "Token.h"

#include <stdexcept>

// Constructor initializes immutable token properties
Token::Token(std::string id, int decimals, std::string symbol)
    : id_(std::move(id)), decimals_(decimals), symbol_(std::move(symbol)) {
    if (id_.empty()) {
        throw std::invalid_argument("Token id cannot be empty");
    }
    if (decimals_ < 0) {
        throw std::invalid_argument("Token decimals cannot be negative");
    }
}

// Getter methods for token properties
const std::string& Token::id() const { return id_; }

int Token::decimals() const { return decimals_; }

const std::string& Token::symbol() const { return symbol_; }
