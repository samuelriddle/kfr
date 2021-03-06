/**
 * Copyright (C) 2016 D Levin (http://www.kfrlib.com)
 * This file is part of KFR
 *
 * KFR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KFR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KFR.
 *
 * If GPL is not suitable for your project, you must purchase a commercial license to use KFR.
 * Buying a commercial license is mandatory as soon as you develop commercial activities without
 * disclosing the source code of your own applications.
 * See http://www.kfrlib.com for details.
 */
#pragma once

#include "../base/log_exp.hpp"
#include "../base/sin_cos.hpp"
#include "../base/sqrt.hpp"
#include "../base/vec.hpp"
#include "../expressions/pointer.hpp"

#pragma clang diagnostic push
#if CID_HAS_WARNING("-Winaccessible-base")
#pragma clang diagnostic ignored "-Winaccessible-base"
#endif

namespace kfr
{

enum class window_type
{
    rectangular     = 1,
    triangular      = 2,
    bartlett        = 3,
    cosine          = 4,
    hann            = 5,
    bartlett_hann   = 6,
    hamming         = 7,
    bohman          = 8,
    blackman        = 9,
    blackman_harris = 10,
    kaiser          = 11,
    flattop         = 12,
    gaussian        = 13,
    lanczos         = 14,
};

template <window_type type>
using cwindow_type_t = cval_t<window_type, type>;

template <window_type type>
constexpr cwindow_type_t<type> cwindow_type{};

enum class window_symmetry
{
    periodic,
    symmetric
};

namespace internal
{

template <typename T>
constexpr T bessel_coef[] = { T(0.25),
                              T(0.027777777777777776236),
                              T(0.0017361111111111110147),
                              T(6.9444444444444444384e-005),
                              T(1.9290123456790123911e-006),
                              T(3.9367598891408417495e-008),
                              T(6.1511873267825652335e-010),
                              T(7.5940584281266239246e-012),
                              T(7.5940584281266233693e-014),
                              T(6.2760813455591932909e-016),
                              T(4.3583898233049949985e-018),
                              T(2.5789288895295827557e-020),
                              T(1.3157800456783586208e-022),
                              T(5.8479113141260384983e-025),
                              T(2.2843403570804837884e-027),
                              T(7.904291893012054025e-030),
                              T(2.4395962632753252792e-032),
                              T(6.75788438580422547e-035),
                              T(1.689471096451056426e-037),
                              T(3.8310002187098784929e-040),
                              T(7.9152897080782616517e-043),
                              T(1.4962740468957016443e-045),
                              T(2.5976979980828152196e-048),
                              T(4.1563167969325041577e-051),
                              T(6.1483976285983795968e-054),
                              T(8.434015951438105991e-057),
                              T(1.0757673407446563809e-059),
                              T(1.2791526049282476926e-062),
                              T(1.4212806721424974034e-065),
                              T(1.4789601166935457918e-068),
                              T(1.4442969889585408123e-071),
                              T(1.3262598613026086927e-074),
                              T(1.1472836170437790782e-077),
                              T(9.3655805472961564331e-081),
                              T(7.2265282000741942594e-084),
                              T(5.2786911614858977913e-087),
                              T(3.6556032974279072401e-090),
                              T(2.4034209713529963119e-093),
                              T(1.5021381070956226783e-096) };

template <typename T, size_t N>
KFR_INLINE vec<T, N> modzerobessel(vec<T, N> x)
{
    const vec<T, N> x_2     = x * 0.5;
    const vec<T, N> x_2_sqr = x_2 * x_2;
    vec<T, N> num           = x_2_sqr;
    vec<T, N> result;
    result = 1 + x_2_sqr;

    KFR_LOOP_UNROLL
    for (size_t i = 0; i < (sizeof(T) == 4 ? 20 : 39); i++)
    {
        result = fmadd((num *= x_2_sqr), bessel_coef<T>[i], result);
    }
    return result;
}

template <cpu_t cpu = cpu_t::native>
struct in_window : in_sin_cos<cpu>, in_log_exp<cpu>, in_select<cpu>, in_sqrt<cpu>, in_abs<cpu>
{
private:
    using in_sin_cos<cpu>::sin;
    using in_sin_cos<cpu>::cos;
    using in_sin_cos<cpu>::sinc;
    using in_log_exp<cpu>::exp;
    using in_select<cpu>::select;
    using in_sqrt<cpu>::sqrt;
    using in_abs<cpu>::abs;

public:
    template <typename T>
    struct window_linspace_0_1 : expression_linspace<T>
    {
        window_linspace_0_1(size_t size, window_symmetry symmetry)
            : expression_linspace<T>(0, 1, size, symmetry == window_symmetry::symmetric)
        {
        }
    };

