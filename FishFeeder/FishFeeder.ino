#include <Servo.h>
#include <EEPROM.h>
#include <LowPower.h>

// Настройки
#define FEED_PERIOD 12    // Период кормёжки В ЧАСАХ
#define PORTION_AMOUNT 2 // Количество порций корма за 1 раз
#define FEEDER_POSITION 70  //  Позиция серво под кормушкой
#define FISHTANK_POSITION 10  // Позиция серво над отверстием

// Пины
#define SERVO_PW 6
#define SERVO_CT 4
#define BTN_PIN 2

int realPeriod;
int sleedAmount;

int servoFeederPos = FEEDER_POSITION;
int servoFishtankPos = FISHTANK_POSITION;
Servo myservo;

volatile bool isrState = false;

void setup()
{
  periodSetup();
  buttonSetup();
  servoSetup();
}

void loop()
{          
  sleep();      
  feed(); 
}

void periodSetup()
{
  if (EEPROM.read(1000) != 50)
  {
    realPeriod = calibrateWDT();
    EEPROM.write(1000, 50);
    EEPROM.put(2, realPeriod);
  }
  
  EEPROM.get(2, realPeriod); 
  sleedAmount = (float)FEED_PERIOD * 3600 / realPeriod * 1000;
}

void buttonSetup()
{
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(0, isrHandler, FALLING);
  delay(300);
  isrState = false;
}

void servoSetup()
{
  myservo.attach(SERVO_CT);
  pinMode(SERVO_PW, OUTPUT); 
}

void feed()
{
  digitalWrite(SERVO_PW, HIGH);
  
  for (int i = 0; i < PORTION_AMOUNT; i++)
  {
    for (int pos = servoFeederPos; pos >= servoFishtankPos ; pos -= 1)
    {
      myservo.write(pos);
      delay(5);
    }
    delay(500);
    
    for (int pos = servoFishtankPos; pos <= servoFeederPos; pos += 1)
    {
      myservo.write(pos);
      delay(5);
    }
    delay(500);        
  }

  digitalWrite(SERVO_PW, LOW);
}

void sleep()
{
  isrState = false;
  
  for (int i = 0; i < sleedAmount; i++)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    if (isrState) 
    {
      delay(2000);
      if (!digitalRead(BTN_PIN))
      {  
          break;
      }
      else
      {
        isrState = false;  
      }
    }      
  }    
}

void isrHandler() 
{
  if (!isrState) isrState = true;
}

int calibrateWDT()
{
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0x47;
  asm ("wdr");
  uint16_t startTime = millis();
  while (!(WDTCSR & (1 << WDIF)));
  uint16_t ms = millis() - startTime;
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 0;
  return ms * 4;
}
