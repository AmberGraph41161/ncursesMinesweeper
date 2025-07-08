CXXFLAGS := -std=c++17 -Wall

main: build/main.o build/minesweeper.o build/saveloaddata.o
	clang++ $(CXXFLAGS) -lncurses -o main $^

build/main.o: src/main.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

build/minesweeper.o: src/minesweeper.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

build/saveloaddata.o: src/saveloaddata.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^
