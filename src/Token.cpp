#include "Token.h"

// Constructor initializes immutable token properties
Token::Token(std::string id, int decimals, std::string symbol)
    : id_(std::move(id)), decimals_(decimals), symbol_(std::move(symbol)) {}

// Getter methods for token properties
const std::string& Token::id() const { return id_; }

int Token::decimals() const { return decimals_; }

const std::string& Token::symbol() const { return symbol_; }
