#pragma once

struct Quotes {
    double bid_px, bid_sz, ask_px, ask_sz;
};

struct Metrics {
    double mid, spread, micro, obi;
};

Metrics compute(const Quotes& q);

int  read_samples(const char* path, Quotes* out, int max);
bool read_best_quotes(const char* path, Quotes& q);
