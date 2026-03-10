#include "Pool.h"

#include <cmath>
#include <limits>
#include <stdexcept>

Pool::Pool(
    std::string poolId,
    std::shared_ptr<Token> token0,
    std::shared_ptr<Token> token1,
    double reserve0,
    double reserve1,
    double reserveUsd,
    std::optional<double> token0Price,
    std::optional<double> token1Price)
    : poolId_(std::move(poolId)),
      token0_(std::move(token0)),
      token1_(std::move(token1)),
      reserve0_(reserve0),
      reserve1_(reserve1),
      reserveUsd_(reserveUsd),
      token0Price_(token0Price),
      token1Price_(token1Price) {}

const std::string& Pool::id() const { return poolId_; }

const std::shared_ptr<Token>& Pool::token0() const { return token0_; }

const std::shared_ptr<Token>& Pool::token1() const { return token1_; }

double Pool::reserve0() const { return reserve0_; }

double Pool::reserve1() const { return reserve1_; }

double Pool::reserveUsd() const { return reserveUsd_; }

const std::optional<double>& Pool::token0PriceSnapshot() const { return token0Price_; }

const std::optional<double>& Pool::token1PriceSnapshot() const { return token1Price_; }

bool Pool::hasToken(const std::string& tokenId) const { return token0_->id() == tokenId || token1_->id() == tokenId; }

bool Pool::isToken0(const std::string& tokenId) const {
    if (!hasToken(tokenId)) {
        throw std::invalid_argument("Token not found in pool");
    }
    return token0_->id() == tokenId;
}

double Pool::getReserveForToken(const std::string& tokenId) const {
    if (token0_->id() == tokenId) {
        return reserve0_;
    }
    if (token1_->id() == tokenId) {
        return reserve1_;
    }
    throw std::invalid_argument("Token not found in pool");
}

double Pool::simulateSwap(double amountIn, const std::string& inputTokenId, double& localReserve0, double& localReserve1) const {
    if (amountIn <= 0.0 || !std::isfinite(amountIn)) {
        return 0.0;
    }

    bool inputIsToken0 = isToken0(inputTokenId);
    double reserveIn = inputIsToken0 ? localReserve0 : localReserve1;
    double reserveOut = inputIsToken0 ? localReserve1 : localReserve0;

    if (reserveIn <= 0.0 || reserveOut <= 0.0) {
        return 0.0;
    }

    // Uniswap V2 formula with 0.3% fee.
    double amountInWithFee = amountIn * 997.0;
    double numerator = amountInWithFee * reserveOut;
    double denominator = reserveIn * 1000.0 + amountInWithFee;

    if (denominator <= std::numeric_limits<double>::epsilon()) {
        return 0.0;
    }

    double rawAmountOut = numerator / denominator;
    int outDecimals = inputIsToken0 ? token1_->decimals() : token0_->decimals();
    double amountOut = roundDownToDecimals(rawAmountOut, outDecimals);

    if (amountOut <= 0.0 || amountOut >= reserveOut) {
        return 0.0;
    }

    if (inputIsToken0) {
        localReserve0 += amountIn;
        localReserve1 -= amountOut;
    } else {
        localReserve1 += amountIn;
        localReserve0 -= amountOut;
    }

    return amountOut;
}

double Pool::roundDownToDecimals(double amount, int decimals) const {
    if (decimals < 0) {
        return amount;
    }

    long double scale = std::pow(10.0L, static_cast<long double>(decimals));
    long double scaled = std::floor(static_cast<long double>(amount) * scale);
    return static_cast<double>(scaled / scale);
}
