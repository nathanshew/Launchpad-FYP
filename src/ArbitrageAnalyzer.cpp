#include "ArbitrageAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace {

// Helper function to calculate median from a vector of values
// Used for robust USD price estimation from multiple pool candidates
double median(std::vector<double> values) {
    if (values.empty()) {
        return 0.0;
    }
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 1) {
        return values[n / 2];  // Odd count: return middle element
    }
    return 0.5 * (values[n / 2 - 1] + values[n / 2]);  // Even count: average of two middle elements
}

}  // namespace

ArbitrageAnalyzer::ArbitrageAnalyzer(std::vector<std::shared_ptr<Pool>> pools) 
    : pools_(std::move(pools)), tokenUsdPrices_(buildTokenUsdPriceMap()) {}

std::vector<ArbitrageOpportunity> ArbitrageAnalyzer::analyzeCycles(const std::vector<Cycle>& cycles) const {
    std::vector<ArbitrageOpportunity> opportunities;

    for (const auto& cycle : cycles) {
        // Validate cycle structure
        if (cycle.edges.empty() || cycle.tokenPath.size() < 2) {
            continue;
        }

        // Get USD price for the starting token (needed to convert profit to USD)
        const std::string& startToken = cycle.tokenPath.front();
        auto priceIt = tokenUsdPrices_.find(startToken);
        if (priceIt == tokenUsdPrices_.end() || priceIt->second <= 0.0) {
            continue;  // Skip if we don't have a valid USD price
        }

        // Get the reserve of the input token in the first pool
        // This determines our trade size options (as % of this reserve)
        const auto& firstEdge = cycle.edges.front();
        double firstReserveIn = firstEdge.pool->getReserveForToken(firstEdge.fromTokenId);
        if (firstReserveIn <= 0.0) {
            continue;
        }

        // Test 3 trade sizes: 1%, 10%, 20% of first pool's input reserve
        // Smaller trades have less slippage but lower absolute profit
        // Larger trades have more slippage but potentially higher absolute profit
        std::vector<double> sizeFractions{0.01, 0.10, 0.20};
        bool foundValid = false;
        ArbitrageOpportunity best;

        for (double frac : sizeFractions) {
            double startAmount = firstReserveIn * frac;
            auto sim = simulateCycle(cycle, startAmount);
            if (!sim.valid) {
                continue;  // This trade size fails viability check
            }

            // Calculate profit in tokens and convert to USD
            double profitToken = sim.finalAmount - startAmount;
            double profitUsd = profitToken * priceIt->second;
            if (profitUsd <= 0.0) {
                continue;  // Not profitable at this trade size
            }

            double roi = (profitToken / startAmount) * 100.0;
            
            // Keep the trade size with maximum USD profit (our ranking metric)
            if (!foundValid || profitUsd > best.profitUsd) {
                foundValid = true;
                best.cycle = cycle;
                best.optimalTradeSize = startAmount;
                best.profitUsd = profitUsd;
                best.percentageReturn = roi;
                best.amounts = std::move(sim.amounts);
                best.startTokenId = startToken;
            }
        }

        if (foundValid) {
            opportunities.push_back(std::move(best));
        }
    }

    // Sort opportunities by USD profit (descending)
    std::sort(opportunities.begin(), opportunities.end(), [](const ArbitrageOpportunity& a, const ArbitrageOpportunity& b) {
        return a.profitUsd > b.profitUsd;
    });

    return opportunities;
}

const std::unordered_map<std::string, double>& ArbitrageAnalyzer::tokenUsdPrices() const { return tokenUsdPrices_; }

std::unordered_map<std::string, double> ArbitrageAnalyzer::buildTokenUsdPriceMap() const {
    // Collect multiple price estimates for each token from different pools
    std::unordered_map<std::string, std::vector<double>> candidates;

    for (const auto& pool : pools_) {
        if (pool->reserveUsd() <= 0.0) {
            continue;  // Skip pools without USD liquidity data
        }

        // Estimate token0 price: assume pool reserves split 50/50 in USD value
        // tokenPrice = totalUSD / (2 * tokenReserve)
        if (pool->reserve0() > 0.0) {
            double p0 = pool->reserveUsd() / (2.0 * pool->reserve0());
            if (std::isfinite(p0) && p0 > 0.0) {
                candidates[pool->token0()->id()].push_back(p0);
            }
        }

        // Same for token1
        if (pool->reserve1() > 0.0) {
            double p1 = pool->reserveUsd() / (2.0 * pool->reserve1());
            if (std::isfinite(p1) && p1 > 0.0) {
                candidates[pool->token1()->id()].push_back(p1);
            }
        }
    }

    // Use median of all estimates to reduce impact of outlier pools
    std::unordered_map<std::string, double> out;
    for (auto& kv : candidates) {
        double m = median(kv.second);
        if (m > 0.0) {
            out.emplace(kv.first, m);
        }
    }
    return out;
}

ArbitrageAnalyzer::SimulationResult ArbitrageAnalyzer::simulateCycle(const Cycle& cycle, double startAmount) const {
    SimulationResult result;
    if (startAmount <= 0.0) {
        return result;  // Invalid starting amount
    }

    // Create LOCAL reserve copies for each pool in the cycle
    // CRITICAL: Never mutate the shared Pool objects' reserves
    std::unordered_map<std::string, std::pair<double, double>> localReserves;
    for (const auto& edge : cycle.edges) {
        localReserves.emplace(edge.pool->id(), std::make_pair(edge.pool->reserve0(), edge.pool->reserve1()));
    }

    double amount = startAmount;
    result.amounts.push_back(amount);

    // Execute each swap in the cycle
    for (const auto& edge : cycle.edges) {
        auto it = localReserves.find(edge.pool->id());
        if (it == localReserves.end()) {
            return result;  // Should never happen, but safety check
        }

        auto& reserves = it->second;
        double reserveIn = edge.pool->isToken0(edge.fromTokenId) ? reserves.first : reserves.second;
        if (reserveIn <= 0.0) {
            return result;  // Invalid reserve
        }

        // VIABILITY CHECK from PRD: trade size must be <= 20% of reserve
        // Prevents unrealistic trades that would cause extreme slippage
        if ((amount / reserveIn) > 0.20) {
            return result;  // Fails viability check
        }

        // Simulate the swap with local reserves
        // This updates the reserves pair and returns the output amount
        amount = edge.pool->simulateSwap(amount, edge.fromTokenId, reserves.first, reserves.second);
        if (amount <= 0.0 || !std::isfinite(amount)) {
            return result;  // Swap failed
        }

        result.amounts.push_back(amount);
    }

    // Success: we have valid amounts for the full cycle
    result.valid = true;
    result.finalAmount = amount;
    return result;
}
