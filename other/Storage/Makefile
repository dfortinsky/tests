GTEST_LIB =
GTEST_INCLUDE =

all: run

run: link
	./Storage

link: clean gtest
	g++ Storage.cpp -std=c++14 -pthread \
		-I${GTEST_INCLUDE} ${GTEST_LIB} \
		-o Storage

gtest:
	@# git clone https://github.com/google/googletest.git
	@# cd googletest
	@# mkdir build
	@# cd build
	@# cmake ..
	@# make

	@ls "${PWD}/googletest/build/lib/libgtest.a"
GTEST_LIB = "${PWD}/googletest/build/lib/libgtest.a"
GTEST_INCLUDE = "${PWD}/googletest/googletest/include"

clean:
	@rm -f Storage