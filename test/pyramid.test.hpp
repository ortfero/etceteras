#pragma once


#include "doctest.h"

#include <etceteras/pyramid.hpp>


TEST_SUITE("pyramid") {
    
    
    SCENARIO("constructed by default") {
        auto target = etceteras::pyramid<int>{};
        REQUIRE_EQ(target.size(), 0);
        REQUIRE_EQ(target.capacity(), 0);
        REQUIRE(target.empty());
        REQUIRE_EQ(target.begin(), target.end());
    }
    
    
    SCENARIO("insert item") {
        auto target = etceteras::pyramid<int>{};
        auto it = target.insert(-1);
        REQUIRE_EQ(target.size(), 1);
        REQUIRE_EQ(target.capacity(), etceteras::pyramid<int>::factor);
        REQUIRE(!target.empty());
        REQUIRE_NE(target.begin(), target.end());
        REQUIRE_EQ(target.begin(), it);
        REQUIRE_EQ(*it, -1);
        *it = 0;
        REQUIRE_EQ(*it, 0);
    }


    SCENARIO("erase item") {
        auto target = etceteras::pyramid<int>{};
        auto it = target.insert(-1);
        it = target.erase(it);
        REQUIRE_EQ(target.size(), 0);
        REQUIRE_EQ(target.capacity(), etceteras::pyramid<int>::factor);
        REQUIRE(target.empty());
        REQUIRE_EQ(target.begin(), target.end());
        REQUIRE_EQ(it, target.end());
    }
    
    
    SCENARIO("copy constructor") {
        auto source = etceteras::pyramid<int>{};
        source.insert(-1);
        auto const target = etceteras::pyramid<int>{source};
        REQUIRE_EQ(target.size(), 1);
        REQUIRE_EQ(*target.begin(), -1);
    }
    
    
    SCENARIO("assignment") {
        auto source = etceteras::pyramid<int>{};
        source.insert(-1);
        auto target = etceteras::pyramid<int>{};
        target = source;
        REQUIRE_EQ(target.size(), 1);
        REQUIRE_EQ(*target.begin(), -1);
    }
    
    
    SCENARIO("move constructor") {
        auto source = etceteras::pyramid<int>{};
        source.insert(-1);
        auto const target = etceteras::pyramid<int>{std::move(source)};
        REQUIRE_EQ(target.size(), 1);
        //REQUIRE_EQ(*target.begin(), -1);
        REQUIRE(source.empty());
    }
    
    
    SCENARIO("preserve insertion order") {
        auto target = etceteras::pyramid<int>{};
        target.insert(1);
        target.insert(2);
        target.insert(3);
        REQUIRE_EQ(target.size(), 3);
        auto it = target.begin();
        REQUIRE_EQ(*it, 1);
        ++it;
        REQUIRE_EQ(*it, 2);
        ++it;
        REQUIRE_EQ(*it, 3);
        ++it;
        REQUIRE_EQ(it, target.end());
    }
    
    
    SCENARIO("erase from the middle") {
        auto target = etceteras::pyramid<int>{};
        target.insert(1);
        auto it = target.insert(2);
        target.insert(3);
        it = target.erase(it);
        REQUIRE_EQ(target.size(), 2);
        REQUIRE_EQ(*it, 3);
        REQUIRE_EQ(*target.begin(), 1);
    }
    
    
    SCENARIO("insert several items") {
        auto target = etceteras::pyramid<int>{};
        auto const factor = etceteras::pyramid<int>::factor;
        for(auto i = 1u; i != factor + 2; ++i)
            target.insert(i);
        REQUIRE_EQ(target.size(), factor + 1);
        REQUIRE_EQ(target.capacity(), factor * factor);
        auto it = target.end();
        --it;
        REQUIRE_EQ(*it, factor + 1);
    }


    
    
}
