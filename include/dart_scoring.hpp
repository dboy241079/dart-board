#pragma once

#include <array>
#include <cmath>
#include <string>

namespace dart_scoring {

// Board-plane coordinates in millimetres: x points right, y points down,
// and (0, 0) is the bull centre. Distances are measured to scoring-wire edges.
constexpr double kInnerBullRadiusMm = 6.35;
constexpr double kOuterBullRadiusMm = 15.9;
constexpr double kTrebleInnerRadiusMm = 99.0;
constexpr double kTrebleOuterRadiusMm = 107.0;
constexpr double kDoubleInnerRadiusMm = 162.0;
constexpr double kBoardOuterRadiusMm = 170.0;

enum class Ring { Miss, InnerBull, OuterBull, Single, Treble, Double };

struct Result {
    Ring ring{Ring::Miss};
    int number{0}; // 1-20, or 25 for either bull
    int multiplier{0};
    int points{0};
    std::string label{"MISS"};
};

inline Result score_xy_mm(double x, double y) {
    static constexpr std::array<int, 20> clockwiseFromTop{
        20, 1, 18, 4, 13, 6, 10, 15, 2, 17,
        3, 19, 7, 16, 8, 11, 14, 9, 12, 5};

    const double radius = std::hypot(x, y);
    if (!std::isfinite(radius) || radius > kBoardOuterRadiusMm) return {};
    if (radius <= kInnerBullRadiusMm)
        return {Ring::InnerBull, 25, 2, 50, "BULL"};
    if (radius <= kOuterBullRadiusMm)
        return {Ring::OuterBull, 25, 1, 25, "25"};

    // atan2(x, -y) makes 0 degrees point up and positive angles clockwise.
    double degrees = std::atan2(x, -y) * 180.0 / std::acos(-1.0);
    if (degrees < 0.0) degrees += 360.0;
    const int sectorIndex = static_cast<int>(std::floor(degrees / 18.0 + 0.5)) % 20;
    const int number = clockwiseFromTop[static_cast<std::size_t>(sectorIndex)];

    Ring ring = Ring::Single;
    int multiplier = 1;
    if (radius >= kTrebleInnerRadiusMm && radius <= kTrebleOuterRadiusMm) {
        ring = Ring::Treble;
        multiplier = 3;
    } else if (radius >= kDoubleInnerRadiusMm) {
        ring = Ring::Double;
        multiplier = 2;
    }
    const char* prefix = ring == Ring::Treble ? "T" : ring == Ring::Double ? "D" : "S";
    return {ring, number, multiplier, number * multiplier,
            std::string(prefix) + std::to_string(number)};
}

} // namespace dart_scoring
