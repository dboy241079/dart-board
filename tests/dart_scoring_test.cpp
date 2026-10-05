#include "../include/dart_scoring.hpp"

#include <cassert>

int main() {
    using dart_scoring::Ring;
    using dart_scoring::score_xy_mm;

    assert(score_xy_mm(0, 0).points == 50);
    assert(score_xy_mm(0, -10).label == "25");
    assert(score_xy_mm(0, -50).label == "S20");
    assert(score_xy_mm(0, 103).label == "T3");
    assert(score_xy_mm(0, -165).label == "D20");
    assert(score_xy_mm(0, -175).ring == Ring::Miss);
    assert(score_xy_mm(0, -50).points == 20);
    assert(score_xy_mm(0, -103).points == 60);
    assert(score_xy_mm(0, -165).points == 40);

    // Sector order and orientation around the board.
    assert(score_xy_mm(50, 0).label == "S6");
    assert(score_xy_mm(-50, 0).label == "S11");
    assert(score_xy_mm(0, 50).label == "S3");
    assert(score_xy_mm(1, -103).label == "T20");
    assert(score_xy_mm(-100, 1).label == "T11");
}
