#ifndef __LED_H
#define __LED_H

#define LED1_PIN GPIO_PIN_1
#define LED1_PORT GPIOA

void LED_Toggle();
void LED_ON();
void LED_OFF();

#endif