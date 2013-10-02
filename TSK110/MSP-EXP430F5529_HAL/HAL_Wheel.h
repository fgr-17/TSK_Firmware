
#ifndef HAL_WHEEL_H
#define HAL_WHEEL_H

#include <stdint.h>

extern void Wheel_init(void);
extern uint8_t Wheel_getPosition(void);
extern uint16_t Wheel_getValue(void);
extern void Wheel_disable(void);
extern void Wheel_enable(void);

extern volatile uint16_t positionData;
extern volatile uint16_t positionDataOld;

#endif /* HAL_WHEEL_H */
