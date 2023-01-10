#pragma once

#include "helper.h"
#include "pixel.h"
#include <array>
#include <set>
#include <map>
#include <optional>



struct CompareColors final {
	bool operator()(D2D1_COLOR_F a, D2D1_COLOR_F b) const noexcept {
		if (a.r != b.r) {
			return a.r < b.r;
		}
		if (a.g != b.g) {
			return a.g < b.g;
		}
		if (a.b != b.b) {
			return a.b < b.b;
		}
		return a.a < b.a;
	}
};

typedef std::array<std::set<D2D1_COLOR_F, CompareColors>, 8> RuleNeighbours;
typedef std::map<D2D1_COLOR_F, RuleNeighbours, CompareColors> Ruleset;


struct CollapsedPixel {
	std::set<D2D1_COLOR_F, CompareColors> options;

	Point2 index;
	D2D1_COLOR_F color;

	CollapsedPixel() : color{ D2D1::ColorF(D2D1::ColorF::White) }, index{ 0, 0 } {}

	// Dla ka¿dej opcji patrzymy czy istnieje regu³a, która pozwoli³aby na wyst¹pienie tej opcji
	bool collapse_options(const Ruleset& rules, const CollapsedPixel& neighbour, size_t direction);

	// Add all possible colors to options
	void populate_options(const Ruleset& rules);

	// Picks color at random
	void pick_color();
};


struct ComparePixels final {
	bool operator()(const std::reference_wrapper<CollapsedPixel>& a, const std::reference_wrapper<CollapsedPixel>& b) const noexcept {
		if (a.get().options.size() != b.get().options.size()) {
			return a.get().options.size() < b.get().options.size();
		}
		if (a.get().index.x != b.get().index.x) {
			return a.get().index.x < b.get().index.x;
		}
		return a.get().index.y < b.get().index.y;
	}
};

class WaveFunctionCollapse {
public:
	// Mogê gradientowaæ elementy, które maj¹ mniej regó³ ni¿ max, ale to ju¿ jakiœ mocny dodatek
	std::vector<CollapsedPixel> generate(const std::vector<std::vector<Pixel>>& pixels, Point2 size);

private:

	void collapse_neighbours(CollapsedPixel& pixel, std::set<std::reference_wrapper<CollapsedPixel>, ComparePixels>& q);

	std::optional<std::reference_wrapper<CollapsedPixel>> pixel(Point2 index);
	void create_matrix(Point2 size);
	void create_rules(const std::vector<std::vector<Pixel>>& pixels);
	void add_white_border();

	D2D1_COLOR_F neighbour_color(int32_t x, int32_t y, const std::vector<std::vector<Pixel>>& pixels);
	static const size_t map_neighbour_location[3][3];

	Ruleset rules;
	std::vector<std::vector<CollapsedPixel>> matrix;
};