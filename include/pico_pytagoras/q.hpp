#pragma once

#include <cmath>
#include <limits>
#include <stdint.h>
#include <type_traits>

namespace ppy {

namespace details {
template <typename T>
struct larger_int {};
template <>
struct larger_int<int8_t> {
    using type = int16_t;
};
template <>
struct larger_int<int16_t> {
    using type = int32_t;
};
template <>
struct larger_int<int32_t> {
    using type = int64_t;
};
template <typename T>
using larger_int_t = typename larger_int<T>::type;

template <typename T>
struct lesser_int {};
template <>
struct lesser_int<int16_t> {
    using type = int8_t;
};
template <>
struct lesser_int<int32_t> {
    using type = int16_t;
};
template <>
struct lesser_int<int64_t> {
    using type = int32_t;
};
template <typename T>
using lesser_int_t = typename lesser_int<T>::type;

template <typename IntT>
auto saturate(larger_int_t<IntT> val) -> IntT {
    using int_limit = std::numeric_limits<IntT>;
    using larger_int_limit = std::numeric_limits<larger_int_t<IntT>>;
    constexpr static auto minval = int_limit::min();
    constexpr static auto maxval = int_limit::max();

    if (val > maxval)
        return maxval;
    else if (val < minval)
        return minval;
    return (IntT) val;
}
} // namespace details

/**
 * A signed fixed point type, based on a signed integer. DecimalBits implicitly decides the number
 * of integer bits: integer bits = 32 - 1 - DecimalBits
 *
 * The Q type is mainly intended for graphics development. We'll list some of the design decisions
 * and their rationale now.
 *
 * Overflow/underflow saturates
 * ----------------------------
 * For simplicity's sake, and to avoid surprising behaviour compared to floating point, this class
 * have saturating arithmetic by default. This costs some cycles compared to the wrap-around 
 * behaviour but it is still much faster than floating point on platforms without an FPU.
 * 
 * Saturating means that if an overflow were to happen, the operations result is the maximum value.
 * 
 * Compared to a floating point type, this class does not have the fp-concept of "infinity".
 * 
 * Wrap-around operators will be added later, and they can be used when it's really needed.
 *
 * Two's complement assumed
 * ------------------------
 * We're not really intending this for work on any system that doesn't do this. In C++20, it seems
 * that two's complement will become a requirement for signed integers.
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

    constexpr auto operator==(Self const &b) const -> bool { return value == b.value; }
    constexpr auto operator!=(Self const &b) const -> bool { return value != b.value; }

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
    // Note: The conversion from this (possibly too large) value, is implementation defined, so we
    // should avoid UB. Hopefully the implementation wraps.
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
    using larger_base_t = details::larger_int_t<BaseT>;
    using unsigned_larger_base_t = std::make_unsigned_t<larger_base_t>;
    using unsigned_base_t = typename Q<Db, BaseT>::unsigned_base_t;

    unsigned_larger_base_t c = (unsigned_larger_base_t)a.value * (unsigned_larger_base_t)b.value;
    // Normalize.
    // Todo: This will truncate. Investigate if rounding is worthwhile.
    c >>= Db;
    // See note on operator+
    return Q<Db, BaseT>::from_raw(details::saturate<BaseT>((larger_base_t) c));
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
using q5d26 = Q<26, int32_t>;

} // namespace ppy
