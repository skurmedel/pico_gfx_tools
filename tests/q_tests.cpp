#include <cstddef>
#include <cstdio>

#include <catch2/catch_test_macros.hpp>

#include <pico_pytagoras/q.hpp>

using namespace ppy;

using q4d27 = ppy::Q<27, int32_t>;

TEST_CASE("Q int32_t: construction is zero", "[fixedpoint]") {
    q1d30 a;

    CHECK(a.value == 0);
}

TEST_CASE("Q int32_t: construction from integer", "[fixedpoint]") {
    q1d30 a{1};
    CHECK(a.value == 1<<30);

    q1d30 b{-1};
    CHECK(b.value == -(1<<30));

    q1d30 c{2};
    CHECK(c.value == 0);

    q4d27 d{7};
    CHECK(d.value == (7<<27));

    q4d27 e{16};
    CHECK(e.value == 0);

    q4d27 f{17};
    CHECK(f.value == 1<<27);
}

TEST_CASE("Q int32_t: additive group", "[fixedpoint]") {
    q1d30 a;
    a.value = 1;
    q1d30 b;
    b.value = 2;

    auto c = a + b;
    CHECK(c.value == 3);

    c = b - a;
    CHECK(c.value == 1);

    // Wraps around upwards.
    auto almost_max = q1d30::max_val - q1d30::from_raw(1);
    c = almost_max + q1d30::from_raw(2);
    CHECK(c.value == q1d30::min_val.value);

    // Wraps around downwards.
    auto almost_min = q1d30::min_val + q1d30::from_raw(1);
    c = almost_min - q1d30::from_raw(2);
    CHECK(c.value == q1d30::max_val.value);
}

TEST_CASE("Q int32_t: multiplicative group", "[fixedpoint]") {
    q1d30 a;
    a.value = 1;
    q1d30 b;
    b.value = 2;

    auto c = a * b;
    CHECK(c.value == 2);

    a.value = -3;
    c = a * b;
    CHECK(c.value == -6);

    // As Two's complement is assumed:
    // Wraps around upwards.
    auto almost_max = q1d30::max_val;
    c = almost_max * q1d30::from_raw(2);
    CHECK(c.value == -2);

    // Wraps around downwards.
    auto almost_min = q1d30::min_val;
    c = almost_min * q1d30::from_raw(2);
    CHECK(c.value == 0);
}