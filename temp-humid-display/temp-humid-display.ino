#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <LittleFS.h>

// Define I2C LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor type and pin
#define DHTTYPE DHT11  // or DHT22
#define DHTPIN 4       // GPIO4 (A2) for DHT sensor data pin

DHT dht(DHTPIN, DHTTYPE);
int i = 0;
int j = 0;

byte loading1[] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x18
};

byte loading2[] = {
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03,
  0x03
};

byte degree[] = {
  0x06,
  0x09,
  0x09,
  0x06,
  0x00,
  0x00,
  0x00,
  0x00
};

// Function to initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

// Function to log data
void logData(float temperature, float humidity) {
  File file = LittleFS.open("/data_log.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temperature, humidity);
  file.close();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Initializing...");
  int y = 0;

  lcd.createChar(0, loading1);
  lcd.createChar(1, loading2);
  lcd.createChar(2, degree);

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

  // Log data to file
  logData(temperature, humidity);

  delay(2000);  // Wait a few seconds between measurements.
}
