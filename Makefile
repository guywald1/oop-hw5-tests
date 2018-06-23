PART2 = ${CURDIR}/part2
TEST = ${CURDIR}/test
DIST = ${CURDIR}/dist
CC=g++
CFLAGS=--std=c++11

.PHONY: all
all: test_part2
	./dist/a.out

.PHONY: test_part2
test_part2:
	$(CC) $(CFLAGS) $(TEST)/part2/*.cpp -o $(DIST)/test_part2
	$(DIST)/test_part2
	rm $(DIST)/test_part2
