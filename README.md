# EDColony (C++ / Linux)

Легкий демон для колонізаційних даних Elite Dangerous (Linux, Proton) + подальший оверлей через Vulkan Layer (у майбутніх кроках).

## Збірка

Вимоги:
- CMake >= 3.22
- GCC >= 12 або Clang >= 15
- Linux x86_64

Залежності (встановіть системні, решта підтягнеться FetchContent):
- Ubuntu/Debian:
  - `sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config sqlite3 libsqlite3-dev`

Команди:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Запуск демона:
```bash
EDCOLONY_CMDR="Your CMDR" EDCOLONY_SVC_URI="http://localhost:8080" ./build/daemon/edcolony_daemon
```

Конфіг через змінні середовища (за замовчуванням порожні):
- EDCOLONY_SVC_URI — базовий URL бекенда колонізації
- EDCOLONY_JOURNAL_DIR — каталог журналів Elite Dangerous (у Proton-префіксі)

## Структура
- core/ — доменні моделі й базова логіка (C++20, без зовн. залежностей)
- daemon/ — мінімальний воркер (скелет), далі: inotify, HTTP клієнт, кеш
- .cursor/rules — правила для розробки під Linux/Proton/оверлей

## Подальші кроки
- Додати парсер журналів (inotify) і HTTP-клієнт до бекенда
- Додати IPC (Unix domain sockets) для обміну з оверлеєм
- Реалізувати Vulkan implicit layer оверлей (окремий модуль)
