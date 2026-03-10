#pragma once

#include <string>

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
