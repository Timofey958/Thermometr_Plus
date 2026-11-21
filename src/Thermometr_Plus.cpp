#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHT22_PIN 2
#define BRIGHTNESS_PIN 10
#define BUTTONS_PIN A0
#define UPDATE_TIME 2000

uint32_t timer = 0; // переменная, хранящая миллисекунды за прошлый цикл

int currentButton = -1;
int lastButton = -1;
/*
  -1 - ничего не нажато
  1 - кнопка UP (x > 10 и x < 110)
  2 - кнопка DOWN (x > 200 и x < 265)
*/ 

int brightness = 255; // яркость

bool errorScreen = false; // флажок экрана ошибки

LiquidCrystal lcd(8, 9, 4, 5, 6 , 7); // такие пины т.к. работает на lcd шилде
DHT dht(DHT22_PIN, DHT22);

byte temperature_symbol[] = { // значок температуры
  B00100,
  B01100,
  B00100,
  B01100,
  B00100,
  B01110,
  B01110,
  B00100
};

byte humidity_symbol[] = { // значок влажности
  B00000,
  B00100,
  B00100,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

void setup() {
  Serial.begin(9600); // для отладки

  pinMode(BRIGHTNESS_PIN, OUTPUT);
  analogWrite(10, 255); // чтобы при включении ярко было

  // инициализация
  lcd.begin(16, 2);
  dht.begin();
 
  // создание своих символов
  lcd.createChar(0, temperature_symbol);
  lcd.createChar(1, humidity_symbol);

  // чтобы при включении не было пустого экрана
  lcd.home();
  lcd.write(byte(0));
  lcd.print("Tem-ture ");

  lcd.setCursor(0, 1);
  lcd.write(byte(1));
  lcd.print("Humidity");
}

void loop() {
  int buttons = analogRead(BUTTONS_PIN);

  if (millis() - timer >= UPDATE_TIME) { // проверка по времени (вместо delay())
    timer = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    if (isnan(humidity) || isnan(temperature)) {
      if (!errorScreen) { // эта проверка нужна, чтобы экран при ошибке не мигал
        lcd.clear();
        lcd.print("Error of sensor!");
        lcd.setCursor(0, 1);
        lcd.print("Check its pin 2");

        errorScreen = true;
      }
    }

    else {
    errorScreen = false;

    lcd.home(); // основная часть проги
    lcd.write(byte(0));
    lcd.print("Tem-ture ");
    lcd.setCursor(10, 0);
    lcd.print(temperature);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    lcd.print("Humidity");
    lcd.setCursor(10, 1);
    lcd.print(humidity);
    lcd.print("%");
    }
  } if (buttons > 10 && buttons < 110) currentButton = 1; // обработка кнопок
  else if (buttons > 200 && buttons < 265) currentButton = 2;
  else currentButton = -1;

  if (currentButton != -1 && currentButton != lastButton) { // регулировка яркости
    if (currentButton == 1) brightness += 10;
    if (currentButton == 2) brightness -= 10;
  } lastButton = currentButton;

  if (brightness > 255) brightness = 255; // чтобы число не ушло в плюс или минус
  if (brightness < 5) brightness = 0; // я бы мог использовать constrain, но тогда экран моргать будет при достижении лимитов

  analogWrite(BRIGHTNESS_PIN, brightness);
}