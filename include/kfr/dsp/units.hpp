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
 *
 * If GPL is not suitable for your project, you must purchase a commercial license to use KFR.
 * Buying a commercial license is mandatory as soon as you develop commercial activities without
 * disclosing the source code of your own applications.
 * See http://www.kfrlib.com for details.
 */
#pragma once

#include "../base/log_exp.hpp"
#include "../base/vec.hpp"
#include "../expressions/basic.hpp"

#pragma clang diagnostic push
#if CID_HAS_WARNING("-Winaccessible-base")
#pragma clang diagnostic ignored "-Winaccessible-base"
#endif

namespace kfr
{

using sample_rate_t = double;

namespace internal
{
template <cpu_t c = cpu_t::native, cpu_t cc = c>
struct in_dsp_units : in_log_exp<cc>, in_select<cc>, in_round<cc>, in_abs<cc>
{
private:
    using in_log_exp<cc>::log;
    using in_log_exp<cc>::exp;
    using in_log_exp<cc>::log10;
    using in_log_exp<cc>::exp10;
    using in_log_exp<cc>::exp_fmadd;
    using in_log_exp<cc>::log_fmadd;
    using in_select<cc>::select;
    using in_round<cc>::fract;
    using in_abs<cc>::abs;

public:
    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF amp_to_dB(T amp)
    {
        return log(cast<subtype<TF>>(amp)) * subtype<TF>(8.6858896380650365530225783783322);
        // return T( 20.0 ) * log10( level );
    }

    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF dB_to_amp(T dB)
    {
        return exp(dB * subtype<TF>(0.11512925464970228420089957273422));
        // return exp10( dB / 20 );
    }

    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF amp_to_dB(T amp, T offset)
    {
        return log_fmadd(amp, subtype<TF>(8.6858896380650365530225783783322), offset);
        // return T( 20.0 ) * log10( level );
    }

    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF dB_to_amp(T dB, T offset)
    {
        auto offs = -subtype<TF>(0.11512925464970228420089957273422) * offset;
        return exp_fmadd(dB, subtype<TF>(0.11512925464970228420089957273422), offs);
        // return exp10( dB / 20 );
    }

    template <typename T>
    KFR_SINTRIN T power_to_dB(T x)
    {
        return log(x) * (10 * c_recip_log_10<T>);
    }

    template <typename T>
    KFR_SINTRIN T dB_to_power(T x)
    {
        if (x == -c_infinity<T>)
            return 0.0;
        else
            return exp(x * (c_log_10<T> / 10.0));
    }

    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF note_to_hertz(T note)
    {
        const subtype<TF> offset = 2.1011784386926213177653145771814;

        return exp_fmadd(note, subtype<TF>(0.05776226504666210911810267678818), offset);
    }

    template <typename T, typename TF = ftype<T>>
    KFR_SINTRIN TF hertz_to_note(T hertz)
    {
        const subtype<TF> offset = -36.376316562295915248836189714583;

        return log_fmadd(hertz, subtype<TF>(17.312340490667560888319096172023), offset);
    }

    template <typename T1, typename T2, typename T3, typename Tc = common_type<T1, T2, T3, f32>>
    KFR_SINTRIN Tc note_to_hertz(T1 note, T2 tunenote, T3 tunehertz)
    {
        const Tc offset = log(tunehertz) - tunenote * subtype<Tc>(0.05776226504666210911810267678818);

        return exp_fmadd(note, subtype<Tc>(0.05776226504666210911810267678818), offset);
    }

    template <typename T1, typename T2, typename T3, typename Tc = common_type<T1, T2, T3, f32>>
    KFR_SINTRIN Tc hertz_to_note(T1 hertz, T2 tunenote, T3 tunehertz)
    {
        const Tc offset = tunenote - log(tunehertz) * subtype<Tc>(17.312340490667560888319096172023);

        return log_fmadd(hertz, subtype<Tc>(17.312340490667560888319096172023), offset);
    }

    KFR_SPEC_FN(in_dsp_units, note_to_hertz)
    KFR_SPEC_FN(in_dsp_units, hertz_to_note)
    KFR_SPEC_FN(in_dsp_units, amp_to_dB)
    KFR_SPEC_FN(in_dsp_units, dB_to_amp)
    KFR_SPEC_FN(in_dsp_units, power_to_dB)
    KFR_SPEC_FN(in_dsp_units, dB_to_power)
};
}

using fn_note_to_hertz = internal::in_dsp_units<>::fn_note_to_hertz;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> note_to_hertz(const T1& x)
{
    return internal::in_dsp_units<>::note_to_hertz(x);
}

template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_note_to_hertz, E1> note_to_hertz(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
using fn_hertz_to_note = internal::in_dsp_units<>::fn_hertz_to_note;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> hertz_to_note(const T1& x)
{
    return internal::in_dsp_units<>::hertz_to_note(x);
}
template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_hertz_to_note, E1> hertz_to_note(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
using fn_amp_to_dB = internal::in_dsp_units<>::fn_amp_to_dB;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> amp_to_dB(const T1& x)
{
    return internal::in_dsp_units<>::amp_to_dB(x);
}

template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_amp_to_dB, E1> amp_to_dB(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
using fn_dB_to_amp = internal::in_dsp_units<>::fn_dB_to_amp;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> dB_to_amp(const T1& x)
{
    return internal::in_dsp_units<>::dB_to_amp(x);
}

template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_dB_to_amp, E1> dB_to_amp(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
using fn_power_to_dB = internal::in_dsp_units<>::fn_power_to_dB;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> power_to_dB(const T1& x)
{
    return internal::in_dsp_units<>::power_to_dB(x);
}

template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_power_to_dB, E1> power_to_dB(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
using fn_dB_to_power = internal::in_dsp_units<>::fn_dB_to_power;
template <typename T1, KFR_ENABLE_IF(is_numeric<T1>::value)>
KFR_INTRIN ftype<T1> dB_to_power(const T1& x)
{
    return internal::in_dsp_units<>::dB_to_power(x);
}

template <typename E1, KFR_ENABLE_IF(is_input_expression<E1>::value)>
KFR_INTRIN expr_func<fn_dB_to_power, E1> dB_to_power(E1&& x)
{
    return { {}, std::forward<E1>(x) };
}
}

#pragma clang diagnostic pop
