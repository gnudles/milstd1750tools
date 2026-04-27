#!/bin/bash
cd sim1750
for f in tests/*.ldm; do
  echo "Testing $f"
  # Just load and run a simple test for sanity
  echo -e "tld $f\ngo\nquit" | ./sim1750 > /dev/null
  if [ $? -eq 0 ]; then echo "$f PASSED"; else echo "$f FAILED"; fi
done
for f in tests/xtc/*.ldm; do
  echo "Testing $f"
  # Just load and run a simple test for sanity
  echo -e "xtc $f\ngo\nquit" | ./sim1750 > /dev/null
  if [ $? -eq 0 ]; then echo "$f PASSED"; else echo "$f FAILED"; fi
done
for f in tests/*.hex; do
  echo "Testing $f"
  # Just load and run a simple test for sanity
  echo -e "load $f\ngo\nquit" | ./sim1750 > /dev/null
  if [ $? -eq 0 ]; then echo "$f PASSED"; else echo "$f FAILED"; fi
done
