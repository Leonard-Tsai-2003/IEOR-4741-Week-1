#include "hw1.h"
#include <cstdio>

static bool skip(const char* line) {
    while (*line == ' ' || *line == '\t') ++line;
    return *line == '\0' || *line == '#' || *line == '\n' || *line == '\r';
}

static void print_metrics(const char* label, const Quotes& q) {
    Metrics m = compute(q);
    printf("%s  bid=%.2f x %.0f  ask=%.2f x %.0f  "
           "mid=%.2f spread=%.2f micro=%.2f obi=%.2f\n",
           label, q.bid_px, q.bid_sz, q.ask_px, q.ask_sz,
           m.mid, m.spread, m.micro, m.obi);
}

Metrics compute(const Quotes& q) {
    double mid    = (q.bid_px + q.ask_px) / 2.0;
    double spread = q.ask_px - q.bid_px;
    double micro  = (q.ask_px * q.bid_sz + q.bid_px * q.ask_sz)
                  / (q.bid_sz + q.ask_sz);
    double obi    = (q.bid_sz - q.ask_sz) / (q.bid_sz + q.ask_sz);
    return {mid, spread, micro, obi};
}

int read_samples(const char* path, Quotes* out, int max) {
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    int n = 0;
    char line[256];
    while (n < max && fgets(line, sizeof line, f)) {
        if (skip(line)) continue;
        Quotes q;
        if (sscanf(line, "%lf %lf %lf %lf",
                   &q.bid_px, &q.bid_sz, &q.ask_px, &q.ask_sz) != 4) continue;
        out[n++] = q;
    }
    fclose(f);
    return n;
}

bool read_best_quotes(const char* path, Quotes& q) {
    FILE* f = fopen(path, "r");
    if (!f) return false;

    bool have_bid = false, have_ask = false;
    char line[256];
    while (fgets(line, sizeof line, f)) {
        if (skip(line)) continue;

        char side;
        double px, sz;
        if (sscanf(line, " %c %lf %lf", &side, &px, &sz) != 3) continue;

        if (side == 'B' || side == 'b') {
            if (!have_bid || px > q.bid_px) { q.bid_px = px; q.bid_sz = sz; have_bid = true; }
            else if (px == q.bid_px) q.bid_sz += sz;
        } else if (side == 'A' || side == 'a') {
            if (!have_ask || px < q.ask_px) { q.ask_px = px; q.ask_sz = sz; have_ask = true; }
            else if (px == q.ask_px) q.ask_sz += sz;
        }
    }
    fclose(f);
    return have_bid && have_ask;
}

int main() {
    Quotes samples[3];
    if (read_samples("samples.txt", samples, 3) < 3) {
        printf("failed to read samples.txt\n");
        return 1;
    }

    const char* names[] = {
        "balanced 500/500",
        "bid-heavy 900/100",
        "ask-heavy 100/900",
    };

    printf("three samples:\n");
    for (int i = 0; i < 3; ++i) print_metrics(names[i], samples[i]);

    Quotes book{};
    if (!read_best_quotes("book.txt", book)) {
        printf("failed to read book.txt\n");
        return 1;
    }
    printf("top-of-book snapshot:\n");
    print_metrics("book.txt", book);
    return 0;
}
