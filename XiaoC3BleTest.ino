#include <LiquidCrystal_I2C.h>
#include <NimBLEDevice.h>

#define LED_RED_PIN       2
#define LED_GREEN_PIN     3
#define LED_BLUE_PIN      4
#define RGBLED_RED_PIN    10
#define RGBLED_GREEN_PIN  9
#define RGBLED_BLUE_PIN   8

bool    is_state_connect;

uint8_t red;
uint8_t green;
uint8_t blue;

NimBLEServer *pServer;
NimBLEService *pService;
NimBLECharacteristic *pCharacteristic;
NimBLEAdvertising *pAdvertising;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(RGBLED_RED_PIN, OUTPUT);
  pinMode(RGBLED_GREEN_PIN, OUTPUT);
  pinMode(RGBLED_BLUE_PIN, OUTPUT);

  red = 0;
  green = 0;
  blue = 0;

  // SerialMonitor setting
  Serial.begin(19200);

  // BLE setting
  NimBLEDevice::init("State-Color-BLE");

  pServer = NimBLEDevice::createServer();
  pService = pServer -> createService("ABCD");
  pCharacteristic = pService -> createCharacteristic(
    "1234",
    NIMBLE_PROPERTY::WRITE,
    3
  );
  pAdvertising = NimBLEDevice::getAdvertising();

  pCharacteristic -> setValue(0x000000);

  pService -> start();
  
  pAdvertising -> addServiceUUID("ABCD");
  pAdvertising -> start();

  // LCD setting
  lcd.init();
  lcd.backlight();
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("connection.");
}

void loop() {
  if(pServer->getConnectedCount()) {
    // Runs only once when connecting.
    if(!is_state_connect){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Color:0x000000");
      lcd.setCursor(0, 1);
      lcd.print("RGB:000:000:000");
      is_state_connect = true;
    }
  } else {
    // Runs only once when disconnecting.
    if(is_state_connect){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Waiting for");
      lcd.setCursor(0, 1);
      lcd.print("connection.");
      red = 0;
      green = 0;
      blue = 0;
      is_state_connect = false;
    }
  }

  if(is_state_connect){
    const uint8_t* temp = pCharacteristic -> getValue().data();
    red = temp[0];
    green = temp[1];
    blue = temp[2];
    char str_buff[20];
    lcd.setCursor(0, 0);
    sprintf(str_buff,"Color:0x%02x%02x%02x", red, green, blue);
    lcd.print(str_buff);
    lcd.setCursor(0, 1);
    sprintf(str_buff,"RGB:%03d:%03d:%03d", red, green, blue);
    lcd.print(str_buff);
  }

  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
  analogWrite(RGBLED_RED_PIN, red);
  analogWrite(RGBLED_GREEN_PIN, green);
  analogWrite(RGBLED_BLUE_PIN, blue);
}
