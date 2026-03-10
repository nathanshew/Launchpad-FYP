#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "CycleDetector.h"

struct ArbitrageOpportunity {
    Cycle cycle;
    double optimalTradeSize = 0.0;
    double profitUsd = 0.0;
    double percentageReturn = 0.0;
    std::vector<double> amounts;
    std::string startTokenId;
};

class ArbitrageAnalyzer {
public:
    explicit ArbitrageAnalyzer(std::vector<std::shared_ptr<Pool>> pools);

    std::vector<ArbitrageOpportunity> analyzeCycles(const std::vector<Cycle>& cycles) const;
    const std::unordered_map<std::string, double>& tokenUsdPrices() const;

private:
    struct SimulationResult {
        bool valid = false;
        std::vector<double> amounts;
        double finalAmount = 0.0;
    };

    std::unordered_map<std::string, double> buildTokenUsdPriceMap() const;
    SimulationResult simulateCycle(const Cycle& cycle, double startAmount) const;

    std::vector<std::shared_ptr<Pool>> pools_;
    std::unordered_map<std::string, double> tokenUsdPrices_;
};
