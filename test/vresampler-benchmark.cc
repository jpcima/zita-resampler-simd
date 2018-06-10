
#include <zita-resampler/vresampler.h>
#include <stdio.h>
#include <random>
#include <chrono>
namespace stc = std::chrono;

int main(int argc, char *argv[])
{
    VResampler *rsm = new VResampler;

    if (argc == 1) {
        fprintf(stderr, "* Using detected CPU features\n");
    }
    else if (argc == 2) {
        fprintf(stderr, "* Requesting CPU features: %s\n", argv[1]);
        rsm->set_cpu(argv[1]);
    }
    else {
        return 1;
    }

    fprintf(stderr, "* Benchmark CPU: %s\n", rsm->get_cpu());

    double sr_in = 53267;
    double sr_out = 48000;
    fprintf(stderr, "* Rate conversion: %g -> %g\n", sr_in, sr_out);

    unsigned hlen = 48;
    fprintf(stderr, "* Filter length: %u\n", hlen);

    if (rsm->setup(sr_out / sr_in, hlen)) {
        fprintf(stderr, "! Error setting up resampling.\n");
        return 1;
    }

    const unsigned iterations = 1024 * 1024;
    std::minstd_rand0 rand;

    stc::steady_clock::time_point tp1, tp2;

    tp1 = stc::steady_clock::now();
    for (unsigned i = 0; i < iterations; ++i)
    {
        float f_in;
        float f_out;
        rsm->inp_count = 0;
        rsm->inp_data = &f_in;
        rsm->out_count = 1;
        rsm->out_data = &f_out;
        while (rsm->process(), rsm->out_count != 0)
        {
            f_in = rand() * (1.0f / rand.max());
            rsm->inp_count = 1;
            rsm->inp_data = &f_in;
            rsm->out_count = 1;
            rsm->out_data = &f_out;
        }
    }
    tp2 = stc::steady_clock::now();

    stc::steady_clock::duration delta = tp2 - tp1;
    auto delta_us = stc::duration_cast<stc::microseconds>(delta);
    fprintf(stderr, "* Duration %g\n", delta_us.count() * 1e-6);

    return 0;
}
