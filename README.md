# Лабораторная работа №1 по курсу "Операционные системы"

**Тема:** Файловые системы Linux-подобных ОС
**Автор:** Щеглов Михаил, группа 3381
**Дата выполнения:** март 2025
**ОС:** Debian GNU/Linux 6.1.0-30-amd64
**Процессор:** Intel Core i7-13650HX (20 потоков)

---

## Цель

Ознакомление с типами файлов и структурой файловых систем, правами доступа, системными утилитами анализа, исследование особенностей работы с>

---

## Структура проекта

```plaintext
.
├── 1_lab.pdf           # Условие лабораторной
├── lab1.log            # Основной лог (все задания, выводы, команды)
├── scripts/            # Скрипты для отдельных заданий
│   ├── lab1_task1.sh
│   ├── lab1_task2.sh
│   ├── lab1_task3.sh
│   ├── lab1_task7.sh
│   ├── lab1_task10.c
│   ├── SUID_TEST.c
│   └── suid_test_program

```
---

## Описание заданий

### Задание 1 — Типы файлов
Скрипт `lab1_task1.sh` ищет и сохраняет примеры всех типов файлов в корневой ФС (`-`, `b`, `c`, `d`, `l`, `p`, `s`) с использованием `ls -lR>

### Задание 2 — Жесткие ссылки
Поиск всех жёстких ссылок на файл через `stat`, `ls -iR` и `grep`. Скрипт `lab1_task2.sh`.

### Задание 3–4 — Символические ссылки
Создание символьных ссылок разными способами (`ln -s`, `cp -s`, `link`) и их автоматический поиск. Скрипт `lab1_task3.sh`.

### Задание 5 — Утилита `find`
Разнообразный поиск файлов по дате, размеру, имени, пустоте, через `-exec` и `-printf`.

### Задание 6 — Заголовки файлов
Анализ структуры PDF, Python и Pickle файлов, а также директорий с помощью `hexdump` и `od`.

### Задание 7 — Размер каталога
Эксперимент с изменением размера каталога при добавлении и удалении большого числа файлов. Скрипт `lab1_task7.sh`.

### Задание 8 — /etc/passwd, /etc/shadow
Анализ прав доступа и структуры этих файлов. Проверка прав доступа у `/usr/bin/passwd`.
