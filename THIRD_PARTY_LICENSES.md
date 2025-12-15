# Лицензии сторонних библиотек

Проект погодной станции использует следующие сторонние библиотеки:

## Используемые библиотеки

### 1. ArduinoJson
- Автор: Benoit Blanchon
- Лицензия: MIT License
- Описание: Библиотека для парсинга и генерации JSON
- Используется для сериализации и десериализации данных датчиков

### 2. Adafruit BMP280 Library
- Автор: Adafruit Industries
- Лицензия: BSD License
- Описание: Библиотека для работы с датчиком атмосферного давления BMP280
- Используется для чтения данных о давлении

### 3. DHT sensor library
- Автор: Adafruit Industries
- Лицензия: MIT License
- Описание: Библиотека для работы с датчиками DHT11, DHT22 и т.д.
- Используется для чтения данных о температуре и влажности

### 4. Adafruit Unified Sensor
- Автор: Adafruit Industries
- Лицензия: MIT License
- Описание: Унифицированный интерфейс для датчиков Adafruit
- Используется как основа для других датчиков

### 5. Adafruit BusIO
- Автор: Adafruit Industries
- Лицензия: MIT License
- Описание: Библиотека для работы с шинами I2C/SPI
- Используется для подключения датчиков по I2C

## Тексты лицензий

### MIT License
```
MIT License

Copyright (c) <year> <copyright holders>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### BSD License (для Adafruit BMP280 Library)
```
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
```

## Соблюдение лицензий

Все сторонние библиотеки используются в соответствии с условиями их лицензий. Проект не изменяет оригинальные библиотеки, а только использует их API в соответствии с документацией.

## Авторские права

Все авторские права на сторонние библиотеки принадлежат их соответствующим владельцам. Проект погодной станции не претендует на правообладание на сторонние библиотеки.