#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define I2C LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor type and pin
#define DHTTYPE DHT11  // or DHT22
#define DHTPIN 4       // GPIO4 (A2) for DHT sensor data pin

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Initializing...");

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
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 0);
    lcd.print("Failed to read");
    lcd.setCursor(0, 1);
    lcd.print("sensor data   ");
    delay(2000);  // Wait a few seconds before retrying
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
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C   ");  // Added spaces to clear previous characters

  // Print humidity to LCD
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %   ");  // Added spaces to clear previous characters

  delay(2000);  // Wait a few seconds between measurements.
}
