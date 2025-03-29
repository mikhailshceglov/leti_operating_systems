#!/bin/bash

dir_name="test_os_7"

# Создаём каталог
mkdir -p "$dir_name"
echo "Created directory: $dir_name"
echo "Initial stat:"
stat "$dir_name"

# Создаём много файлов
cd "$dir_name"
touch filler_{1..1000}
echo
echo "After creating 1000 files:"
cd ..
stat "$dir_name"

# Удаляем все файлы
cd "$dir_name"
rm filler_*
echo
echo "After removing files:"
cd ..
stat "$dir_name"

# Завершаем
echo
echo "Experiment completed."
