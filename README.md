# wf_analytics

Finds what sells best on [warframe.market](https://warframe.market).

The scraper goes through every item and each of its variants (mod rank, fish size,
relic refinement, ayatan stars), reads 90 days of sales and gives every offer a
score out of 100. A small GitHub page shows the top 10 and lets you search the rest.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./warframe_analytics > analytics.json
```

It sends one request per item, 2 per second at most, so a full run takes about
35 minutes. Progress and errors go to stderr.

## Score

| Value | How it is computed |
| --- | --- |
| price | median of the daily median sale prices |
| copies | rank 0 copies used to make the offer: `(r + 1)(r + 2) / 2` for an arcane of rank `r`, 1 otherwise |
| price per copy | price / copies |
| volume | sales per day, once the 10% best and 10% worst days are dropped |
| score | 100 x geometric mean of the percentile ranks of price per copy and volume |

A few absurd trades cannot move a median, and an offer that sold on less than
10 days out of 90 ends with a volume of 0, so a score of 0.

A rank 5 arcane takes 21 rank 0 copies. Scoring on the price per copy shows when
selling those 21 copies one by one pays more than ranking it up. The cost rules
live in the `COSTS` table of `src/analytics/Analyzer.hpp`, by item tag.

## Page

`index.html` is served by GitHub Pages (Jekyll) and reads `analytics.json` at the
root of the repository. To publish new numbers, run the scraper and push the file.
