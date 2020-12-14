/**
 * @file utils.h
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 * @date 14.11.20
 * @brief File containing useful functions that are needed by the entired application.
 */

#ifndef NIBO_UTILS_H
#define NIBO_UTILS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Threshold for obstacles considered to be near a distance sensor.
 */
#define NEAR        (uint16_t) 0x00a0

/**
 * threshold for obstacles considered to be very near a distance sensor
 */
#define VERY_NEAR   (uint16_t) 0x01c0

/**
 * Enum for nibo's current state.
 */
enum state {
    ALLEY,
    DEAD_END,
    OBSTACLE_AHEAD,
    OBSTACLE_LEFT_AHEAD,
    OBSTACLE_RIGHT_AHEAD,
    FREE
};

/**
 * Enum for the position of distance sensors.
 */
enum distance_sensors {
    DS_LEFT = 4,
    DS_FRONT_LEFT = 3,
    DS_FRONT = 2,
    DS_FRONT_RIGHT = 1,
    DS_RIGHT = 0
};

/**
 * Enum for the position of LEDs.
 */
enum leds {
    LEDS_LEFT = 2,
    LEDS_FRONT_LEFT = 3,
    LEDS_FRONT_FRONT_LEFT = 4,
    LEDS_FRONT_FRONT_RIGHT = 5,
    LEDS_FRONT_RIGHT = 6,
    LEDS_RIGHT = 7
};

enum state get_current_state();

bool is_near(enum distance_sensors sensor, uint16_t threshold);

uint8_t request_distance_data();

#endif //NIBO_UTILS_H
