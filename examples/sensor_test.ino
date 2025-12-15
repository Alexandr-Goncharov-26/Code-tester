/*
 * Пример скетча для тестирования датчиков погодной станции
 * 
 * Этот скетч позволяет протестировать работу датчиков по отдельности
 * перед запуском основной программы погодной станции.
 */

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// Конфигурация пинов
#define DHT_PIN 4
#define DHT_TYPE DHT22

// Создание объектов датчиков
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_BMP280 bmp; // Используем I2C

void setup() {
    Serial.begin(115200);
    delay(2000); // Ждем стабилизации питания
    
    Serial.println("Тестирование датчиков погодной станции");
    Serial.println("------------------------------------");
    
    // Инициализация DHT22
    dht.begin();
    Serial.println("DHT22 инициализирован");
    
    // Инициализация BMP280
    if (!bmp.begin()) {
        Serial.println("Не удалось найти BMP280. Проверьте подключение!");
    } else {
        Serial.println("BMP280 инициализирован");
        /* Default settings from datasheet */
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    }
    
    Serial.println("Начало тестирования...");
    Serial.println();
}

void loop() {
    // Чтение данных с DHT22
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    // Проверка корректности чтения
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Ошибка чтения с DHT22!");
    } else {
        Serial.print("Температура: ");
        Serial.print(temperature);
        Serial.print(" °C\t");

        Serial.print("Влажность: ");
        Serial.print(humidity);
        Serial.println(" %");
    }
    
    // Чтение данных с BMP280
    float pressure = bmp.readPressure();
    float tempFromBMP = bmp.readTemperature();
    
    if (pressure == 0) {
        Serial.println("Ошибка чтения с BMP280!");
    } else {
        Serial.print("Давление: ");
        Serial.print(pressure);
        Serial.print(" Pa\t");

        Serial.print("Темп.(BMP): ");
        Serial.print(tempFromBMP);
        Serial.println(" °C");
    }
    
    Serial.println("---");
    delay(2000); // Пауза между измерениями
}