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

#include "fir.hpp"

namespace kfr
{

namespace native
{

template <typename T, typename E1>
KFR_INLINE internal::in_fir<>::expression_short_fir<2, T, E1> fracdelay(E1&& e1, T delay)
{
    if (delay < 0)
        delay = 0;
    if (delay > 1)
        delay = fract(delay);
    univector<T, 2> taps({ 1 - delay, delay });
    return internal::in_fir<>::expression_short_fir<2, T, E1>(std::forward<E1>(e1), taps.ref());
}
}
}
