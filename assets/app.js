const { analytics, market, assets } = document.querySelector("main").dataset;

const TOP = 10;
const LIMIT = 10 ** 1000;

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
      element("a", { href: market + offer.slug, textContent: offer.name }),
      element("span", { className: "variant", textContent: offer.variant }),
    ],
  },
  {
    title: "Price",
    className: "number",
    cell: (offer) => [
      `${offer.price} p`,
      element("span", { className: "variant", textContent: perCopy(offer) }),
    ],
  },
  { title: "Sold / day", className: "number", cell: (offer) => offer.volume },
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

// price of one rank 0 copy, for offers that need several (arcanes)
function perCopy({ price, copies = 1 }) {
  return copies > 1 ? `${(price / copies).toFixed(1)} p × ${copies}` : "";
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

function show({ updated, offers }) {
  const input = document.querySelector("#search");

  offers.forEach((offer, index) => {
    offer.rank = index + 1;
    offer.text = `${offer.name} ${offer.variant}`.toLowerCase();
  });
  fill(document.querySelector("#top"), offers.slice(0, TOP));
  input.addEventListener("input", () => search(offers, input.value));
  search(offers, input.value);
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
