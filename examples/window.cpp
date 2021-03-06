/**
 * KFR (http://kfrlib.com)
 * Copyright (C) 2016  D Levin
 * See LICENSE.txt for details
 */

// library_version()
#include <kfr/version.hpp>

// print(), format()
#include <kfr/cometa/string.hpp>

#include <kfr/math.hpp>

// simpleimpulse()
#include <kfr/dsp/oscillators.hpp>

// window*
#include <kfr/dsp/window.hpp>

// plot_save()
#include <kfr/io/python_plot.hpp>

using namespace kfr;
using namespace kfr::native;

int main(int argc, char** argv)
{
    println(library_version());

    using namespace native;
    const std::string options = "freqresp=True, dots=True, padwidth=1024, "
                                "log_freq=False, horizontal=False, normalized_freq=True";

    univector<double, 64> output;
    output = window_hann(output.size());
    plot_save("window_hann", output, options + ", title='Hann window'");

    output = window_hamming(output.size());
    plot_save("window_hamming", output, options + ", title='Hamming window'");

    output = window_blackman(output.size());
    plot_save("window_blackman", output, options + ", title='Blackman window'");

    output = window_blackman_harris(output.size());
    plot_save("window_blackman_harris", output, options + ", title='Blackman-Harris window'");

    output = window_gaussian(output.size());
    plot_save("window_gaussian", output, options + ", title='Gaussian window'");

    output = window_triangular(output.size());
    plot_save("window_triangular", output, options + ", title='Triangular window'");

    output = window_bartlett(output.size());
    plot_save("window_bartlett", output, options + ", title='Bartlett window'");

    output = window_cosine(output.size());
    plot_save("window_cosine", output, options + ", title='Cosine window'");

    output = window_bartlett_hann(output.size());
    plot_save("window_bartlett_hann", output, options + ", title='Bartlett-Hann window'");

    output = window_bohman(output.size());
    plot_save("window_bohman", output, options + ", title='Bohman window'");

    output = window_lanczos(output.size());
    plot_save("window_lanczos", output, options + ", title='Lanczos window'");

    output = window_flattop(output.size());
    plot_save("window_flattop", output, options + ", title='Flat top window'");

    output = window_kaiser(output.size(), 2.5);
    plot_save("window_kaiser", output, options + ", title='Kaiser window'");

    return 0;
}
