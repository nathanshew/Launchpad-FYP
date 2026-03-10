#include <gtest/gtest.h>

#include <memory>

#include "Graph.h"

TEST(GraphTest, AddPoolCreatesEdges) {
    auto t0 = std::make_shared<Token>("A", 18, "A");
    auto t1 = std::make_shared<Token>("B", 6, "B");
    auto pool = std::make_shared<Pool>("p", t0, t1, 10.0, 1000.0, 1000.0);

    Graph graph;
    graph.addPool(pool);

    EXPECT_EQ(graph.getEdgesFrom("A").size(), 1U);
    EXPECT_EQ(graph.getEdgesFrom("B").size(), 1U);
    EXPECT_EQ(graph.getEdgesFrom("A")[0].toTokenId, "B");
    EXPECT_EQ(graph.getEdgesFrom("B")[0].toTokenId, "A");
}
