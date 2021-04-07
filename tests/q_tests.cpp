#include <cstddef>
#include <cstdio>

#include <catch2/catch_test_macros.hpp>

#include <pico_pytagoras/q.hpp>

TEST_CASE("Q int32_t: construction is zero", "[fixedpoint]") {
    ppy::q1d30 a;

    CHECK(a.value == 0);
}

TEST_CASE("Q int32_t: construction from integer", "[fixedpoint]") {
    ppy::q1d30 a{1};
    CHECK(a.value == 1<<30);

    ppy::q1d30 b{-1};
    CHECK(b.value == -(1<<30));

    ppy::q1d30 c{2};
    CHECK(c.value == 0);

    ppy::Q<27> d{7};
    CHECK(d.value == (7<<27));

    ppy::Q<27> e{16};
    CHECK(e.value == 0);

    ppy::Q<27> f{17};
    CHECK(f.value == 1<<27);
}

TEST_CASE("Q int32_t: additive group", "[fixedpoint]") {
    ppy::q1d30 a;
    a.value = 1;
    ppy::q1d30 b;
    b.value = 2;

    auto c = a + b;
    CHECK(c.value == 3);

    c = b - a;
    CHECK(c.value == 1);

    // Wraps around upwards.
    auto almost_max = ppy::q1d30::max_val - ppy::q1d30::from_raw(1);
    c = almost_max + ppy::q1d30::from_raw(2);
    CHECK(c.value == ppy::q1d30::min_val.value);

    // Wraps around downwards.
    auto almost_min = ppy::q1d30::min_val + ppy::q1d30::from_raw(1);
    c = almost_min - ppy::q1d30::from_raw(2);
    CHECK(c.value == ppy::q1d30::max_val.value);
}

TEST_CASE("Q int32_t: multiplicative group", "[fixedpoint]") {
    ppy::q1d30 a;
    a.value = 1;
    ppy::q1d30 b;
    b.value = 2;

    auto c = a * b;
    CHECK(c.value == 2);

    a.value = -3;
    c = a * b;
    CHECK(c.value == -6);

    // As Two's complement is assumed:
    // Wraps around upwards.
    auto almost_max = ppy::q1d30::max_val;
    c = almost_max * ppy::q1d30::from_raw(2);
    CHECK(c.value == -2);

    // Wraps around downwards.
    auto almost_min = ppy::q1d30::min_val;
    c = almost_min * ppy::q1d30::from_raw(2);
    CHECK(c.value == 0);
}