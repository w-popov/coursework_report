#include <vector> 
#include "catch.hpp"

TEST_CASE("TEST: test")
{
    SECTION("Section 1: ")
    {
        std::vector<int> base{20, 19, 4,  3,  2};
        std::vector<int> expected{1,  2,  3,  4};
        
        REQUIRE(base == expected);
    }
}

