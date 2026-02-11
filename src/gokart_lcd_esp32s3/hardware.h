#ifndef HARDWARE_H
#define HARDWARE_H

/* Pedals */
#define RIGHT_PEDAL_PIN 7
#define LEFT_PEDAL_PIN  8

/* Buttons */
#define RIGHT_BUTTON_PIN 9
#define LEFT_BUTTON_PIN  10

/* Relays */
#define LEFT_THREE_SPEED_IN1_BLUE    16
#define LEFT_THREE_SPEED_IN2_YELLOW  25
#define RIGHT_THREE_SPEED_IN3_BLUE   14
#define RIGHT_THREE_SPEED_IN4_YELLOW 24
#define LEFT_REVERSE_IN5_BROWN       26
#define RIGHT_REVERSE_IN6_BROWN      29

/* UART */
#define RX_PIN 44
#define TX_PIN 43

/* CAN */
#define CAN_TX_GPIO GPIO_NUM_21
#define CAN_RX_GPIO GPIO_NUM_20

#define HALL_SENSOR GPIO_NUM_3 //Digital output of hall sensor


#endif
