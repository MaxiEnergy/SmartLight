// Подключение библиотек для работы с BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Подключение библиотек для работы со светодиодной матрицей
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

#define Light_Sensor 13 // Пин подключения модуля фоторезистора (датчика освещенности)
#define PIN_RELAY 15 // Пин подключения одноканального реле для управления электрохромной пленкой

// Пины светодиода, информирующего о подключении устройства к смартфону по BLE
#define LED1_1 12
#define LED1_2 14
#define LED1_3 27

// Пины светодиода, информирующего о режиме работы устройства
#define LED2_1 22
#define LED2_2 1
#define LED2_3 3

String Msg = ""; // Переменная для сообщения пользователя в виде бегущей строки
int showType = 3; // Переменная для выбора стиля свечения матрицы
bool autoMode = true; // Переменная-флаг для режима работы устройства
int lightThreshold = 70; // Переменная, обозначающая порог значения освещенности
bool deviceConnected = false; // Переменная-флаг для подключения-отключения устройства по BLE
unsigned long previousMillis = 0; // Переменная для хранения времени последнего выполнения кода в миллисекундах
const int sampleInterval = 1000; // Константа для задания интервала времени между выполнениями кода в миллисекундах

Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, 23, NEO_GRB + NEO_KHZ800); // Инициализация объекта strip типа Adafruit_NeoPixel для управления 64 светодиодами (матрица 8 на 8) на пине 23 с параметрами цвета и частоты
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 23, NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT +  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG, NEO_GRB  + NEO_KHZ800); // Инициализация объекта matrix типа Adafruit_NeoMatrix для управления 64 светодиодами на пине 23 с различными параметрами ориентации и распределения

// Массив colors типа uint16_t, содержащий один цвет, созданный с помощью метода Color() объекта matrix
const uint16_t colors[] = {
  matrix.Color(200, 200, 200)
};

int x = matrix.width(); // Инициализация переменной x типа int и присвоение ей значения ширины матрицы светодиодов, полученное с помощью метода width() объекта matrix

//Функция для работы с кириллицей
String utf8rus(String source) {
  int i, k; // Объявление переменных i и k для управления циклами и индексации
  String target; // Объявление строки target, в которой будет храниться результат
  unsigned char n; // Объявление переменной n для хранения текущего символа
  char m[2] = {'0', '\0'}; // Инициализация массива m для хранения текущего символа в виде строки

  k = source.length(); // Получение длины исходной строки source
  i = 0; // Инициализация счетчика i значением 0

  while (i < k) { // Цикл обработки каждого символа в строке source
    n = source[i]; // Получение текущего символа из строки source
    i++; // Увеличение счетчика i

    if (n >= 0xBF) { // Проверка, является ли символ частью многобайтового символа
      switch (n) { // Обработка многобайтовых символов
        case 0xD0: { // Обработка символов с первым байтом 0xD0
          n = source[i]; // Получение следующего байта
          i++; // Увеличение счетчика i
          if (n == 0x81) { n = 0xA8; break; } // Обработка диапазона символов
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F; // Обработка диапазона символов
          break;
        }
        case 0xD1: { // Обработка символов с первым байтом 0xD1
          n = source[i]; // Получение следующего байта
          i++; // Увеличение счетчика i
          if (n == 0x91) { n = 0xB7; break; } // Обработка диапазона символов
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F; // Обработка диапазона символов
          break;
        }
      }
    }
    m[0] = n; // Присваивание текущего символа массиву m
    target = target + String(m); // Добавление текущего символа к результату
  }
  return target; // Возвращение результата
}

//Функция для реализации бегущей строки
void runLineChange(){
  digitalWrite(PIN_RELAY, HIGH); // Подача тока на электрохромную пленку посредством включения реле
  matrix.begin(); // Инициализация матрицы NeoMatrix
  matrix.setTextWrap(false); // Отключение автоматического переноса текста
  matrix.setBrightness(10); // Установка яркости матрицы (от 0 до 255)                     
  matrix.setTextColor(colors[0]); // Установка цвета текста на первый цвет в массиве colors
  for (int i = 0; i <= matrix.width() + 6 * Msg.length(); i++) { // Цикл для анимации бегущего строки
    matrix.fillScreen(10); // Заполнение всей матрицы заданным цветом (10)
    matrix.setCursor(x, 0); // Установка позиции курсора для вывода текста
    matrix.print(utf8rus(Msg)); // Вывод текста, преобразованного функцией utf8rus
    if(--x < -100) { // Условие для сброса позиции текста                               
      x = matrix.width(); // Возврат к начальной позиции
    }
    matrix.show(); // Обновление матрицы
    delay(150); // Задержка для анимации
  }
  digitalWrite(PIN_RELAY, LOW); // Прекращение подачи тока на электрохромную пленку посредством выключения реле
}

