/**
 * @file controller.h
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 * @date 14.11.20
 * @brief File containing functions to control the nibo's behaviour.
 */

#ifndef NIBO2_CONTROLLER_H
#define NIBO2_CONTROLLER_H

int leave_dead_end();

void drive_in_alley();

void turn_right();

void turn_left();

void avoid_obstacle_ahead();

#endif //NIBO2_CONTROLLER_H
