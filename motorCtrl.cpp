
#include "Particle.h"


int dirA1 = D3;
int dirA2 = D4;
int dirB1 = D5;
int dirB2 = D6;
int pwmA = A4;
int pwmB = A5;

void setupMotor()
{
    pinMode(dirA1, OUTPUT);
    pinMode(dirA2, OUTPUT);
    pinMode(dirB1, OUTPUT);
    pinMode(dirB2, OUTPUT);
    pinMode(pwmA, OUTPUT);
    pinMode(pwmB, OUTPUT);
}

void forward(bool dir)
{
    digitalWrite(dirA1, dir ? HIGH : LOW);
    digitalWrite(dirA2, dir ? LOW : HIGH);
    digitalWrite(dirB1, dir ? HIGH : LOW);
    digitalWrite(dirB2, dir ? LOW : HIGH);

    analogWrite(pwmA, 255);
    analogWrite(pwmB, 255);
    delay(1000);
    digitalWrite(dirB1, LOW);
    digitalWrite(dirB2, LOW);
    digitalWrite(dirA1, LOW);
    digitalWrite(dirA2, LOW);
    analogWrite(pwmB, 0);
    analogWrite(pwmA, 0);
}

void turn(bool dir)
{
    digitalWrite(dirA1, dir ? LOW : HIGH);
    digitalWrite(dirA2, dir ? HIGH : LOW);
    digitalWrite(dirB1, dir ? HIGH : LOW);
    digitalWrite(dirB2, dir ? LOW : HIGH);

    analogWrite(pwmA, 171);
    analogWrite(pwmB, 171);
}

void stop()
{
    digitalWrite(dirB1, LOW);
    digitalWrite(dirB2, LOW);
    digitalWrite(dirA1, LOW);
    digitalWrite(dirA2, LOW);
    analogWrite(pwmB, 0);
    analogWrite(pwmA, 0);
}
