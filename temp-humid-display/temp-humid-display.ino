#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define I2C LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor type and pin
#define DHTTYPE DHT11  // or DHT22
#define DHTPIN 4       // GPIO4 (A2) for DHT sensor data pin

DHT dht(DHTPIN, DHTTYPE);
int i = 0;
int j = 0;

byte customChar1[] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18
};

byte customChar2[] = {
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03
};

byte customChar3[] = {
  0x0C,
  0x12,
  0x12,
  0x0C,
  0x00,
  0x00,
  0x00,
  0x00
};

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Initializing...");
  int y = 0;

  lcd.createChar(0, customChar1);
  lcd.createChar(1, customChar2);
  lcd.createChar(2, customChar3);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize DHT sensor
  dht.begin();

  // Print initial message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp & Humidity");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);

  // Loading Animation
  while (y < 1) {
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.write(1);
    lcd.setCursor(i, 1);
    lcd.write(1);
    delay(50);
    lcd.clear();
    lcd.setCursor(i, 0);
    lcd.write(2);
    lcd.setCursor(i, 1);
    lcd.write(2);
    delay(50);
    lcd.clear();
    i++;
    if (i >= 16) {
      i = 0;
      y++;
    }
  }
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int x = 0;

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    // Loading Animation
    while (x < 3) {
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.write(1);
      lcd.setCursor(i, 1);
      lcd.write(1);
      delay(50);
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.write(2);
      lcd.setCursor(i, 1);
      lcd.write(2);
      delay(50);
      lcd.clear();
      i++;
      if (i == 16) {
        i = 0;
        x++;
      }
    }
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 0);
    lcd.print("Failed to read");
    lcd.setCursor(0, 1);
    lcd.print("sensor data   ");
    delay(5000);  // Wait a few seconds before retrying
    return;
  }

  // Print temperature and humidity to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Print temperature to LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp:    ");
  lcd.print(temperature);
  lcd.write(byte(0));
  lcd.print("C   ");  // Added spaces to clear previous characters

  // Print humidity to LCD
  lcd.setCursor(0, 1);
  lcd.print("Humid:   ");
  lcd.print(humidity);
  lcd.print(" %   ");  // Added spaces to clear previous characters

  delay(2000);  // Wait a few seconds between measurements.
}
