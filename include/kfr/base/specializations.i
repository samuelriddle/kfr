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
 */
#pragma once

#include "vec.hpp"
#ifndef KFR_SHUFFLE_SPECIALIZATIONS
#include "shuffle.hpp"
#endif

namespace kfr
{
namespace internal
{
template <>
inline vec<f32, 32> shufflevector<f32, 32>(
    csizes_t<0, 1, 8, 9, 16, 17, 24, 25, 2, 3, 10, 11, 18, 19, 26, 27, 4, 5, 12, 13, 20, 21, 28, 29, 6, 7, 14,
             15, 22, 23, 30, 31>,
    vec<f32, 32> x, vec<f32, 32>)
{
    f32x32 w = x;

    w = concat(permute<0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15>(low(w)),
               permute<0, 1, 8, 9, 4, 5, 12, 13, 2, 3, 10, 11, 6, 7, 14, 15>(high(w)));

    w = permutegroups<(4), 0, 4, 2, 6, 1, 5, 3, 7>(w); // avx: vperm2f128 & vinsertf128, sse: no-op
    return w;
}

template <>
inline vec<f32, 32> shufflevector<f32, 32>(
    csizes_t<0, 1, 16, 17, 8, 9, 24, 25, 4, 5, 20, 21, 12, 13, 28, 29, 2, 3, 18, 19, 10, 11, 26, 27, 6, 7, 22,
             23, 14, 15, 30, 31>,
    vec<f32, 32> x, vec<f32, 32>)
{
    f32x32 w = x;

    w = concat(permute<0, 1, 8, 9, 4, 5, 12, 13, /**/ 2, 3, 10, 11, 6, 7, 14, 15>(even<8>(w)),
               permute<0, 1, 8, 9, 4, 5, 12, 13, /**/ 2, 3, 10, 11, 6, 7, 14, 15>(odd<8>(w)));

    w = permutegroups<(4), 0, 4, 1, 5, 2, 6, 3, 7>(w); // avx: vperm2f128 & vinsertf128, sse: no-op
    return w;
}

inline vec<f32, 32> bitreverse_2(vec<f32, 32> x)
{
    return shufflevector<f32, 32>(csizes<0, 1, 16, 17, 8, 9, 24, 25, 4, 5, 20, 21, 12, 13, 28, 29, 2, 3, 18,
                                         19, 10, 11, 26, 27, 6, 7, 22, 23, 14, 15, 30, 31>,
                                  x, x);
}

template <>
inline vec<f32, 64> shufflevector<f32, 64>(
    csizes_t<0, 1, 32, 33, 16, 17, 48, 49, 8, 9, 40, 41, 24, 25, 56, 57, 4, 5, 36, 37, 20, 21, 52, 53, 12, 13,
             44, 45, 28, 29, 60, 61, 2, 3, 34, 35, 18, 19, 50, 51, 10, 11, 42, 43, 26, 27, 58, 59, 6, 7, 38,
             39, 22, 23, 54, 55, 14, 15, 46, 47, 30, 31, 62, 63>,
    vec<f32, 64> x, vec<f32, 64>)
{
    x = concat(bitreverse_2(even<8>(x)), bitreverse_2(odd<8>(x)));
    return permutegroups<(8), 0, 4, 1, 5, 2, 6, 3, 7>(x);
}

template <>
inline vec<f32, 16> shufflevector<f32, 16>(csizes_t<0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15>,
                                           vec<f32, 16> x, vec<f32, 16>)
{
//    asm volatile("int $3");
    x = permutegroups<(4), 0, 2, 1, 3>(x);

    x = concat(shuffle<0, 2, 8 + 0, 8 + 2>(low(x), high(x)), shuffle<1, 3, 8 + 1, 8 + 3>(low(x), high(x)));

    return x;
}

template <>
inline vec<f32, 16> shufflevector<f32, 16>(csizes_t<0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15>,
                                           vec<f32, 16> x, vec<f32, 16>)
{
    x = concat(shuffle<0, 8 + 0, 1, 8 + 1>(low(x), high(x)), shuffle<2, 8 + 2, 3, 8 + 3>(low(x), high(x)));

    x = permutegroups<(4), 0, 2, 1, 3>(x);

    return x;
}

template <>
inline vec<f32, 32> shufflevector<f32, 32>(
    csizes_t<0, 16, 1, 17, 2, 18, 3, 19, 4, 20, 5, 21, 6, 22, 7, 23, 8, 24, 9, 25, 10, 26, 11, 27, 12, 28, 13,
             29, 14, 30, 15, 31>,
    vec<f32, 32> x, vec<f32, 32>)
{
    x = permutegroups<(8), 0, 2, 1, 3>(x);

    x = concat(interleavehalfs(low(x)), interleavehalfs(high(x)));

    return x;
}
}
}