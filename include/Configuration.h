#ifndef CONFIGURATION_H
#define CONFIGURATION_H

namespace config {

static const int HEADER_FONT_SZ = 12;

static const double MIN_TRIANGLE_SZ = 0.05;
static const double MAX_TRIANGLE_SZ = 50.0;
static const double DELTA_TRIANGLE_SZ = 0.1;
static const double DEFAULT_TRIANGLE_SZ = 0.5;

static const double MIN_WALLS_HEIGHT = 0.05;
static const double MAX_WALLS_HEIGHT = 500.0;
static const double DELTA_WALLS_HEIGHT = 1.0;
static const double DEFAULT_WALLS_HEIGHT = 2.0;

static const double VIEWPORT_MIN_X = -500000.00;
static const double VIEWPORT_MAX_X = 500000.00;
static const double VIEWPORT_MIN_Y = -500000.00;
static const double VIEWPORT_MAX_Y = 500000.00;
static const double DEFAULT_VIEWPORT_MIN_X = -5.0;
static const double DEFAULT_VIEWPORT_MAX_X = 5.0;
static const double DEFAULT_VIEWPORT_MIN_Y = -5.0;
static const double DEFAULT_VIEWPORT_MAX_Y = 5.0;

} // namespace config

#endif // CONFIGURATION_H
