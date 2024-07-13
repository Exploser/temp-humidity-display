#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPIFFS.h>

// Define I2C LCD address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor type and pin
#define DHTTYPE DHT11  // or DHT22
#define DHTPIN 4       // GPIO4 (A2) for DHT sensor data pin

DHT dht(DHTPIN, DHTTYPE);

// Custom characters for loading animation and degree symbol
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

// Function to initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  } else {
    Serial.println("SPIFFS mounted successfully");
  }
}

// Function to log data
void logData(float temperature, float humidity) {
  File file = SPIFFS.open("/data_log.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  String logEntry = String("Temperature: ") + String(temperature) + " °C, Humidity: " + String(humidity) + " %\n";
  file.print(logEntry);
  file.close();
}

// Function to read and print log data
void readLogData() {
  File file = SPIFFS.open("/data_log.txt", FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

// Function to reset log file if too large
void resetLogIfTooLarge() {
  File file = SPIFFS.open("/data_log.txt", FILE_READ);
  if (!file) {
    Serial.println("Failed to open file to check size");
    return;
  }

  // Check file size (e.g., reset if larger than 100KB)
  if (file.size() > 100 * 1024) {
    file.close();
    SPIFFS.remove("/data_log.txt");
    Serial.println("Log file too large, resetting...");
  } else {
    file.close();
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize custom characters
  lcd.createChar(0, loading1);
  lcd.createChar(1, loading2);
  lcd.createChar(2, degree);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize DHT sensor
  dht.begin();

  // Initialize SPIFFS
  initSPIFFS();

  // Print initial message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp & Humidity");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);

  // Loading Animation
  for (int y = 0; y < 1; y++) {
    for (int i = 0; i < 16; i++) {
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.write(0);
      lcd.setCursor(i, 1);
      lcd.write(0);
      delay(50);
      lcd.clear();
      lcd.setCursor(i, 0);
      lcd.write(1);
      lcd.setCursor(i, 1);
      lcd.write(1);
      delay(50);
    }
  }

  // Read and print log data on Serial Monitor at startup
  readLogData();
}

void loop() {
  // Check for Serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any leading/trailing whitespace

    if (command == "GET_DATA") {
      Serial.println("Retrieving log data...");
      readLogData();
      return;
    }
  }

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Read Fail");
    delay(2000);  // Wait a few seconds before retrying
    return;
  }

  // // Print temperature and humidity to Serial Monitor
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.print(" °C, Humidity: ");
  // Serial.print(humidity);
  // Serial.println(" %");

  // Print temperature to LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp:  ");
  lcd.print(temperature);
  lcd.write(byte(0));
  lcd.print("C   ");  // Added spaces to clear previous characters

  // Print humidity to LCD
  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(humidity);
  lcd.print(" %   ");  // Added spaces to clear previous characters

  // Log data to file
  logData(temperature, humidity);

  // Reset log file if too large
  resetLogIfTooLarge();

  delay(2000);  // Wait a few seconds between measurements.
}
