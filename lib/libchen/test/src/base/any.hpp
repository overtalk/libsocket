/**
 * Created by Jian Chen
 * @since  2016.05.27
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

#include <chen/chen.hpp>
#include <gtest/gtest.h>

TEST(BaseAnyTest, General)
{
    // empty
    chen::any any;

    EXPECT_TRUE(any.empty());

    // non-empty
    any = 15;

    EXPECT_FALSE(any.empty());

    // integer
    int a = any;

    EXPECT_NE(0, a);
    EXPECT_NE(10, a);

    EXPECT_EQ(15, a);

    // string
    any = std::string("chensoft.com");
    std::string s = any;

    EXPECT_NE(std::string("chensoft"), s);
    EXPECT_EQ(std::string("chensoft.com"), s);
}