all: run

run: link
	./Storage

link: clean
	g++ Storage.cpp -std=c++14 -o Storage

clean:
	rm -f Storage