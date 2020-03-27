GTEST_LIB =
GTEST_INCLUDE =

all: run

run: link
	./LRUcache

link: clean gtest
	g++ LRUcache.cpp -std=c++14 -pthread \
		${GTEST_INCLUDE} ${GTEST_LIB} \
		-o LRUcache

gtest:
	@# git clone https://github.com/google/googletest.git
	@# cd googletest
	@# mkdir build
	@# cd build
	@# cmake ..
	@# make

	@ls "${PWD}/googletest/build/lib/libgtest.a"
GTEST_LIB = "${PWD}/googletest/build/lib/libgtest.a" "${PWD}/googletest/build/lib/libgmock.a"
GTEST_INCLUDE = -I"${PWD}/googletest/googletest/include" -I"${PWD}/googletest/googlemock/include"

clean:
	@rm -f LRUcache