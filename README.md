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

It sends one request per item, 3 per second at most, so a full run takes about
25 minutes. Progress and errors go to stderr.

## Score

| Value | How it is computed |
| --- | --- |
| price | median of the daily median sale prices |
| volume | sales per day, once the 10% best and 10% worst days are dropped |
| score | 100 x geometric mean of the percentile ranks of price and volume |

A few absurd trades cannot move a median, and an offer that sold on less than
10 days out of 90 ends with a volume of 0, so a score of 0.

## Page

`index.html` is served by GitHub Pages (Jekyll) and reads `analytics.json` at the
root of the repository. To publish new numbers, run the scraper and push the file.
