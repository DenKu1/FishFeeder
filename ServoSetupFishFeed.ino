#include <Servo.h>

#define SERVO_PW 6
#define SERVO_CT 4
  
int pos = 0;    // variable to store the servo position
int ser_pos_feeder = 60; //  position under food tank 
int ser_pos_fishtank = 10; //  position over feeding hole 

Servo myservo;  // create servo object to control a servo 

void setup() {
    myservo.attach(SERVO_CT);
    pinMode(SERVO_PW, OUTPUT);
   // digitalWrite(SERVO_PW, HIGH);   // Turn powersource to servo ON   
   // myservo.write(ser_pos_feeder); 
}

void loop() {
  digitalWrite(SERVO_PW, HIGH);
   for (pos = ser_pos_feeder; pos >= ser_pos_fishtank ; pos -= 1) {  
    myservo.write(pos);
    delay(3);   
  }
  delay(500);  

   for (pos = ser_pos_fishtank; pos <= ser_pos_feeder; pos += 1) { 
    myservo.write(pos);    
  }
   delay(500);  
  digitalWrite(SERVO_PW, LOW);

  delay(3000);
}
