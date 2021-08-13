#include <cstddef>
#include <cstdio>
#include <iostream>

#include <pico_pytagoras/q.hpp>
using namespace ppy;

template <uint8_t Db, typename BaseT>
auto operator<<(std::ostream &s, Q<Db, BaseT> const &val) -> std::ostream & {
    using q = Q<Db, BaseT>;
    constexpr auto integer_mask = q::integer_mask;
    s << ((val.value & integer_mask) >> Db) << ":" << (val.value & ~integer_mask);
    return s;
}

#include <catch2/catch_test_macros.hpp>

using q4d27 = ppy::Q<27, int32_t>;

TEST_CASE("Q int32_t: construction is zero", "[fixedpoint]") {
    q1d30 a;

    CHECK(a.value == 0);
}

TEST_CASE("Q int32_t: construction from integer", "[fixedpoint]") {
    q1d30 a{1};
    CHECK(a.value == 1 << 30);

    q1d30 b{-1};
    CHECK(b.value == -(1 << 30));

    q1d30 c{2};
    CHECK(c.value == 0);

    q4d27 d{7};
    CHECK(d.value == (7 << 27));

    q4d27 e{16};
    CHECK(e.value == 0);

    q4d27 f{17};
    CHECK(f.value == 1 << 27);
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

    // This will disappear due to truncation.
    auto c = a * b;
    CHECK(c.value == 0);

    // Lowest bits of integer portion is safe.
    q5d26 d{4};
    auto e = d * d;
    CHECK(e == q5d26{16});
    // Saturates
    e = e * e;
    CAPTURE(e.value);
    CAPTURE(q5d26::max_val.value);
    CHECK(e == q5d26::max_val);

    // Todo: add tests for saturation.
}

TEST_CASE("saturate", "[fixedpoint]") {
    using int32_limits = std::numeric_limits<int32_t>;
    int32_t a = details::saturate<int32_t>((int64_t)int32_limits::max() + 1);
    CHECK(a == int32_limits::max());
    a = details::saturate<int32_t>((int64_t)int32_limits::min() - 1);
    CHECK(a == int32_limits::min());
    a = details::saturate<int32_t>((int64_t)1234);
    CHECK(a == 1234);
}