# Homework 1: Top-of-Book Metrics

`hw1.cpp` reads small market-data files and calculates common top-of-book
metrics. It uses the structures and public function declarations in `hw1.h`.

## Build and run

From this directory, compile and run the program with:

```sh
g++ -std=c++11 -o /tmp/hw1 hw1.cpp && /tmp/hw1
```

The program uses relative paths, so run the executable from this directory.

## Three tasks in the program

1. **Compare three supplied top-of-book quotes.** The program reads the three
   rows in `data/samples.txt`, then reports how identical prices can produce
   different microprices and order-book imbalances when their sizes differ.
2. **Create a top-of-book snapshot from a multi-level order book.** It reads
   `data/book.txt`, identifies the highest bid and lowest ask, and aggregates
   sizes when multiple orders exist at either best price.
3. **Track a sequence of order books.** It reads five files under
   `data/books/` and prints each resulting top-of-book metrics line to show
   how the market evolves across snapshots.

## Data structure

The `Quotes` structure represents one top-of-book quote:

```cpp
struct Quotes {
    double bid_px, bid_sz, ask_px, ask_sz;
};
```

- `bid_px` and `bid_sz` are the best bid price and its total size.
- `ask_px` and `ask_sz` are the best ask price and its total size.

The `Metrics` structure stores the calculated values:

```cpp
struct Metrics {
    double mid, spread, micro, obi;
};
```

The metrics are calculated as follows, where `B` and `A` denote the best bid
and ask prices, and `Q_B` and `Q_A` denote their sizes:

- Midprice: `(B + A) / 2`
- Spread: `A - B`
- Microprice: `(A * Q_B + B * Q_A) / (Q_B + Q_A)`
- Order-book imbalance (OBI): `(Q_B - Q_A) / (Q_B + Q_A)`

### Input files

`data/samples.txt` contains one already-aggregated top-of-book quote per row:

```text
bid_price  bid_size  ask_price  ask_size
100.00     500       100.02     500
```

`data/book.txt` and every file in `data/books/` contain multiple levels. Each
non-comment row has this format:

```text
side  price  size
B     100.00 400
A     100.02 250
```

`B` means a buy order (bid), and `A` means a sell order (ask). A snapshot has
six bid levels and six ask levels. Lines beginning with `#` and blank lines are
ignored. The program selects the highest bid and lowest ask; it adds together
the sizes of all rows at those best prices.

## Functions

- `skip(const char* line)`: returns `true` for blank, whitespace-only, or
  comment lines.
- `print_metrics(const char* label, const Quotes& q)`: calculates and prints
  all metrics for one top-of-book quote.
- `print_book_metrics(const char* path)`: reads a multi-level book file and
  prints its resulting top-of-book metrics.
- `compute(const Quotes& q)`: returns a `Metrics` value containing midprice,
  spread, microprice, and OBI.
- `read_samples(const char* path, Quotes* out, int max)`: reads up to `max`
  pre-aggregated quote rows from a samples file and returns the count read.
- `read_best_quotes(const char* path, Quotes& q)`: reads a multi-level book,
  finds its best bid and ask, aggregates size at each best price, and returns
  whether both sides were found.
- `main()`: runs the three tasks in order.

## Example output

```text
three samples:
balanced 500/500  bid=100.00 x 500  ask=100.02 x 500  mid=100.01 spread=0.02 micro=100.01 obi=0.00
bid-heavy 900/100  bid=100.00 x 900  ask=100.02 x 100  mid=100.01 spread=0.02 micro=100.02 obi=0.80
ask-heavy 100/900  bid=100.00 x 100  ask=100.02 x 900  mid=100.01 spread=0.02 micro=100.00 obi=-0.80

top-of-book snapshot:
data/book.txt  bid=100.00 x 500  ask=100.02 x 300  mid=100.01 spread=0.02 micro=100.01 obi=0.25

top-of-book sequence:
data/books/book_01.txt  bid=100.00 x 500  ask=100.02 x 500  mid=100.01 spread=0.02 micro=100.01 obi=0.00
data/books/book_02.txt  bid=100.01 x 750  ask=100.03 x 300  mid=100.02 spread=0.02 micro=100.02 obi=0.43
data/books/book_03.txt  bid=100.02 x 900  ask=100.04 x 150  mid=100.03 spread=0.02 micro=100.04 obi=0.71
data/books/book_04.txt  bid=100.02 x 200  ask=100.05 x 900  mid=100.03 spread=0.03 micro=100.03 obi=-0.64
data/books/book_05.txt  bid=100.00 x 300  ask=100.04 x 700  mid=100.02 spread=0.04 micro=100.01 obi=-0.40
```

## Interpreting a rising OBI

OBI ranges from `-1` to `+1`. A rising OBI means bid-side size is becoming
larger relative to ask-side size. This indicates stronger displayed buying
interest at the best prices, and it shifts the microprice toward the ask. For
example, the sequence rises from `0.00` in `book_01` to `0.71` in `book_03` as
the best bid size grows and the best ask size shrinks. OBI describes displayed
liquidity, not a guarantee that the next traded price will rise.
