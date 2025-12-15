/*
 * Погодная автономная станция с предсказанием погоды
 * 
 * Реализация для ESP32 с использованием принципов чистого кода
 * 
 * Автор: Weather Station Team
 * Лицензия: MIT
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// Конфигурационные константы
namespace WeatherStationConfig {
    const char* WIFI_SSID = "WEATHER_STATION_AP";
    const char* WIFI_PASSWORD = "weather123";
    const int DHT_PIN = 4;
    const int DHT_TYPE = DHT22;
    const unsigned long MEASUREMENT_INTERVAL = 60000; // 1 минута
    const unsigned long ARCHIVE_INTERVAL = 604800000; // 7 дней в миллисекундах
    const size_t MAX_DATA_POINTS = 1008; // Количество точек данных за неделю при интервале 10 мин
}

using namespace WeatherStationConfig;

// Структура для хранения погодных данных
struct WeatherData {
    float temperature;      // Температура в градусах Цельсия
    float humidity;         // Влажность в процентах
    float pressure;         // Давление в Паскалях
    float predictedTemp;    // Предсказанная температура
    float predictedHumidity; // Предсказанная влажность
    unsigned long timestamp; // Временная метка
};

// Интерфейс для сенсоров
class ISensor {
public:
    virtual ~ISensor() = default;
    virtual bool initialize() = 0;
    virtual bool readData(WeatherData& data) = 0;
};

// Реализация DHT сенсора
class DHTSensor : public ISensor {
private:
    DHT* dht;
    int pin;
    int type;

public:
    DHTSensor(int sensorPin, int sensorType) : pin(sensorPin), type(sensorType) {
        dht = nullptr;
    }

    ~DHTSensor() {
        if (dht) {
            delete dht;
        }
    }

    bool initialize() override {
        dht = new DHT(pin, type);
        if (!dht) {
            return false;
        }
        
        dht->begin();
        return true;
    }

    bool readData(WeatherData& data) override {
        if (!dht) {
            return false;
        }

        float humidity = dht->readHumidity();
        float temperature = dht->readTemperature();

        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Failed to read from DHT sensor!");
            return false;
        }

        data.humidity = humidity;
        data.temperature = temperature;
        return true;
    }
};

// Реализация BMP280 сенсора
class BMP280Sensor : public ISensor {
private:
    Adafruit_BMP280* bmp;

public:
    BMP280Sensor() {
        bmp = nullptr;
    }

    ~BMP280Sensor() {
        if (bmp) {
            delete bmp;
        }
    }

    bool initialize() override {
        bmp = new Adafruit_BMP280();
        if (!bmp) {
            return false;
        }

        if (!bmp->begin()) {
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            return false;
        }

        /* Default settings from datasheet */
        bmp->setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
        return true;
    }

    bool readData(WeatherData& data) override {
        if (!bmp) {
            return false;
        }

        data.pressure = bmp->readPressure();
        return true;
    }
};

// Класс для управления сенсорами
class SensorManager {
private:
    ISensor* dhtSensor;
    ISensor* bmp280Sensor;

public:
    SensorManager() : dhtSensor(nullptr), bmp280Sensor(nullptr) {}

    ~SensorManager() {
        if (dhtSensor) {
            delete dhtSensor;
        }
        if (bmp280Sensor) {
            delete bmp280Sensor;
        }
    }

    bool initialize() {
        dhtSensor = new DHTSensor(DHT_PIN, DHT_TYPE);
        bmp280Sensor = new BMP280Sensor();

        bool dhtSuccess = dhtSensor->initialize();
        bool bmpSuccess = bmp280Sensor->initialize();

        return dhtSuccess && bmpSuccess;
    }

    bool readAllSensors(WeatherData& data) {
        bool success = true;
        
        if (dhtSensor) {
            success &= dhtSensor->readData(data);
        } else {
            Serial.println("DHT sensor not initialized");
            success = false;
        }

        if (bmp280Sensor) {
            success &= bmp280Sensor->readData(data);
        } else {
            Serial.println("BMP280 sensor not initialized");
            success = false;
        }

        data.timestamp = millis();
        return success;
    }
};

// Класс для хранения и архивирования данных
class DataStorage {
private:
    File dataFile;
    DynamicJsonDocument jsonBuffer{1024};
    WeatherData dataHistory[WeatherStationConfig::MAX_DATA_POINTS];
    size_t dataCount = 0;
    unsigned long lastArchiveTime = 0;

public:
    DataStorage() : dataCount(0), lastArchiveTime(0) {}

    bool initialize() {
        if (!SPIFFS.begin(true)) {
            Serial.println("An error occurred while mounting SPIFFS");
            return false;
        }
        return true;
    }

