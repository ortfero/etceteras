#pragma once


#include "doctest.h"

#include <etceteras/expected.hpp>


TEST_SUITE("expected") {
    
    SCENARIO("constructed from value") {
        auto const target = etceteras::expected<int, int>{42};
        REQUIRE(!!target);
        REQUIRE_EQ(*target, 42);
        REQUIRE(target.has_value());
        REQUIRE_EQ(target.value(), 42);
    }
    
    
    SCENARIO("constructed from error") {
        auto const target = etceteras::expected<int, int>{etceteras::make_unexpected(42)};
        REQUIRE(!target);
        REQUIRE(!target.has_value());
        REQUIRE_EQ(target.error(), 42);
    }
    
    SCENARIO("constructed from non-default-constructed") {
        struct data {
            data() = delete;
            data(int) { }
        };
        auto const target = etceteras::expected<data, int>{data{42}};
        REQUIRE(!!target);
    }
    
    
    SCENARIO("constructed from movable-only") {
        struct data {
            data() = delete;
            data(int) { }
            data(data const&) = delete;
            data(data&&) { }
        };
        auto const target = etceteras::expected<data, int>{data{42}};
        REQUIRE(!!target);
    }
}
