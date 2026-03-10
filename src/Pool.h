#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Token.h"

class Pool {
public:
    Pool(
        std::string poolId,
        std::shared_ptr<Token> token0,
        std::shared_ptr<Token> token1,
        double reserve0,
        double reserve1,
        double reserveUsd,
        std::optional<double> token0Price = std::nullopt,
        std::optional<double> token1Price = std::nullopt);

    const std::string& id() const;
    const std::shared_ptr<Token>& token0() const;
    const std::shared_ptr<Token>& token1() const;
    double reserve0() const;
    double reserve1() const;
    double reserveUsd() const;
    const std::optional<double>& token0PriceSnapshot() const;
    const std::optional<double>& token1PriceSnapshot() const;

    bool hasToken(const std::string& tokenId) const;
    bool isToken0(const std::string& tokenId) const;
    double getReserveForToken(const std::string& tokenId) const;

    // Simulates one swap on local reserve copies only.
    double simulateSwap(double amountIn, const std::string& inputTokenId, double& localReserve0, double& localReserve1) const;

private:
    double roundDownToDecimals(double amount, int decimals) const;

    std::string poolId_;
    std::shared_ptr<Token> token0_;
    std::shared_ptr<Token> token1_;
    double reserve0_;
    double reserve1_;
    double reserveUsd_;
    std::optional<double> token0Price_;
    std::optional<double> token1Price_;
};
