#include "generator.h"
#include <ranges>
#include <random>

std::vector<CollapsedPixel> WaveFunctionCollapse::generate(const std::vector<std::vector<Pixel>>& pixels, Point2 size) {
	std::vector<CollapsedPixel> result;
	create_rules(pixels);
	create_matrix(size);
	add_white_border();

	// Zacznij sortowaæ po iloœci mo¿liwych regu³
	std::set<std::reference_wrapper<CollapsedPixel>, ComparePixels> q;
	for (auto& row : matrix) {
		for (auto& pixel : row) {
			q.insert(pixel);
		}
	}

	// G³ówna czêœæ algorytmu:
	// Bierzemy pierwszy wierzcho³ek z kolejki i decydujemy jego kolor
	// Zmieniamy wszystkich s¹siadów
	// Jeœli oni siê zmienili to zmieniamy ich s¹siadów
	while (!q.empty()) {
		auto pixel = *q.begin();
		q.erase(q.begin());

		pixel.get().pick_color();
		collapse_neighbours(pixel, q);

		result.emplace_back(std::move(pixel));
	}
	return result;
}

void WaveFunctionCollapse::collapse_neighbours(CollapsedPixel& pixel, std::set<std::reference_wrapper<CollapsedPixel>, ComparePixels>& q) {
	std::vector<std::reference_wrapper<CollapsedPixel>> to_update;
	for (int32_t i = -1; i <= 1; i++) {
		for (int32_t j = -1; j <= 1; j++) {
			if (i == j && i == 0) {
				continue;
			}
			auto neigh = this->pixel(D2D1::Point2(pixel.index.x + i, pixel.index.y + j));
			if (!neigh) {
				continue;
			}
			// Inverse mapping for location
			// TODO: To musi braæ wszystkich s¹siadów, a nie tylko tego jednego
			// Generalnie algorytm powinien znaleŸæ wszystkie kwadraty 3x3, które jeszcze pasuj¹ do tej sytuacji
			// Zatem nie powinniœmy mu przekazywaæ informacji o kierunku, tylko wszystkich s¹siadów
			if (neigh->get().collapse_options(rules, pixel, map_neighbour_location[1 - i][1 - j])) {
				to_update.push_back(neigh.value());
			}
		}
	}
	// Reeavluate set accordingly to changed options
	for (auto it : to_update) {
		q.erase(it);
		q.insert(it);
	}
	for (auto it : to_update) {
		collapse_neighbours(it, q);
	}
}

void WaveFunctionCollapse::create_rules(const std::vector<std::vector<Pixel>>& pixels) {
	rules[D2D1::ColorF(D2D1::ColorF::White)] = {};
	for (size_t i = 0; i < pixels.size(); i++) {
		for (size_t j = 0; j < pixels[i].size(); j++) {
			auto color = pixels[i][j].color();
			if (!rules.contains(color)) {
				rules[color] = {};
			}

			for (int32_t p = -1; p <= 1; p++) {
				for (int32_t q = -1; q <= 1; q++) {
					if (q == p && q == 0) {
						continue;
					}
					auto neigh_color = neighbour_color(p + i, q + j, pixels);
					auto loc = map_neighbour_location[p + 1][q + 1];
					if (loc >= rules[color].size()) {
						continue;
					}
					rules[color][loc].insert(neigh_color);

					if (p + i < 0 || p + i >= pixels.size() || q + j < 0 || q + j >= pixels[p + i].size()) {
						rules[D2D1::ColorF(D2D1::ColorF::White)][map_neighbour_location[1 - p][1 - q]].insert(color);
					}
				}
			}
		}
	}
}

std::optional<std::reference_wrapper<CollapsedPixel>> WaveFunctionCollapse::pixel(Point2 index) {
	if (index.x < 0 || index.x >= matrix.size()) {
		return {};
	}
	if (index.y < 0 || index.y >= matrix[index.x].size()) {
		return {};
	}
	return matrix[index.x][index.y];
}

void WaveFunctionCollapse::create_matrix(Point2 size) {
	matrix.resize(size.x + 2);
	size_t x = 0;
	for (auto& row : matrix) {
		row.resize(size.y + 2);
		size_t y = 0;
		for (auto& pixel : row) {
			pixel.populate_options(rules);
			pixel.index = D2D1::Point2(x, y++);
		}
		x++;
	}
}

D2D1_COLOR_F WaveFunctionCollapse::neighbour_color(int32_t x, int32_t y, const std::vector<std::vector<Pixel>>& pixels) {
	if (x < 0 || x >= pixels.size()) {
		return D2D1::ColorF(D2D1::ColorF::White);
	}
	if (y < 0 || y >= pixels[x].size()) {
		return D2D1::ColorF(D2D1::ColorF::White);
	}
	return pixels[x][y].color();
}

void WaveFunctionCollapse::add_white_border() {
	Ruleset empty_ruleset;
	empty_ruleset[D2D1::ColorF(D2D1::ColorF::White)] = {};
	for (auto& pixel : matrix.front()) {
		pixel.populate_options(empty_ruleset);
	}
	for (auto& pixel : matrix.back()) {
		pixel.populate_options(empty_ruleset);
	}
	for (auto& col : matrix) {
		col.front().populate_options(empty_ruleset);
		col.back().populate_options(empty_ruleset);
	}
}

const size_t WaveFunctionCollapse::map_neighbour_location[3][3]{
	{0, 1, 2},
	{3, 8, 4},
	{5, 6, 7}
};

bool CollapsedPixel::collapse_options(const Ruleset& rules, const CollapsedPixel& neighbour, size_t direction) {
	auto allowed_options = options | std::views::filter([&](const auto& el) {
		for (const auto& opt : neighbour.options) {
			if (rules.at(el)[direction].contains(opt)) {
				return true;
			}
		}
		return false;
	});

	// Waiting for std::ranges::to in c++23 :c
	std::set<D2D1_COLOR_F, CompareColors> new_options;
	for (auto it : allowed_options) {
		new_options.insert(it);
	}

	if (new_options.size() == options.size()) {
		return false;
	}

	options = std::move(new_options);
	return true;
}

void CollapsedPixel::populate_options(const Ruleset& rules) {
	options.clear();
	for (auto& [key, _] : rules) {
		options.insert(key);
	}
}

void CollapsedPixel::pick_color() {
	if (options.size() == 0) {
		return;
	}
	static std::random_device r;
	static std::default_random_engine e1(r());

	std::uniform_int_distribution<size_t> uniform_dist(0, options.size() - 1);
	size_t random_val = uniform_dist(e1);

	if (random_val == 0) {
		color = *options.begin();
	} else {
		auto color_range = options | std::views::drop(random_val - 1);
		color = *color_range.begin();
	}

	// Rest of functions still operate on options
	options.clear();
	options.insert(color);
}
