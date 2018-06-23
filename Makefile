PART2 = ${CURDIR}/part2
TEST = ${CURDIR}/test
CC=g++
CFLAGS=--std=c++11

all: test_part2

setup:
	mkdir -p src

.PHONY: test_part2
test_part2:
	$(CC) $(CFLAGS) $(TEST)/part2/*.cpp -o ./a.out
	./a.out
