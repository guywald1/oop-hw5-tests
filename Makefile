TEST = ${CURDIR}/test
CC=g++
CFLAGS=--std=c++11

all: test_part1 test_part2

setup:
	mkdir -p src

.PHONY: test_part1
test_part1:
	@echo "Testing part 1..."
	@echo "Generating part 1..."
	@python3 $(TEST)/part1/generate.py > ./generated.cpp
	@python3 $(TEST)/part1/buildTests.py > ./main.cpp
	@echo "Part 1 generated."
	@$(CC) $(CFLAGS) ./main.cpp -o ./a.out
	@./a.out && echo "*** PART 1 PASSED! ***"
	@rm ./generated.cpp
	@rm ./main.cpp
	@rm ./a.out

.PHONY: test_part2
test_part2:
	@echo "Testing part 2... (could take a while)"
	@$(CC) $(CFLAGS) $(TEST)/part2/*.cpp -o ./a.out
	@./a.out
	@rm ./a.out
