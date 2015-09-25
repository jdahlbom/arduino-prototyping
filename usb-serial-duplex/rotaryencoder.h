#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

struct RotaryEncoder
{
  int pin0;
  int pin1;
  volatile int rotationState;
  volatile int previousState;
};

#endif ROTARY_ENCODER_H
