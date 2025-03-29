#!/bin/bash

#if [ $# -ne 1 ]; then
#  echo "Usage: $0 <filename>"
#  exit 1
#fi

#target_path=$(realpath "$1")
#echo "Absolute path: $target_path"
#echo "Found links:"

# Поиск всех символьных ссылок и сравнение их абсолютных путей
#find / -type l 2>/dev/null | while IFS= read -r link; do
#  link_target=$(readlink -f "$link")
#  if [ "$link_target" = "$target_path" ]; then
#    echo "$link"
#  fi
#done

# Приводим входной файл к абсолютному пути
target="$(readlink -f "$1")"

find / -type l 2>/dev/null | while read symlink; do
  # Приводим и саму ссылку к тому же виду
  link_target="$(readlink -f "$symlink")"
  if [ "$link_target" = "$target" ]; then
    echo "$symlink"
  fi
done

