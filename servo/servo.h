/* servomotion
 *
 * servo.h
 *
 *  Created on: Jun 22, 2014
 *      (c) 2014 Tuwuh Sarwoprasojo
 *      All rights reserved.
 */

#ifndef SERVO_H_
#define SERVO_H_

#define SERVO_LOG2_SMOOTHING_FACTOR (0)

#define MAX_SERVO      (10000)
#define MIN_SERVO      (-10000)
#define MAX_SERVO_US   (2000)
#define MIN_SERVO_US   (1000)

void servo_init(void);
void servo_set_position(uint8_t id, int16_t pos);

#endif /* SERVO_H_ */
