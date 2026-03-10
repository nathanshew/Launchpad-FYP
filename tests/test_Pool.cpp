#include <gtest/gtest.h>

#include <cmath>
#include <memory>

#include "Pool.h"

TEST(PoolTest, SwapCalculationSmallAmount) {
    auto t0 = std::make_shared<Token>("A", 18, "A");
    auto t1 = std::make_shared<Token>("B", 6, "B");
    Pool pool("p", t0, t1, 100.0, 200000.0, 400000.0);

    double r0 = pool.reserve0();
    double r1 = pool.reserve1();
    double out = pool.simulateSwap(1.0, "A", r0, r1);

    EXPECT_GT(out, 1974.0);
    EXPECT_LT(out, 1975.0);
}

TEST(PoolTest, SwapCalculationLargeAmountHasMoreSlippage) {
    auto t0 = std::make_shared<Token>("A", 18, "A");
    auto t1 = std::make_shared<Token>("B", 6, "B");
    Pool pool("p", t0, t1, 100.0, 200000.0, 400000.0);

    double r0s = pool.reserve0();
    double r1s = pool.reserve1();
    double small = pool.simulateSwap(1.0, "A", r0s, r1s);

    double r0l = pool.reserve0();
    double r1l = pool.reserve1();
    double large = pool.simulateSwap(20.0, "A", r0l, r1l);

    EXPECT_GT(large, small);
    EXPECT_LT(large / 20.0, small / 1.0);
}

TEST(PoolTest, ReserveUpdate) {
    auto t0 = std::make_shared<Token>("A", 18, "A");
    auto t1 = std::make_shared<Token>("B", 6, "B");
    Pool pool("p", t0, t1, 100.0, 200000.0, 400000.0);

    double r0 = pool.reserve0();
    double r1 = pool.reserve1();
    double out = pool.simulateSwap(2.0, "A", r0, r1);

    EXPECT_GT(out, 0.0);
    EXPECT_DOUBLE_EQ(r0, 102.0);
    EXPECT_LT(r1, 200000.0);
}

TEST(PoolTest, OutputRoundingByTokenDecimals) {
    auto t0 = std::make_shared<Token>("A", 18, "A");
    auto t1 = std::make_shared<Token>("B", 2, "B");
    Pool pool("p", t0, t1, 100.0, 1234.56, 2000.0);

    double r0 = pool.reserve0();
    double r1 = pool.reserve1();
    double out = pool.simulateSwap(0.3333333333, "A", r0, r1);

    double scaled = out * 100.0;
    EXPECT_DOUBLE_EQ(scaled, std::floor(scaled));
}

TEST(PoolTest, MultiHopRoundingPropagation) {
    auto a = std::make_shared<Token>("A", 18, "A");
    auto b = std::make_shared<Token>("B", 2, "B");
    auto c = std::make_shared<Token>("C", 3, "C");

    Pool ab("ab", a, b, 100.0, 1234.56, 2000.0);
    Pool bc("bc", b, c, 500.0, 1000.0, 2000.0);

    double abR0 = ab.reserve0();
    double abR1 = ab.reserve1();
    double hop1Out = ab.simulateSwap(0.3333333333, "A", abR0, abR1);

    // Hop1 output token is B with 2 decimals, so it must be quantized to cents.
    double hop1Scaled = hop1Out * 100.0;
    EXPECT_DOUBLE_EQ(hop1Scaled, std::floor(hop1Scaled));

    double bcR0 = bc.reserve0();
    double bcR1 = bc.reserve1();
    double hop2Out = bc.simulateSwap(hop1Out, "B", bcR0, bcR1);

    EXPECT_GT(hop2Out, 0.0);
}
