#include <AFMotor.h>

AF_DCMotor motor1(1); //right
AF_DCMotor motor2(2); //left
AF_DCMotor motor3(3); //left
AF_DCMotor motor4(4); //right

char bt='S';
int s=255;
void setup()
{
  Serial.begin(9600);
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  Stop();
}


void loop() {
 
bt=Serial.read();

if(bt=='F')
{
 forward(); 
}

if(bt=='B')
{
 backward(); 
}

//left1
if(bt=='L')
{
 left1(); 
}

if(bt=='K')
{
 left2(); 
}

if(bt=='J')
{
 left3(); 
}

if(bt=='H')
{
 left4(); 
}

if(bt=='G')
{
 left5(); 
}

if(bt=='R')
{
 right1(); 
}

if(bt=='T')
{
 right2(); 
}

if(bt=='Y')
{
 right3(); 
}

if(bt=='U')
{
 right4(); 
}

if(bt=='I')
{
 right5(); 
}

if(bt=='S')
{
 Stop(); 
}


}
void forward() //F
{
  motor1.setSpeed(125);
  motor2.setSpeed(125);
  motor3.setSpeed(125);
  motor4.setSpeed(125);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void backward() //B
{
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}
void left1() //L
{
  motor1.setSpeed(255);
  motor2.setSpeed(160);
  motor3.setSpeed(160);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void left2() //K
{
  motor1.setSpeed(255);
  motor2.setSpeed(120);
  motor3.setSpeed(120);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void left3() //J
{
  motor1.setSpeed(150);
  motor2.setSpeed(150);
  motor3.setSpeed(150);
  motor4.setSpeed(150);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);
}


void left4() //H
{
  motor1.setSpeed(255);
  motor2.setSpeed(120);
  motor3.setSpeed(120);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void left5() //G
{
  motor1.setSpeed(255);
  motor2.setSpeed(90);
  motor3.setSpeed(90);
  motor4.setSpeed(255);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void right1() //R
{
  motor1.setSpeed(160);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(160);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}
void right2() //T
{
  motor1.setSpeed(120);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(120);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}
void right3() //Y
{
  motor1.setSpeed(150);
  motor2.setSpeed(150);
  motor3.setSpeed(150);
  motor4.setSpeed(150);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
}
void right4() //U
{
  motor1.setSpeed(120);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(120);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void right5() //I
{
  motor1.setSpeed(90);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(90);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void Stop()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}
