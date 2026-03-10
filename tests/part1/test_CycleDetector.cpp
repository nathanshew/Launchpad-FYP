#include <gtest/gtest.h>

#include <memory>

#include "CycleDetector.h"
#include "Graph.h"

TEST(CycleDetectorTest, SimpleTriangleCycle) {
    auto a = std::make_shared<Token>("A", 18, "A");
    auto b = std::make_shared<Token>("B", 18, "B");
    auto c = std::make_shared<Token>("C", 18, "C");

    auto ab = std::make_shared<Pool>("ab", a, b, 100.0, 100.0, 200.0);
    auto bc = std::make_shared<Pool>("bc", b, c, 100.0, 100.0, 200.0);
    auto ca = std::make_shared<Pool>("ca", c, a, 100.0, 100.0, 200.0);

    Graph graph;
    graph.addPool(ab);
    graph.addPool(bc);
    graph.addPool(ca);

    CycleDetector detector;
    auto cycles = detector.findCycles(graph, 5);

    EXPECT_FALSE(cycles.empty());
}

TEST(CycleDetectorTest, NoFalseCycles) {
    auto a = std::make_shared<Token>("A", 18, "A");
    auto b = std::make_shared<Token>("B", 18, "B");
    auto c = std::make_shared<Token>("C", 18, "C");

    auto ab = std::make_shared<Pool>("ab", a, b, 100.0, 100.0, 200.0);
    auto bc = std::make_shared<Pool>("bc", b, c, 100.0, 100.0, 200.0);

    Graph graph;
    graph.addPool(ab);
    graph.addPool(bc);

    CycleDetector detector;
    auto cycles = detector.findCycles(graph, 5);

    EXPECT_TRUE(cycles.empty());
}

TEST(CycleDetectorTest, MaxDepthRespected) {
    auto a = std::make_shared<Token>("A", 18, "A");
    auto b = std::make_shared<Token>("B", 18, "B");
    auto c = std::make_shared<Token>("C", 18, "C");
    auto d = std::make_shared<Token>("D", 18, "D");

    auto ab = std::make_shared<Pool>("ab", a, b, 100.0, 100.0, 200.0);
    auto bc = std::make_shared<Pool>("bc", b, c, 100.0, 100.0, 200.0);
    auto cd = std::make_shared<Pool>("cd", c, d, 100.0, 100.0, 200.0);
    auto da = std::make_shared<Pool>("da", d, a, 100.0, 100.0, 200.0);

    Graph graph;
    graph.addPool(ab);
    graph.addPool(bc);
    graph.addPool(cd);
    graph.addPool(da);

    CycleDetector detector;
    auto cyclesDepth3 = detector.findCycles(graph, 3);
    auto cyclesDepth4 = detector.findCycles(graph, 4);

    EXPECT_TRUE(cyclesDepth3.empty());
    EXPECT_FALSE(cyclesDepth4.empty());
}
