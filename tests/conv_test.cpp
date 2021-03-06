/**
 * KFR (http://kfrlib.com)
 * Copyright (C) 2016  D Levin
 * See LICENSE.txt for details
 */

// library_version()
#include <kfr/io/tostring.hpp>
#include <kfr/version.hpp>

#include <kfr/expressions/reduce.hpp>

#include <tuple>

#include "testo/testo.hpp"
#include <kfr/dft/conv.hpp>

using namespace kfr;

TEST(test_convolve)
{
    univector<double, 5> a({ 1, 2, 3, 4, 5 });
    univector<double, 5> b({ 0.25, 0.5, 1.0, 0.5, 0.25 });
    univector<double> c = convolve(a, b);
    CHECK(c.size() == 9);
    CHECK(native::rms(c - univector<double>({ 0.25, 1., 2.75, 5., 7.5, 8.5, 7.75, 3.5, 1.25 })) < 0.0001);
}

int main(int argc, char** argv)
{
    println(library_version());

    return testo::run_all("", true);
}
