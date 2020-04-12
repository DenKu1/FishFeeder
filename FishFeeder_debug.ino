#include <Servo.h>
#include <EEPROM.h>
#include <LowPower.h>

// Настройки
#define FEED_PERIOD 12    // Период кормёжки В ЧАСАХ
#define PORTION_AMOUNT 3 // Количество порций корма за 1 раз
#define INVERSE_BUTTON 0  // 0 - норм. открытая, 1 - норм. замкнутая кнопка
#define FEEDER_POSITION 80  //  Позиция серво под кормушкой
#define FISHTANK_POSITION 25  // Позиция серво над отверстием

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
  Serial.begin(9600);
  periodSetup();
  buttonSetup();
  servoSetup();
}

void loop()
{        
  feed();    
  sleep();      
}

void periodSetup()
{
  if (EEPROM.read(1000) != 50)
  {
    Serial.println("First launch!");
    
    realPeriod = calibrateWDT();
    EEPROM.write(1000, 50);
    EEPROM.put(2, realPeriod);
  }

  EEPROM.get(2, realPeriod);
  sleedAmount = (float)FEED_PERIOD * 3600 / realPeriod * 1000;
  // Debug
  Serial.println("Period setupped! Sleep amount:");
  Serial.println(sleedAmount);  
  Serial.println("Real period");
  Serial.println(realPeriod);  
}

void buttonSetup()
{
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(0, isrHandler, INVERSE_BUTTON ? RISING : FALLING);
  delay(300);
  isrState = false; 
  // Debug
  Serial.println("Button setupped! isrState:");
  Serial.println(isrState);  
}

void servoSetup()
{
  myservo.attach(SERVO_CT);
  pinMode(SERVO_PW, OUTPUT);
  digitalWrite(SERVO_PW, HIGH);
  myservo.write(FEEDER_POSITION);
}

void feed()
{
  // Debug
  Serial.println("Start feeding!");
  
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

  //Debug
  Serial.println("End feeding");  
  
}

void sleep()
{
  //Debug
  Serial.println("Got to sleep mode!");  
  
  isrState = false;
  
  for (int i = 0; i < sleedAmount; i++)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    if (isrState) break;
  }  

  //Debug
  Serial.println("Back from sleep mode!");  
}

void isrHandler() {
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
