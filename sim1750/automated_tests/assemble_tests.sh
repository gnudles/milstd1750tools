#!/bin/bash
for f in *.s; do
  echo "Compiling $f"
  ../../as1750/as1750 -al -o "${f%.s}.ldm" "$f" > /dev/null
done
