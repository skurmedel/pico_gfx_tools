#pragma once

#include <cmath>
#include <limits>
#include <stdint.h>
#include <type_traits>

namespace ppy {

template <typename T>
struct next_int {};
template <>
struct next_int<int8_t> {
    using type = int16_t;
};
template <>
struct next_int<int16_t> {
    using type = int32_t;
};
template <>
struct next_int<int32_t> {
    using type = int64_t;
};
template <typename T>
using next_int_t = typename next_int<T>::type;

/**
 * A signed fixed point type, based on a signed integer. DecimalBits implicitly decides the number
 * of integer bits: integer bits = 32 - 1 - DecimalBits
 *
 * The Q type is mainly intended for graphics development. We'll list some of the design decisions
 * and their rationale now.
 *
 * Overflow/underflow has wrap-around behaviour by default
 * -------------------------------------------------------
 * ARMv6-M as featured on Cortex-M0 and Cortex-M0+, does not feature an instruction to add with
 * saturation. This is the case for many other non-ARM microprocessors as well.
 *
 * We could then check for overflow, but this costs at least a cycle. So the fastest, most
 * performant mode is to simply wrap-around on overflow, and ignore the overflow flag.
 *
 * As this is mainly a GFX geared lib, overflow bugs on things like coordinates usually result in a
 * erronous "look", i.e things look bad or hilarious. If you are building a rocket or a cancer
 * treatment device, you shouldn't use this type. And if you needed to be told that, you shouldn't
 * build a rocket or a cancer treatment device.
 *
 * When the behaviour is absolutely needed, there are routines that do saturating and checked
 * arithmetic.
 *
 * Two's complement assumed
 * ------------------------
 * We're not really intending this for work on any system that doesn't do this.
 *
 */
template <uint8_t DecimalBits, typename BaseT = int32_t>
struct Q {
    using Self = Q<DecimalBits, BaseT>;
    static_assert(std::is_signed_v<BaseT>, "Base type needs to be signed.");
    static_assert(std::is_integral_v<BaseT>, "Base type needs to be an integer.");

    using base_t = BaseT;
    using base_t_limits = std::numeric_limits<BaseT>;
    using unsigned_base_t = std::make_unsigned_t<base_t>;

    static_assert(DecimalBits <= base_t_limits::digits,
                  "Can't have more bits than fit in the base type.");
    static constexpr inline int8_t sign_bits = 1;
    static constexpr inline int8_t decimal_bits = DecimalBits;
    static constexpr inline int8_t integer_bits = base_t_limits::digits - decimal_bits;
    static constexpr inline Self max_val = Self::from_raw(base_t_limits::max());
    static constexpr inline Self min_val = Self::from_raw(base_t_limits::min());

    static constexpr inline base_t integer_mask_unshifted = ((1 << integer_bits) - 1);
    static constexpr inline base_t integer_mask = (integer_mask_unshifted << decimal_bits);

    constexpr Q() : value(0) {}
    /**
     * Constructs a value from an integer. Note that it may not fit, and if so, gets truncated.
     */
    constexpr explicit Q(base_t integer)
        : value((integer & (integer_mask_unshifted)) << decimal_bits) {
        if (std::signbit(integer))
            value = -value;
    }
    constexpr Q(Self const &) = default;
    constexpr Q(Self &&) = default;
    constexpr auto operator=(Self const &) -> Self & = default;
    constexpr auto operator=(Self &&) -> Self & = default;

    static constexpr auto from_raw(base_t const &b) -> Self {
        auto q = Self();
        q.value = b;
        return q;
    }

    base_t value;
};

template <uint8_t Db, typename BaseT>
auto operator+(Q<Db, BaseT> const &a, Q<Db, BaseT> const &b) -> Q<Db, BaseT> {
    using unsigned_base_t = typename Q<Db, BaseT>::unsigned_base_t;
    // Note: The conversion from this (possibly too large) value, is implementation defined, so we should
    // avoid UB. Hopefully the implementation wraps.
    return Q<Db, BaseT>::from_raw((unsigned_base_t)a.value + (unsigned_base_t)b.value);
}

template <uint8_t Db, typename BaseT>
auto operator-(Q<Db, BaseT> const &a, Q<Db, BaseT> const &b) -> Q<Db, BaseT> {
    using unsigned_base_t = typename Q<Db, BaseT>::unsigned_base_t;
    // See note on operator+
    return Q<Db, BaseT>::from_raw((unsigned_base_t)a.value - (unsigned_base_t)b.value);
}

template <uint8_t Db, typename BaseT>
auto operator*(Q<Db, BaseT> const &a, Q<Db, BaseT> const &b) -> Q<Db, BaseT> {
    using unsigned_base_t = typename Q<Db, BaseT>::unsigned_base_t;
    // See note on operator+
    return Q<Db, BaseT>::from_raw((unsigned_base_t)a.value * (unsigned_base_t)b.value);
}

/**
 * Useful for Mandelbrot & Julia Set renders, where the interesting domain is from about
 * [-1.5, +1.5].
 */
using q1d30 = Q<30, int32_t>;
/**
 * General purpose graphics type, domain is approx (-32, +32), non-inclusive, and smallest positive
 * integer is approx. 1.5 * 10^-8.
 */
using q5d26 = Q<22, int32_t>;

} // namespace ppy