    void saveData(const WeatherData& data) {
        // Добавляем новые данные в историю
        if (dataCount < WeatherStationConfig::MAX_DATA_POINTS) {
            dataHistory[dataCount] = data;
            dataCount++;
        } else {
            // Если массив полон, сдвигаем элементы
            for (size_t i = 0; i < WeatherStationConfig::MAX_DATA_POINTS - 1; i++) {
                dataHistory[i] = dataHistory[i + 1];
            }
            dataHistory[WeatherStationConfig::MAX_DATA_POINTS - 1] = data;
        }

        // Архивируем данные каждые 7 дней
        if (millis() - lastArchiveTime > ARCHIVE_INTERVAL) {
            archiveWeeklyData();
            lastArchiveTime = millis();
        }
    }

    void archiveWeeklyData() {
        String filename = "/archive_" + String(millis()) + ".json";
        File file = SPIFFS.open(filename, "w");
        if (!file) {
            Serial.println("Failed to create archive file");
            return;
        }

        JsonArray root = jsonBuffer.to<JsonArray>();
        for (size_t i = 0; i < dataCount && i < WeatherStationConfig::MAX_DATA_POINTS; i++) {
            JsonObject dataPoint = root.createNestedObject();
            dataPoint["temp"] = dataHistory[i].temperature;
            dataPoint["humidity"] = dataHistory[i].humidity;
            dataPoint["pressure"] = dataHistory[i].pressure;
            dataPoint["predicted_temp"] = dataHistory[i].predictedTemp;
            dataPoint["predicted_humidity"] = dataHistory[i].predictedHumidity;
            dataPoint["timestamp"] = dataHistory[i].timestamp;
        }

        serializeJson(root, file);
        file.close();
        
        Serial.println("Data archived to " + filename);
        
        // Сброс истории после архивации
        dataCount = 0;
    }

    size_t getStoredDataCount() const {
        return dataCount;
    }

    WeatherData* getDataHistory(size_t& count) {
        count = dataCount;
        return dataHistory;
    }
};

// Класс для прогнозирования погоды
class WeatherPredictor {
private:
    // Простой алгоритм прогнозирования на основе трендов
    float tempTrend = 0.0f;
    float humidityTrend = 0.0f;

public:
    void updateTrends(const WeatherData& current, const WeatherData& previous) {
        if (current.timestamp > previous.timestamp) {
            tempTrend = (current.temperature - previous.temperature) / 
                       ((current.timestamp - previous.timestamp) / 1000.0f);
            humidityTrend = (current.humidity - previous.humidity) / 
                           ((current.timestamp - previous.timestamp) / 1000.0f);
        }
    }

    void predictWeather(WeatherData& data, const WeatherData& current) {
        // Простая экстраполяция на основе трендов
        data.predictedTemp = current.temperature + (tempTrend * 3600); // Прогноз на 1 час вперед
        data.predictedHumidity = current.humidity + (humidityTrend * 3600);

        // Ограничиваем значения в разумных пределах
        data.predictedTemp = constrain(data.predictedTemp, -50.0f, 60.0f);
        data.predictedHumidity = constrain(data.predictedHumidity, 0.0f, 100.0f);
    }
};

// Класс для веб-сервера
class WebServer {
private:
    WiFiServer* server;
    WiFiClient client;
    WeatherData* latestData;
    
public:
    WebServer() : server(nullptr), latestData(nullptr) {}
    
    bool initialize() {
        server = new WiFiServer(80);
        if (!server) {
            return false;
        }
        
        server->begin();
        Serial.println("Web server started");
        Serial.print("Connect to http://");
        Serial.println(WiFi.localIP());
        
        return true;
    }
    
    void setLatestData(WeatherData* data) {
        latestData = data;
    }
    
    void handleClient() {
        if (!server) return;
        
        WiFiClient newClient = server->available();
        if (newClient) {
            if (client && client.connected()) {
                client.stop();
            }
            client = newClient;
            
            String request = client.readStringUntil('\r');
            client.flush();
            
            if (request.indexOf("/data") >= 0) {
                sendWeatherData();
            } else if (request.indexOf("/predict") >= 0) {
                sendPredictionData();
            } else {
                sendMainPage();
            }
        }
    }
    
private:
    void sendMainPage() {
        String html = "<!DOCTYPE html><html><head>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        html += "<title>Погодная станция</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f8ff; }";
        html += ".container { max-width: 800px; margin: 0 auto; }";
        html += ".card { background-color: white; padding: 20px; margin: 10px 0; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
        html += "h1 { color: #2c3e50; text-align: center; }";
        html += ".data-item { display: flex; justify-content: space-between; padding: 10px 0; border-bottom: 1px solid #eee; }";
        html += ".label { font-weight: bold; color: #34495e; }";
        html += ".value { color: #2980b9; }";
        html += "</style>";
        html += "</head><body>";
        html += "<div class='container'>";
        html += "<h1>Погодная станция ESP32</h1>";
        html += "<div class='card'>";
        html += "<h2>Текущие показания</h2>";
        
