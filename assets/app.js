const { analytics, market, assets, platinum } =
  document.querySelector("main").dataset;

const TOP = 10;
const LIMIT = 10 ** 1000;

// farm filter -> label, and the offers it keeps
const FARM_FILTERS = {
  any: ["Any", () => true],
  farmable: ["Farmable", (offer) => offer.farm !== undefined],
  hour: ["Under 1 h", (offer) => offer.farm <= 1],
  ten: ["Under 10 h", (offer) => offer.farm <= 10],
  none: ["Not farmable", (offer) => offer.farm === undefined],
};

// hours in a unit -> unit, biggest first
const DURATIONS = [
  [24, "d"],
  [1, "h"],
  [1 / 60, "min"],
];

// minimum score -> css class
const GRADES = [
  [70, "good"],
  [40, "fair"],
  [0, "poor"],
];

const COLUMNS = [
  { title: "#", className: "number rank", cell: (offer) => offer.rank },
  {
    title: "",
    className: "thumb",
    cell: (offer) =>
      element("img", { src: assets + offer.thumb, alt: "", loading: "lazy" }),
  },
  {
    title: "Item",
    cell: (offer) => [
      element("a", {
        href: market + offer.slug,
        target: "_blank",
        rel: "noopener",
        textContent: offer.name,
      }),
      element("span", { className: "variant", textContent: offer.variant }),
    ],
  },
  {
    title: "Price",
    className: "number",
    cell: (offer) => [
      ...plat(offer.price),
      element("span", { className: "variant" }, perCopy(offer)),
    ],
  },
  { title: "Sold / day", className: "number", cell: (offer) => offer.volume },
  {
    title: "Farm",
    className: "number",
    cell: (offer) =>
      offer.farm === undefined
        ? "–"
        : [
            duration(offer.farm),
            element("span", { className: "variant" }, [
              ...plat(offer.perHour),
              " / h",
            ]),
          ],
  },
  {
    title: "Score",
    className: "number score",
    cell: (offer) =>
      element("span", {
        className: grade(offer.score),
        textContent: offer.score,
      }),
  },
];

function element(tag, properties = {}, children = []) {
  const node = Object.assign(document.createElement(tag), properties);

  node.append(...children);
  return node;
}

// amount followed by the platinum icon
function plat(amount) {
  return [
    `${amount} `,
    element("img", { src: platinum, className: "platinum", alt: "p" }),
  ];
}

// price of one rank 0 copy, for offers that need several (arcanes)
function perCopy({ price, copies = 1 }) {
  return copies > 1
    ? [...plat((price / copies).toFixed(1)), ` × ${copies}`]
    : [];
}

// two significant digits
function round(value) {
  return Number(value.toPrecision(2));
}

// in the biggest unit that fits once rounded: 45 min, 2.5 h, 12 d
function duration(hours) {
  const [size, unit] =
    DURATIONS.find(([size]) => round(hours / size) >= 1) ?? DURATIONS.at(-1);

  return `${round(hours / size)} ${unit}`;
}

function grade(score) {
  return GRADES.find(([minimum]) => score >= minimum)[1];
}

function row(tag, cell) {
  return element(
    "tr",
    {},
    COLUMNS.map((column) =>
      element(
        tag,
        { className: column.className ?? "" },
        [cell(column)].flat(),
      ),
    ),
  );
}

function options(select, entries) {
  select.replaceChildren(
    ...entries.map(([value, textContent]) =>
      element("option", { value, textContent }),
    ),
  );
}

function fill(table, offers) {
  table.replaceChildren(
    element("thead", {}, [row("th", (column) => column.title)]),
    element(
      "tbody",
      {},
      offers.map((offer) => row("td", (column) => column.cell(offer))),
    ),
  );
}

function search(offers, query) {
  const words = query.toLowerCase().split(/\s+/).filter(Boolean);
  const found = offers.filter((offer) =>
    words.every((word) => offer.text.includes(word)),
  );

  fill(document.querySelector("#results"), found.slice(0, LIMIT));
  document.querySelector("#count").textContent =
    `${found.length} offers, showing the best ${Math.min(found.length, LIMIT)}`;
}

// offers in the chosen ranking that pass the farm filter, best first
function ranked(offers) {
  const ranking = document.querySelector("#ranking").value;
  const [, keep] = FARM_FILTERS[document.querySelector("#farm").value];

  return offers
    .filter((offer) => offer.scores[ranking] !== undefined && keep(offer))
    .sort((a, b) => b.scores[ranking] - a.scores[ranking])
    .map((offer, index) =>
      Object.assign(offer, { rank: index + 1, score: offer.scores[ranking] }),
    );
}

function render(offers) {
  const list = ranked(offers);

  fill(document.querySelector("#top"), list.slice(0, TOP));
  search(list, document.querySelector("#search").value);
}

function show({ updated, rankings, offers }) {
  options(
    document.querySelector("#ranking"),
    rankings.map(({ id, label }) => [id, label]),
  );
  options(
    document.querySelector("#farm"),
    Object.entries(FARM_FILTERS).map(([id, [label]]) => [id, label]),
  );
  offers.forEach((offer) => {
    offer.text = `${offer.name} ${offer.variant}`.toLowerCase();
  });
  document
    .querySelectorAll("select, #search")
    .forEach((control) =>
      control.addEventListener("input", () => render(offers)),
    );
  render(offers);
  document.querySelector("#updated").textContent =
    `Updated ${new Date(updated).toLocaleDateString()}.`;
}

fetch(analytics)
  .then((response) => response.json())
  .then(show)
  .catch((error) => {
    document.querySelector("#count").textContent =
      `Could not load ${analytics}: ${error.message}`;
  });
