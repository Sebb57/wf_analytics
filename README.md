# wf_analytics

Finds what sells best on [warframe.market](https://warframe.market).

The scraper goes through every item and each of its variants (mod rank, fish size,
relic refinement, ayatan stars), reads 90 days of sales, estimates how long it
takes to farm, and gives every offer a score out of 100 in each ranking. A small
GitHub page shows the top 10 and lets you count farm time in the score or not,
hide it, filter on it and search the rest.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./warframe_analytics > analytics.json
```

It downloads the drop tables once, then sends one request per item and one more
per set, 2 per second at most, so a full run takes about 35 minutes. Progress and
errors go to stderr.

## Score

| Value | How it is computed |
| --- | --- |
| price | median of the daily median sale prices |
| copies | rank 0 copies used to make the offer: `(r + 1)(r + 2) / 2` for an arcane of rank `r`, 1 otherwise |
| price per copy | price / copies |
| volume | sales per day, once the 10% best and 10% worst days are dropped |
| farm | hours to farm what makes the offer, see below |
| farm rate | copies farmed per hour: copies / farm |
| platinum per hour | price / farm, shown on the page |

A score is 100 x the geometric mean of the percentile ranks of its criteria,
among the offers where every criterion is known. Farm is an optional criterion,
so every offer gets two scores:

| Score | Criteria | On the page |
| --- | --- | --- |
| `farm` | price per copy, volume, farm rate | "Count farm time in the score" ticked |
| `market` | price per copy, volume | unticked |

An offer that cannot be farmed has no `farm` score: the page lists it last. The
scores live in the `RANKINGS` table of `src/analytics/Scorer.hpp`.

A few absurd trades cannot move a median, and an offer that sold on less than
10 days out of 90 ends with a volume of 0, so a score of 0.

A rank 5 arcane takes 21 rank 0 copies. Scoring on the price per copy shows when
selling those 21 copies one by one pays more than ranking it up. The cost rules
live in the `COSTS` table of `src/analytics/Analyzer.hpp`, by item tag.

## Farm

The drop chances come from the official drop tables
([JSON copy](https://drops.warframestat.us/data/all.slim.json)). For every item
the scraper takes its best source: minutes for one try / drop chance.

- A try at a mission takes the time in the `MISSIONS` table of
  `src/analytics/Farm.hpp`, an estimate per mission type. In endless missions the
  rewards follow the A, A, B, C cycle, so a rotation C reward takes 4 times that.
- A prime part comes from a relic: farm the relic, then open it (5 minutes).
  Only relics that still drop count, so vaulted parts are not farmable.
- A set is the sum of its parts, a ranked arcane is its copies times one.
- Only the form that drops gets a farm time: rank 0 mods, intact relics. Ranked
  mods and refined relics have none, nor do items bought from syndicates or
  vendors (Baro, Conclave, Simaris...).

These are orders of magnitude for one player alone: squads, refined relics,
boosters and the other rewards of the same mission are not counted.

## Page

`index.html` is served by GitHub Pages (Jekyll) and reads `analytics.json` at the
root of the repository.

The `Daily analysis` workflow reruns the scraper every night at midnight (Paris)
and commits `analytics.json` when it changed. It can also be started by hand from
the Actions tab.
