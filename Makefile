CXX = clang++
override CXXFLAGS += -g -Wno-everything

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.cpp')
HEADERS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.h')

scanner: scanner.cpp
	$(CXX) $(CXXFLAGS) scanner.cpp -o scanner

test1: scanner scannertest1
	./scanner < scannertest1

test2: scanner scannertest2
	./scanner < scannertest2

clean:
	rm -f scanner