//Функция для градиентного свечения
void rainbow(uint8_t wait) {
  uint16_t i, j; // Объявление счетчиков i и j
  for (j = 0; j < 256; j++) { // Внешний цикл для прохода по всему диапазону цветов (0-255)
    for(i = 0; i < strip.numPixels(); i++) { // Внутренний цикл для прохода по всем светодиодам матрицы
      strip.setPixelColor(i, Wheel((i+j) & 255)); // Установка цвета для каждого светодиода матрицы с использованием функции Wheel
    }
    strip.setBrightness(25); // Установка яркости светодиодов
    strip.show(); // Обновление цвета светодиодов
    delay(wait); // Задержка для анимации градиентного свечения
  }
}

// Функция для преобразования значения WheelPos в цвет в формате RGB
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos; // Инвертирование значения WheelPos
  if (WheelPos < 85) { // Если WheelPos меньше 85, создается градиент от синего к красному
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) { // Если WheelPos между 85 и 170, создается градиент от синего к зеленому
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170; // Если WheelPos больше 170, создается градиент от зеленого к красному
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Функция для установки одного цвета для всех светодиодов матрицы
void setColor(uint32_t c) {
  for(uint16_t i = 0; i < strip.numPixels(); i++) { // Цикл для прохода по всем светодиодам матрицы
    strip.setPixelColor(i, c); // Установка цвета для каждого светодиода
  }
  strip.show(); // Обновление цвета светодиодов
}

//Функция для выбора режима работы светодиодной матрицы
void startShow(int i) {
  switch(i){
    case 1: setColor(strip.Color(255, 255, 255)); //Включение белого свечения матрицы
            break;
    case 2: setColor(strip.Color(0, 0, 0)); //Выключение матрицы
            break;
    case 3: rainbow(20); //Запуск функции градиентного свечения
            break;
    case 4: runLineChange(); //Запуск функции бегущей строки
            break;
  }
}

//Функция реализации автоматического режима работы устройства
void controlLed() {
  int lightValue = analogRead(Light_Sensor); // Чтение аналогового значения с датчика освещенности
  lightValue = 100 - (lightValue / 40.95); // Преобразование считанного значения в проценты (0-100)
  if (autoMode) { // Проверка, активирован ли автоматический режим
    if (lightValue < lightThreshold) { // Если уровень освещенности ниже заданного порога
      showType = 1; // Установка типа анимации на 1 (включение матрицы)
      startShow(showType); // Запуск анимации с заданным типом
    } else { // Если уровень освещенности выше или равен заданному порогу
        showType = 2; // Установка типа анимации на 2 (выключение матрицы)
        startShow(showType); // Запуск анимации с заданным типом
      }
  }
}

BLECharacteristic* pLevelLightCharacteristic; // Указатель на характеристику BLE для уровня освещенности
BLECharacteristic* pLedCharacteristic; // Указатель на характеристику BLE для переключения режима работы устройства
BLECharacteristic* pLedManualCharacteristic; // Указатель на характеристику BLE для ручного управления матрицей
BLECharacteristic* pPDLCCharacteristic; // Указатель на характеристику BLE для управления электрохромной пленкой
BLECharacteristic* pRunLineCharacteristic; // Указатель на характеристику BLE для запуска бегущей строки

BLEServer* pServer; // Указатель на BLE сервер

// Класс для обработки событий BLE сервера
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) { // Функция, вызываемая при подключении устройства к BLE серверу
    deviceConnected = true; // Установка флага, указывающего на то, что устройство подключено
  };
  void onDisconnect(BLEServer* pServer) { // Функция, вызываемая при отключении устройства от BLE сервера
    deviceConnected = false; // Сброс флага, указывающего на то, что устройство подключено
  }
};

// Класс для обработки записи в характеристику BLE режима работы устройства
class LedCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) { // Функция, вызываемая при записи значения в характеристику
    std::string value = pCharacteristic->getValue(); // Получение значения, записанного в характеристику
    if (value.length() > 0) { // Проверка длины полученного значения
      if (value[0] == 0x01) { // Если первый байт равен 0x01
        autoMode = true; // Включение автоматического режима
        digitalWrite(LED2_1, 0); // На светодиоде устанавливается зеленое свечение
        digitalWrite(LED2_2, 0);
        digitalWrite(LED2_3, 255);
        controlLed(); // Вызов функции для автоматического управления устройством
      } else { // Если первый байт не равен 0x01
          autoMode = false; // Отключение автоматического режима
          digitalWrite(LED2_1, 0); //Отключение светодиода
          digitalWrite(LED2_2, 0);
          digitalWrite(LED2_3, 0);
          showType = 2; // Установка типа анимации на 2 (выключение матрицы)
          startShow(showType); // Запуск анимации с заданным типом
        }
    }
  }
};