    template <typename T>
    struct window_linspace_m1_1 : expression_linspace<T>
    {
        window_linspace_m1_1(size_t size, window_symmetry symmetry)
            : expression_linspace<T>(-1, 1, size, symmetry == window_symmetry::symmetric)
        {
        }
    };

    template <typename T>
    struct window_linspace_mpi_pi : expression_linspace<T>
    {
        window_linspace_mpi_pi(size_t size, window_symmetry symmetry)
            : expression_linspace<T>(-c_pi<T>, +c_pi<T>, size, symmetry == window_symmetry::symmetric)
        {
        }
    };

    template <typename T>
    struct window_linspace_m1_1_trunc : expression_linspace<T>
    {
        window_linspace_m1_1_trunc(size_t size, window_symmetry symmetry)
            : expression_linspace<T>(-T(size - 1) / size, T(size - 1) / size, size,
                                     symmetry == window_symmetry::symmetric)
        {
        }
    };

    template <typename T>
    struct window_linspace_m1_1_trunc2 : expression_linspace<T>
    {
        window_linspace_m1_1_trunc2(size_t size, window_symmetry symmetry)
            : expression_linspace<T>(symmetric_linspace,
                                     (size & 1) ? T(size - 1) / T(size + 1) : T(size - 1) / (size), size,
                                     symmetry == window_symmetry::symmetric)
        {
        }
    };

    template <typename T>
    struct expression_rectangular : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_rectangular<T>;
        expression_rectangular(size_t size, T = T(), window_symmetry = window_symmetry::symmetric)
            : m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            using UI = utype<U>;
            const vec<UI, N> i = enumerate(vec<UI, N>()) + cast<UI>(index);
            return select(i < cast<UI>(m_size), U(1), U(0));
        }
        size_t size() const { return m_size; }

    private:
        size_t m_size;
    };

