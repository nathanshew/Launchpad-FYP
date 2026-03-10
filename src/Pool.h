#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Token.h"

/**
 * Pool - Represents a Uniswap V2 liquidity pool.
 * 
 * Stores pool state including:
 * - Two tokens (token0 and token1)
 * - Reserve amounts for each token
 * - Total USD liquidity
 * - Optional snapshot prices from the data source
 * 
 * Key design principle: NEVER mutates shared pool state during simulation.
 * The simulateSwap() method operates on LOCAL reserve copies passed by reference.
 */
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

    // Token relationship helpers
    bool hasToken(const std::string& tokenId) const;
    bool isToken0(const std::string& tokenId) const;
    double getReserveForToken(const std::string& tokenId) const;

    /**
     * simulateSwap - Calculates swap output using Uniswap V2 AMM formula.
     * 
     * Formula: amountOut = (reserveOut * amountIn * 997) / (reserveIn * 1000 + amountIn * 997)
     * The 997/1000 factor represents Uniswap's 0.3% trading fee.
     * 
     * CRITICAL: Operates on LOCAL reserve copies (localReserve0, localReserve1) that are
     * passed by reference and updated after the swap. Never modifies the Pool's internal
     * reserve0_/reserve1_ members to ensure thread-safe, deterministic profitability analysis.
     * 
     * Also applies per-hop rounding: output is rounded down to the output token's decimal
     * precision to match real on-chain behavior.
     * 
     * @param amountIn Amount of input token being swapped
     * @param inputTokenId Address of the input token
     * @param localReserve0 Reference to token0 reserve (updated after swap)
     * @param localReserve1 Reference to token1 reserve (updated after swap)
     * @return Amount of output token received (after fees and rounding)
     */
    double simulateSwap(double amountIn, const std::string& inputTokenId, double& localReserve0, double& localReserve1) const;

private:
    /**
     * roundDownToDecimals - Applies token decimal rounding to a raw amount.
     * 
     * Uses floor(amount * 10^decimals) / 10^decimals to match ERC-20 precision.
     * This prevents overestimating profits from fractional token amounts.
     */
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
