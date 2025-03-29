#! /bin/bash
set -e

if [ $# -ne 1 ]; then
  echo "неверный формат <filename>"
  exit 1
fi

target_inode=$(ls -i $1 | grep -oE "^[0-9]+")
echo "Выбранный inode: $target_inode"

echo "Поиск..."
echo "Жесткие ссылки:"
ls -iR1 / 2> /dev/null | grep -E "^$target_inode "
