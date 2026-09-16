#include "hw1.h"
#include <cstdio>

static bool skip(const char* line) {
    while (*line == ' ' || *line == '\t') ++line;
    return *line == '\0' || *line == '#' || *line == '\n' || *line == '\r';
}

static void print_metrics(const char* label, const Quotes& q) {
    const Metrics m = compute(q);
    printf("%s  bid=%.2f x %.0f  ask=%.2f x %.0f  "
           "mid=%.2f spread=%.2f micro=%.2f obi=%.2f\n",
           label, q.bid_px, q.bid_sz, q.ask_px, q.ask_sz,
           m.mid, m.spread, m.micro, m.obi);
}

static bool print_book_metrics(const char* path) {
    Quotes book{};
    if (!read_best_quotes(path, book)) {
        printf("failed to read %s\n", path);
        return false;
    }
    print_metrics(path, book);
    return true;
}

Metrics compute(const Quotes& q) {
    const double total_size = q.bid_sz + q.ask_sz;
    return {(q.bid_px + q.ask_px) / 2.0,
            q.ask_px - q.bid_px,
            (q.ask_px * q.bid_sz + q.bid_px * q.ask_sz) / total_size,
            (q.bid_sz - q.ask_sz) / total_size};
}

int read_samples(const char* path, Quotes* out, int max) {
    FILE* f = fopen(path, "r");
    if (!f) return 0;

    int n = 0;
    char line[256];
    while (n < max && fgets(line, sizeof line, f)) {
        if (skip(line)) continue;
        Quotes q{};
        if (sscanf(line, "%lf %lf %lf %lf", &q.bid_px, &q.bid_sz,
                   &q.ask_px, &q.ask_sz) == 4) out[n++] = q;
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
            if (!have_bid || px > q.bid_px)
                q.bid_px = px, q.bid_sz = sz, have_bid = true;
            else if (px == q.bid_px) q.bid_sz += sz;
        } else if (side == 'A' || side == 'a') {
            if (!have_ask || px < q.ask_px)
                q.ask_px = px, q.ask_sz = sz, have_ask = true;
            else if (px == q.ask_px) q.ask_sz += sz;
        }
    }
    fclose(f);
    return have_bid && have_ask;
}

int main() {
    // LAB 01: read the samples and print the metrics
    constexpr int sample_count = 3;
    Quotes samples[sample_count];
    if (read_samples("data/samples.txt", samples, sample_count) < sample_count) {
        printf("failed to read data/samples.txt\n");
        return 1;
    }

    const char* names[] = {
        "balanced 500/500",
        "bid-heavy 900/100",
        "ask-heavy 100/900",
    };
    printf("three samples:\n");
    for (int i = 0; i < sample_count; ++i) print_metrics(names[i], samples[i]);

    // HW 1: read the top-of-book snapshot and print the metrics
    Quotes book{};
    if (!read_best_quotes("data/book.txt", book)) {
        printf("failed to read data/book.txt\n");
        return 1;
    }
    printf("\ntop-of-book snapshot:\n");
    print_metrics("data/book.txt", book);

    // HW 1: read the top-of-book sequence and print the metrics
    const char* books[] = {
        "data/books/book_01.txt", "data/books/book_02.txt",
        "data/books/book_03.txt", "data/books/book_04.txt",
        "data/books/book_05.txt",
    };
    printf("\ntop-of-book sequence:\n");
    for (const char* path : books)
        if (!print_book_metrics(path)) return 1;
    return 0;
}