        if (latestData) {
            html += "<div class='data-item'><span class='label'>Температура:</span> <span class='value'>" + String(latestData->temperature) + " °C</span></div>";
            html += "<div class='data-item'><span class='label'>Влажность:</span> <span class='value'>" + String(latestData->humidity) + " %</span></div>";
            html += "<div class='data-item'><span class='label'>Давление:</span> <span class='value'>" + String(latestData->pressure) + " Pa</span></div>";
            html += "<div class='data-item'><span class='label'>Предсказ. темп.:</span> <span class='value'>" + String(latestData->predictedTemp) + " °C</span></div>";
            html += "<div class='data-item'><span class='label'>Предсказ. влаж.:</span> <span class='value'>" + String(latestData->predictedHumidity) + " %</span></div>";
            html += "<div class='data-item'><span class='label'>Время:</span> <span class='value'>" + String(latestData->timestamp) + " ms</span></div>";
        } else {
            html += "<p>Данные недоступны</p>";
        }
        
        html += "</div>";
        html += "<div class='card'>";
        html += "<h2>Графики</h2>";
        html += "<p>Для просмотра графиков перейдите на <a href='/data'>страницу данных</a></p>";
        html += "</div>";
        html += "</div>";
        html += "</body></html>";
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.print(html);
    }
    
    void sendWeatherData() {
        String jsonString = "{";
        if (latestData) {
            jsonString += "\"temperature\":" + String(latestData->temperature) + ",";
            jsonString += "\"humidity\":" + String(latestData->humidity) + ",";
            jsonString += "\"pressure\":" + String(latestData->pressure) + ",";
            jsonString += "\"predictedTemp\":" + String(latestData->predictedTemp) + ",";
            jsonString += "\"predictedHumidity\":" + String(latestData->predictedHumidity) + ",";
            jsonString += "\"timestamp\":" + String(latestData->timestamp);
        } else {
            jsonString += "\"error\":\"No data available\"";
        }
        jsonString += "}";
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:application/json");
        client.println("Connection: close");
        client.println();
        client.print(jsonString);
    }
    
    void sendPredictionData() {
        String jsonString = "{";
        if (latestData) {
            jsonString += "\"predictedTemp\":" + String(latestData->predictedTemp) + ",";
            jsonString += "\"predictedHumidity\":" + String(latestData->predictedHumidity);
        } else {
            jsonString += "\"error\":\"No prediction data available\"";
        }
        jsonString += "}";
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:application/json");
        client.println("Connection: close");
        client.println();
        client.print(jsonString);
    }
};

// Основные компоненты системы
SensorManager sensorManager;
DataStorage dataStorage;
WeatherPredictor weatherPredictor;
WebServer webServer;
WeatherData previousData = {0, 0, 0, 0, 0, 0};
unsigned long lastMeasurementTime = 0;

void setup() {
    Serial.begin(115200);
    
    // Инициализация компонентов
    if (!sensorManager.initialize()) {
        Serial.println("Failed to initialize sensors");
        return;
    }
    
    if (!dataStorage.initialize()) {
        Serial.println("Failed to initialize data storage");
        return;
    }
    
    // Настройка Wi-Fi
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    IPAddress IP = WiFi.softAPIP();
    Serial.println("Access Point started");
    Serial.print("IP address: ");
    Serial.println(IP);
    
    if (!webServer.initialize()) {
        Serial.println("Failed to initialize web server");
        return;
    }
    
    Serial.println("Weather station initialized successfully");
}

void loop() {
    // Проверяем, пора ли делать новое измерение
    if (millis() - lastMeasurementTime > MEASUREMENT_INTERVAL) {
        WeatherData currentData = {0, 0, 0, 0, 0, 0};
        
        if (sensorManager.readAllSensors(currentData)) {
            // Обновляем тренды и делаем прогноз
            weatherPredictor.updateTrends(currentData, previousData);
            weatherPredictor.predictWeather(currentData, currentData);
            
            // Сохраняем данные
            dataStorage.saveData(currentData);
            
            // Обновляем предыдущие данные
            previousData = currentData;
            
            // Обновляем последние данные для веб-сервера
            webServer.setLatestData(&currentData);
            
            Serial.println("Data collected:");
            Serial.println("  Temperature: " + String(currentData.temperature) + " °C");
            Serial.println("  Humidity: " + String(currentData.humidity) + " %");
            Serial.println("  Pressure: " + String(currentData.pressure) + " Pa");
            Serial.println("  Predicted Temp: " + String(currentData.predictedTemp) + " °C");
            Serial.println("  Predicted Humidity: " + String(currentData.predictedHumidity) + " %");
        } else {
            Serial.println("Failed to read sensors");
        }
        
        lastMeasurementTime = millis();
    }
    
    // Обрабатываем HTTP-запросы
    webServer.handleClient();
    
    // Даем системе немного передохнуть
    delay(10);
}