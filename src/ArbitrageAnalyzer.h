#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CycleDetector.h"

/**
 * ArbitrageOpportunity - Represents a profitable arbitrage cycle with its optimal trade parameters.
 * 
 * Contains the cycle path, optimal trade size, profit in USD, ROI%, and intermediate amounts.
 */
struct ArbitrageOpportunity {
    Cycle cycle;                        // The token path and pools involved
    double optimalTradeSize = 0.0;      // Starting amount that maximizes profit (in tokens)
    double optimalTradeSizeUsd = 0.0;   // Starting amount in USD (calculated from token price)
    double profitUsd = 0.0;             // Profit in USD (used for ranking)
    double percentageReturn = 0.0;      // ROI as percentage
    std::vector<double> amounts;        // Amount at each step of the cycle
    std::string startTokenId;           // Which token to start with
};

/**
 * ArbitrageAnalyzer - Evaluates profitability of cycles and ranks opportunities.
 * 
 * Key responsibilities:
 * 1. Build USD price map for all tokens (median aggregation from pool data)
 * 2. Test multiple trade sizes (1%, 10%, 20% of first pool reserve)
 * 3. Simulate each cycle with local reserve copies (no state mutation)
 * 4. Apply per-hop viability check (amountIn/reserveIn <= 20%)
 * 5. Calculate USD profit and rank by absolute profit
 */
class ArbitrageAnalyzer {
public:
    explicit ArbitrageAnalyzer(std::vector<std::shared_ptr<Pool>> pools);

    /**
     * analyzeCycles - Evaluates all cycles and returns profitable ones ranked by USD profit.
     * 
     * For each cycle:
     * - Tests 3 trade sizes: 1%, 10%, 20% of first hop's input reserve
     * - Simulates with local reserve copies (state-isolated)
     * - Converts profit to USD using token price map
     * - Keeps the trade size that maximizes USD profit
     * 
     * @return Opportunities sorted by descending USD profit
     */
    std::vector<ArbitrageOpportunity> analyzeCycles(const std::vector<Cycle>& cycles) const;
    
    // Returns the token->USD price map for debugging/reference
    const std::unordered_map<std::string, double>& tokenUsdPrices() const;

private:
    /**
     * SimulationResult - Internal struct for tracking cycle simulation outcomes.
     */
    struct SimulationResult {
        bool valid = false;              // False if any hop fails viability check
        std::vector<double> amounts;     // Amount after each swap
        double finalAmount = 0.0;        // Final amount in starting token
    };

    /**
     * buildTokenUsdPriceMap - Creates deterministic USD price estimates for all tokens.
     * 
     * For each token, calculates USD price from all pools containing it, then takes
     * the median to reduce impact of outlier pools.
     * 
     * Formula: tokenPrice = poolReserveUSD / (2 * tokenReserve)
     * 
     * @return Map from token address to estimated USD price
     */
    std::unordered_map<std::string, double> buildTokenUsdPriceMap() const;
    
    /**
     * simulateCycle - Runs a full cycle simulation with a given starting amount.
     * 
     * Maintains LOCAL reserves for each pool (no shared state mutation).
     * Applies per-hop viability check: amountIn/reserveIn must be <= 20%.
     * Returns invalid result if any hop fails.
     * 
     * @param cycle The arbitrage path to simulate
     * @param startAmount Initial trade size
     * @return Simulation result with validity and final amount
     */
    SimulationResult simulateCycle(const Cycle& cycle, double startAmount) const;

    std::vector<std::shared_ptr<Pool>> pools_;                      // All pools for price mapping
    std::unordered_map<std::string, double> tokenUsdPrices_;        // Token USD prices (median aggregated)
};
