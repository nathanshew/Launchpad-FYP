#include <gtest/gtest.h>

#include "Token.h"

TEST(TokenTest, ConstructorAndGetters) {
    Token token("0xabc", 18, "ABC");
    EXPECT_EQ(token.id(), "0xabc");
    EXPECT_EQ(token.decimals(), 18);
    EXPECT_EQ(token.symbol(), "ABC");
}
