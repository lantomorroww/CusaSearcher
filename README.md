# CusaSearcher

[![Build Status](https://github.com/lantomorroww/CusaSearcher/actions/workflows/build.yml/badge.svg)](https://github.com/lantomorroww/CusaSearcher/actions)# CusaSearcher

[![Build and Test](https://github.com/lantomorroww/CusaSearcher/actions/workflows/build.yml/badge.svg)](https://github.com/lantomorroww/CusaSearcher/actions/workflows/build.yml)

Приложение на Qt/C++ для поиска игр PlayStation по названию, коду диска (CUSA/PPSA) и синонимам, с информацией о локализации (наличие русского языка, субтитров, озвучки).

## 🎯 Зачем этот проект

Демонстрация навыков:
- Работа с **C++17** и **Qt Widgets**.
- Парсинг и валидация **JSON** (nlohmann/json).
- Построение **двухуровневого индекса** (прямой и обратный) для быстрого поиска.
- Покрытие кода **Unit-тестами** (GoogleTest).
- Сборка через **CMake**, работа с **Git**.

## ✨ Возможности

- 🔍 Поиск по названию (частичное совпадение, без учёта регистра).
- 🔢 Поиск по коду диска (полному или только цифровой части).
- 🌐 Поиск по синонимам (русские названия, транслит).
- 🗺️ Отображение регионов и кодов дисков (Россия, Польша, Турция, Индия, Великобритания).
- 🇷🇺 Информация о локализации: полная озвучка / только субтитры / нет русского.
- ➕ Добавление новых игр через диалоговое окно.
- 🎨 Тёмная тема, фоновый GIF, музыка с регулировкой громкости.
- ⌨️ Автодополнение при вводе (`QCompleter`).

## 🧠 Архитектура (ключевые решения)

- **`FileHandler`** — загрузка игр из JSON с поддержкой двух форматов локализации (суффикс в коде и отдельное поле).
- **`SynonymIndex`** — обратный индекс «синоним → код». Поиск по синониму за **O(1)**.
- **Fallback-код** — если для региона указан только маркер локализации (`О`, `С`), код подставляется из другого региона.
- **Разделение логики и UI** — `FileHandler` и `SynonymIndex` не зависят от Qt (кроме строковых утилит), что упрощает тестирование.

## 🛠 Стек

- **C++17**
- **Qt 5.15** (Widgets, Multimedia)
- **nlohmann/json**
- **CMake**
- **GoogleTest**

## 🚀 Сборка и запуск

### Зависимости (Debian/Ubuntu)

```bash
sudo apt install build-essential cmake qtbase5-dev qtmultimedia5-dev nlohmann-json3-dev