// Класс для обработки записи в характеристику BLE для управления электрохромной пленкой
class PDLCCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) { // Функция, вызываемая при записи значения в характеристику
    std::string value = pCharacteristic->getValue(); // Получение значения, записанного в характеристику
    if (value.length() > 0) { // Проверка длины полученного значения
      if (value[0] == 0x01) { // Если первый байт равен 0x01
        digitalWrite(PIN_RELAY, LOW); // Выключение реле - выключение электрохромной пленки
      } else { // Если первый байт не равен 0x01
          digitalWrite(PIN_RELAY, HIGH); //Включение реле - включение электрохромной пленки
        }
    } else { // Если значение не было записано (пустая строка)
        digitalWrite(PIN_RELAY, LOW); // Выключение реле - выключение электрохромной пленки
      }
  }
};

// Класс для обработки записи в характеристику BLE, отвечающую за "бегущую строку"
class RunLineCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) { // Функция, вызываемая при записи значения в характеристику
    std::string value = pCharacteristic->getValue(); // Получение значения, записанного в характеристику
    if (value.length() > 0) { // Проверка длины полученного значения
      Msg = value.c_str(); // Преобразование полученного значения в строку и сохранение в переменную Msg
      showType = 4; // Установка типа анимации на "бегущую строку" (4)
      startShow(showType); // Запуск анимации "бегущая строка"
      showType = 2; //Выключение матрицы
      startShow(showType); //Запуск режима с выключением матрицы
    } else { // Если значение не было записано (пустая строка)
        showType = 2; //Выключение матрицы
        startShow(showType); //Запуск режима с выключением матрицы
      }
  }
};

// Класс для обработки записи в характеристику BLE, отвечающую за ручной режим управления устройством
class LedManualCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) { // Функция, вызываемая при записи значения в характеристику
    std::string value = pCharacteristic->getValue(); // Получение значения, записанного в характеристику
    if (autoMode == false) { // Проверка, находится ли устройство в автоматическом режиме
      if (value.length() > 0) { // Проверка длины полученного значения
        if (value[0] == 1) { // Если первый байт равен 1
          showType = 1; // Установка типа анимации (1) - включение матрицы
          startShow(showType); // Запуск анимации
        } else if (value[0] == 2){ // Если первый байт равен 2
            showType = 3; // Установка типа анимации (3) - градиентное свечение
            startShow(showType); // Запуск анимации
          } else if (value[0] == 0){ // Если первый байт равен 0
              showType = 2; //Установка типа анимации (2) - выключение матрицы
              startShow(showType); // Запуск анимации
            }
      }
    } else { // Если устройство находится в автоматическом режиме
        controlLed(); // Вызов функции для автоматического управления светодиодами
      }
  }
};

