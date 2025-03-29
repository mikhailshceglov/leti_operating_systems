#! /bin/bash

output_fname="$1"

> $output_fname

echo "output processing in file..."
ls -lR / 2> /dev/null > 1_tmp

for filetype in "\-" "b" "c" "d" "l" "p" "s"; do
  regex="^(\/.*:)$\n([^\/\n][a-zA-Z0-9:.,_\+>\/ \-]+\n)*?^($filetype.*)$"
  cat 1_tmp | pcregrep -o1 -o3 --om-separator=" " -M "$regex" | head -n 1 >> "$output_fname"
done

rm 1_tmp
