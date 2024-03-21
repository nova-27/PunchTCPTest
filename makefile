all: build/main.out

build/main.out: main.cpp
	g++ main.cpp -g -o $@

clean:
	rm -f build/*