    template <typename T>
    struct expression_triangular : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_triangular<T>;
        expression_triangular(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(1 - abs(linspace(cinput, index, y)));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_m1_1_trunc2<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_bartlett : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_bartlett<T>;
        expression_bartlett(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(1 - abs(linspace(cinput, index, y)));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_m1_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_cosine : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_cosine<T>;
        expression_cosine(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(sin(c_pi<T> * linspace(cinput, index, y)));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_hann : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_hann<T>;
        expression_hann(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(T(0.5) * (T(1) - cos(c_pi<T, 2> * linspace(cinput, index, y))));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_bartlett_hann : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_bartlett_hann<T>;

        expression_bartlett_hann(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            const vec<T, N> xx = linspace(cinput, index, y);
            return cast<U>(T(0.62) - T(0.48) * abs(xx - T(0.5)) + T(0.38) * cos(c_pi<T, 2> * (xx - T(0.5))));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_hamming : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_hamming<T>;
        expression_hamming(size_t size, T alpha = 0.54, window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), alpha(alpha), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(alpha - (1.0 - alpha) * (cos(c_pi<T, 2> * linspace(cinput, index, y))));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        T alpha;
        size_t m_size;
    };

    template <typename T>
    struct expression_bohman : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_bohman<T>;
        expression_bohman(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            const vec<U, N> n = abs(linspace(cinput, index, y));
            return cast<U>((T(1) - n) * cos(c_pi<T> * n) + (T(1) / c_pi<T>)*sin(c_pi<T> * n));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_m1_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_blackman : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_blackman<T>;
        expression_blackman(size_t size, T alpha = 0.16,
                            window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), a0((1 - alpha) * 0.5), a1(0.5), a2(alpha * 0.5), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            const vec<T, N> n = linspace(cinput, index, y);
            return cast<U>(a0 - a1 * cos(c_pi<T, 2> * n) + a2 * cos(c_pi<T, 4> * n));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        T a0, a1, a2;
        size_t m_size;
    };

    template <typename T>
    struct expression_blackman_harris : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_blackman_harris<T>;
        expression_blackman_harris(size_t size, T = T(),
                                   window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            const vec<T, N> n = linspace(cinput, index, y) * c_pi<T, 2>;

            return cast<U>(T(0.35875) - T(0.48829) * cos(n) + T(0.14128) * cos(2 * n) -
                           T(0.01168) * cos(3 * n));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_kaiser : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_kaiser<T>;
        expression_kaiser(size_t size, T beta = 0.5, window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), beta(beta), m(reciprocal(modzerobessel(make_vector(beta))[0])),
              m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(modzerobessel(beta * sqrt(1 - sqr(linspace(cinput, index, y)))) * m);
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_m1_1<T> linspace;
        T beta;
        T m;
        size_t m_size;
    };

    template <typename T>
    struct expression_flattop : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_flattop<T>;
        expression_flattop(size_t size, T = T(), window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            const vec<T, N> n = linspace(cinput, index, y) * c_pi<T, 2>;
            constexpr T a0 = 1;
            constexpr T a1 = 1.93;
            constexpr T a2 = 1.29;
            constexpr T a3 = 0.388;
            constexpr T a4 = 0.028;
            return cast<U>(a0 - a1 * cos(n) + a2 * cos(2 * n) - a3 * cos(3 * n) + a4 * cos(4 * n));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_0_1<T> linspace;
        size_t m_size;
    };

    template <typename T>
    struct expression_gaussian : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_gaussian<T>;

        expression_gaussian(size_t size, T alpha = 2.5, window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), alpha(alpha), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(exp(-0.5 * sqr(alpha * linspace(cinput, index, y))));
        }

        size_t size() const { return m_size; }
    private:
        window_linspace_m1_1_trunc<T> linspace;
        T alpha;
        size_t m_size;
    };

    template <typename T>
    struct expression_lanczos : input_expression
    {
        using value_type = T;

        template <cpu_t newcpu>
        using retarget_this = typename in_window<newcpu>::template expression_lanczos<T>;
        expression_lanczos(size_t size, T alpha = 2.5, window_symmetry symmetry = window_symmetry::symmetric)
            : linspace(size, symmetry), alpha(alpha), m_size(size)
        {
        }
        template <typename U, size_t N>
        KFR_INLINE vec<U, N> operator()(cinput_t, size_t index, vec_t<U, N>) const
        {
            constexpr vec_t<T, N> y{};
            return cast<U>(sinc(linspace(cinput, index, y)));
        }
        size_t size() const { return m_size; }

    private:
        window_linspace_mpi_pi<T> linspace;
        T alpha;
        size_t m_size;
    };
};

template <window_type>
struct window_by_type;

#define KFR_WINDOW_BY_TYPE(win)                                                                              \
    template <>                                                                                              \
    struct window_by_type<window_type::win>                                                                  \
    {                                                                                                        \
        template <typename T>                                                                                \
        using type = in_window<>::expression_##win<T>;                                                       \
    };
KFR_WINDOW_BY_TYPE(rectangular)
KFR_WINDOW_BY_TYPE(triangular)
KFR_WINDOW_BY_TYPE(bartlett)
KFR_WINDOW_BY_TYPE(cosine)
KFR_WINDOW_BY_TYPE(hann)
KFR_WINDOW_BY_TYPE(bartlett_hann)
KFR_WINDOW_BY_TYPE(hamming)
KFR_WINDOW_BY_TYPE(bohman)
KFR_WINDOW_BY_TYPE(blackman)
KFR_WINDOW_BY_TYPE(blackman_harris)
KFR_WINDOW_BY_TYPE(kaiser)
KFR_WINDOW_BY_TYPE(flattop)
KFR_WINDOW_BY_TYPE(gaussian)
KFR_WINDOW_BY_TYPE(lanczos)
}

KFR_INLINE internal::in_window<>::expression_rectangular<fbase> window_rectangular(size_t size)
{
    return internal::in_window<>::expression_rectangular<fbase>(size, fbase());
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_triangular<T> window_triangular(size_t size,
                                                                             ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_triangular<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_bartlett<T> window_bartlett(size_t size,
                                                                         ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_bartlett<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_cosine<T> window_cosine(size_t size, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_cosine<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_hann<T> window_hann(size_t size, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_hann<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_bartlett_hann<T> window_bartlett_hann(size_t size,
                                                                                   ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_bartlett_hann<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_hamming<T> window_hamming(size_t size, T alpha = 0.54,
                                                                       ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_hamming<T>(size, alpha);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_bohman<T> window_bohman(size_t size, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_bohman<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_blackman<T> window_blackman(
    size_t size, T alpha = 0.16, window_symmetry symmetry = window_symmetry::symmetric,
    ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_blackman<T>(size, alpha, symmetry);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_blackman_harris<T> window_blackman_harris(
    size_t size, window_symmetry symmetry = window_symmetry::symmetric, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_blackman_harris<T>(size, T(), symmetry);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_kaiser<T> window_kaiser(size_t size, T beta = T(0.5),
                                                                     ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_kaiser<T>(size, beta);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_flattop<T> window_flattop(size_t size, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_flattop<T>(size);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_gaussian<T> window_gaussian(size_t size, T alpha = 2.5,
                                                                         ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_gaussian<T>(size, alpha);
}
template <typename T = fbase>
KFR_INLINE internal::in_window<>::expression_lanczos<T> window_lanczos(size_t size, ctype_t<T> = ctype_t<T>())
{
    return internal::in_window<>::expression_lanczos<T>(size);
}

template <typename T           = fbase, window_type type,
          typename window_expr = typename internal::window_by_type<type>::template type<T>>
KFR_NOINLINE window_expr window(size_t size, cval_t<window_type, type>, T win_param = T(),
                                window_symmetry symmetry = window_symmetry::symmetric,
                                ctype_t<T>               = ctype_t<T>())
{
    return window_expr(size, win_param, symmetry);
}

template <typename T = fbase>
KFR_NOINLINE expression_pointer<T> window(size_t size, window_type type, T win_param,
                                          window_symmetry symmetry = window_symmetry::symmetric,
                                          ctype_t<T>               = ctype_t<T>())
{
    return cswitch(
        cvals<window_type, window_type::rectangular, window_type::triangular, window_type::bartlett,
              window_type::cosine, window_type::hann, window_type::bartlett_hann, window_type::hamming,
              window_type::bohman, window_type::blackman, window_type::blackman_harris, window_type::kaiser,
              window_type::flattop, window_type::gaussian, window_type::lanczos>,
        type,
        [=](auto win) {
            constexpr window_type window = val_of(win);
            return to_pointer<T>(
                typename internal::window_by_type<window>::template type<T>(size, win_param, symmetry));
        },
        fn_returns<expression_pointer<T>>());
}
}

#pragma clang diagnostic pop
