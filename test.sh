#!/usr/bin/env bash

fail() {
  #make stop > /dev/null
  echo "Unexpected output"
  echo -----------------------------------
  echo Tests failed
  exit 1
}

echo Starting server
echo -----------------------------------

make build
nohup make background
sleep 1 # the worst way to wait for something to start

set -e

echo Starting tests
echo -----------------------------------

echo "List with empty shelves" && echo -n "  " && curl -s localhost:8080/list | grep -i empty || fail
echo "Create book 126-3256" && echo -n "  " && curl -s -d title localhost:8080/book/126-3256 | grep -i created || fail
echo "Should find book 126-3256" && echo -n "  " && curl -s localhost:8080/list | grep title || fail
echo "Delete copy of book 126-3256" && echo -n "  " && curl -s -X DELETE localhost:8080/book/126-3256 | grep -i deleted || fail
echo "Should not find book 126-3256" && echo -n "  " && curl -s localhost:8080/book/126-3256 | grep "not found" || fail
echo "Recreate book 126-3256" && echo -n "  " && curl -s -d title localhost:8080/book/126-3256 | grep -i created || fail
echo "Create another copy of book 126-3256" && echo -n "  " && curl -s -d title localhost:8080/book/126-3256 | grep -i created || fail
echo "Delete one copy of book 126-3256" && echo -n "  " && curl -s -X DELETE localhost:8080/book/126-3256 | grep -i deleted || fail
echo "Should still find book 126-3256" && echo -n "  " && curl -s localhost:8080/list | grep title || fail
echo "Change title on book 126-3256" && echo -n "  " && curl -s -d titler -X PATCH localhost:8080/book/126-3256 | grep -i updated || fail
echo "Should see updates to book 126-3256" && echo -n "  " && curl -s localhost:8080/book/126-3256 | grep titler || fail
echo "Create book 126-3256-123" && echo -n "  " && curl -s -d title1 localhost:8080/book/126-3256-123 | grep -i created || fail
echo "Create book 126-3256-124" && echo -n "  " && curl -s -d title2 localhost:8080/book/126-3256-124 | grep -i created || fail
echo "Create book 126-3256-125" && echo -n "  " && curl -s -d title3 localhost:8080/book/126-3256-125 | grep -i created || fail
echo "Should find book 126-3256-123" && echo -n "  " && curl -s localhost:8080/list | grep title1 || fail
echo "Should find book 126-3256-124" && echo -n "  " && curl -s localhost:8080/list | grep title2 || fail
echo "Should find book 126-3256-125" && echo -n "  " && curl -s localhost:8080/list | grep title3 || fail

echo -----------------------------------
echo Tests passed
make stop > /dev/null
