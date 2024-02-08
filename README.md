<img width="1354" alt="Снимок экрана 2024-02-08 в 17 16 01" src="https://github.com/MaxiEnergy/SmartLight/assets/58640177/217f9caa-c045-4b21-8cc6-f298f320a624">

# SmartLight
SmartLight является умным светильником без реализации механизмов защиты, созданным для демонстрации методики обеспечения безопасности Интернета вещей в рамках выпускной квалификационной работы. В качестве технологии беспроводной передачи данных используется BLE

## Содержание
- [Стек технологий и компонентов](#cтек-технологий-и-компонентов)
- [Описание функционала](#описание-функционала)
- [Схема подключения электронных компонентов](#схема-подключения-электронных-компонентов)
- [Корпус](#корпус)
- [Выпускная квалификационная работа](#выпускная-квалификационная-работа)
- [Дополнительные материалы](#дополнительные-материалы)

## Стек технологий и компонентов

### Программная часть
- Язык программирования C++
- Среда разработки Arduino IDE

### Аппаратная часть
- Модуль фоторезистора KY-018
- 2 модуля светодиода 5050 KY-009
- Светодиодная матрица WS2812B SK6812
- Однокональное электромеханическое реле
- DC/AC повышающий преобразователь напряжения
- Электрохромная пленка
- 2 элемента питания АА
- ESP32-WROOM-32 (NodeMCU-32)

## Описание функционала
- Включение и выключение светового элемента на устройстве
- Включение и выключение цветного градиентного свечения на устройстве
- Включение и выключение электрохромной пленки для реализации рассеянного и направленного свечения
- Вывод текстовой информации на устройство с автоматическим включением электрохромной пленки
- Сбор информации об уровне освещенности и автоматическое включение и выключение светового элемента
- Взаимодействие с мобильным устройством с помощью мобильного приложения Smart Connect посредством технологии BLE

## Схема подключения электронных компонентов
<img width="1236" alt="image" src="https://github.com/MaxiEnergy/SmartLight/assets/58640177/620d3f4b-6699-4834-aa00-5b086c41a80f">

## Корпус
Для данного устройства разработан корпус, который можно распечатать на 3D-принтере. 
Файлы с моделями частей корпуса устройства: 
1. Заднаяя крышка.stl
2. Передняя крышка.stl
3. Рамка.stl

### Чертежи частей корпуса
<img width="1728" alt="Снимок экрана 2024-02-01 в 20 51 50" src="https://github.com/MaxiEnergy/SmartLight/assets/58640177/0ca14287-b559-4404-b44d-dbb1fe594318">

### 3D-модель
<img width="1276" alt="image" src="https://github.com/MaxiEnergy/SmartLight/assets/58640177/4c69c93d-8576-48b7-831e-7f3e727b1591">

## Выпускная квалификационная работа
С оригиналом выпускной квалификационной работы можно ознакомиться по [ссылке](https://www.dropbox.com/scl/fi/evywal0odwmc1053ck2il/_.pdf?rlkey=i7d2i8soxyn0wn4oav7f0py9n&dl=0)

## Дополнительные материалы
Читайте про создание данного устройства в статье на Хабре

[IMG_0354-7.webm](https://github.com/MaxiEnergy/SmartLight/assets/58640177/3db7f97e-bdc2-4d69-b290-d2aef10bb262)

<video autoplay loop muted>
  <source src="https://github.com/MaxiEnergy/SmartLight/assets/58640177/3db7f97e-bdc2-4d69-b290-d2aef10bb262" type="video/webm">
</video>