// Функция setup(), вызывается один раз при запуске устройства
void setup() {

  // Назначение пинов светодиода для подключения устройства на роль выходов (для управления)
  pinMode(LED1_1, OUTPUT);
  digitalWrite(LED1_1, 0);
  pinMode(LED1_2, OUTPUT);
  digitalWrite(LED1_2, 0);
  pinMode(LED1_3, OUTPUT);
  digitalWrite(LED1_3, 255); // Установка светодиода на синее свечение

  // Назначение пинов светодиода для режима устройства на роль выходов (для управления)
  pinMode(LED2_1, OUTPUT);
  digitalWrite(LED2_1, 0);
  pinMode(LED2_2, OUTPUT);
  digitalWrite(LED2_2, 0);
  pinMode(LED2_3, OUTPUT);
  digitalWrite(LED2_3, 255); // Установка светодиода на зеленое свечение

  strip.begin(); // Инициализация объекта strip, который управляет матрицей
  strip.show(); // Отображение начального состояния матрицы

  BLEDevice::init("SmartLight"); // Инициализация BLE устройства с именем "SmartLight"
  pServer = BLEDevice::createServer(); // Создание BLE сервера для обработки соединений и событий
  pServer->setCallbacks(new MyServerCallbacks()); // Установка колбэков (обратных вызовов) для сервера, чтобы обрабатывать события сервера

  BLEService* pLevelLightService = pServer->createService(BLEUUID((uint16_t)0x170D)); // Создание Bluetooth сервиса с указанным UUID (0x170D) - сервис уровня освещенности
  pLevelLightCharacteristic = pLevelLightService->createCharacteristic(BLEUUID((uint16_t)0x2A31), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY); // Создание характеристики для сервиса с указанным UUID (0x2A31) и установка ей свойств READ и NOTIFY
  BLEDescriptor* pDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2902)); // Создание дескриптора для характеристики с указанным UUID (0x2902)
  pLevelLightCharacteristic->addDescriptor(pDescriptor); // Добавление дескриптора к характеристике

  BLEService* pLedService = pServer->createService(BLEUUID((uint16_t)0x180A)); // Создание Bluetooth сервиса с указанным UUID (0x180A) - сервис режима работы устройства
  pLedCharacteristic = pLedService->createCharacteristic(BLEUUID((uint16_t)0x2A56), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE); // Создание характеристики (переключение режима работы) для сервиса с указанным UUID (0x2A56) и установка ей свойств READ и WRITE
  pLedCharacteristic->setCallbacks(new LedCharacteristicCallbacks()); // Установка колбэков (обратных вызовов) для характеристики для обработки событий чтения и записи

  pLedManualCharacteristic = pLedService->createCharacteristic(BLEUUID((uint16_t)0x2A59), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE); // Создание характеристики (управление в ручном режиме) для сервиса с указанным UUID (0x2A59) и установка ей свойств READ и WRITE
  pLedManualCharacteristic->setCallbacks(new LedManualCharacteristicCallbacks()); // Установка колбэков (обратных вызовов) для характеристики для обработки событий чтения и записи

  pPDLCCharacteristic = pLedService->createCharacteristic(BLEUUID((uint16_t)0x2A60), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE); // Создание характеристики (управление электрохромной пленкой) для сервиса с указанным UUID (0x2A60) и установка ей свойств READ и WRITE
  pPDLCCharacteristic->setCallbacks(new PDLCCharacteristicCallbacks()); // Установка колбэков (обратных вызовов) для характеристики для обработки событий чтения и записи

  pRunLineCharacteristic = pLedService->createCharacteristic(BLEUUID((uint16_t)0x2A61), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE); // Создание характеристики (управление бегущей строкой) для сервиса с указанным UUID (0x2A61) и установка ей свойств READ и WRITE
  pRunLineCharacteristic->setCallbacks(new RunLineCharacteristicCallbacks()); // Установка колбэков (обратных вызовов) для характеристики для обработки событий чтения и записи

  pLevelLightService->start(); // Запуск сервиса уровня освещенности
  pLedService->start(); // Запуск сервиса режима работы устройства

  BLEAdvertising* pAdvertising = pServer->getAdvertising(); // Получение объекта Advertising из BLE сервера
  pAdvertising->addServiceUUID(pLevelLightService->getUUID()); // Добавление UUID (уникальных идентификаторов) сервиса уровня освещенности
  pAdvertising->addServiceUUID(pLedService->getUUID()); // Добавление UUID (уникальных идентификаторов) сервиса режима работы устройства
  pAdvertising->setScanResponse(false); // Установка параметра Scan Response в false - отключение сканирующего ответа
  pAdvertising->setMinPreferred(0x06); // Установка минимального предпочтительного интервала для сканирования
  pAdvertising->setMinPreferred(0x12); // Установка максимального предпочтительного интервала для сканирования
  BLEDevice::startAdvertising(); // Видимость для других устройств

  pinMode(PIN_RELAY, OUTPUT); //Назначения пина реле на роль выхода для управления
  digitalWrite(PIN_RELAY, HIGH); //Включение реле - включение электрохромной пленки

  // Алгоритм приветствия устройства при включении
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);
  String HelloMsg = utf8rus("Привет! Меня зовут SmartLight. Меня собрал Максим. Рад вам светить!");
  for (int i = 0; i <= matrix.width() + 6 * HelloMsg.length(); i++) {
    matrix.fillScreen(10);
    matrix.setCursor(x, 0);
    matrix.print(HelloMsg);
    if(--x < -410) {  
      x = matrix.width();
    }
    matrix.show();
    delay(80);
  }
  digitalWrite(PIN_RELAY, LOW); 
}

void loop() {
  unsigned long currentMillis = millis();

  // Считывание значения с датчика освещенности и преобразование его в процентное значение
  int lightValue = analogRead(Light_Sensor);
  lightValue = 100 - (lightValue / 40.95);

  if (deviceConnected) { // Проверка, подключено ли какое-либо устройство по BLE
    pLevelLightCharacteristic->setValue(lightValue); // Установка значения характеристики с освещенностью 
    pLevelLightCharacteristic->notify(); // Передача данных по уровню защищенности в виде уведомления
    digitalWrite(LED1_2, 255); // Переключение светодиода подключения на зеленый цвет
    digitalWrite(LED1_3, 0);
  } else { // Если устройство не подключено
      digitalWrite(LED1_2, 0);
      digitalWrite(LED1_3, 255); // Светодиод подключения светится синим
      BLEDevice::startAdvertising(); // Видимость для других устройств
    }

  delay(sampleInterval); // Задержка перед следующей итерацией цикла

  controlLed(); // Вызов функции управления матрицей на основе режима
  startShow(showType); // Вызов функции для анимации матрицы (для работы матрицы)
}