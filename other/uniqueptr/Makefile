GTEST_LIB =
GTEST_INCLUDE =

all: run

run: link
	./uniqueptr

link: clean gtest
	g++ uniqueptr.cpp -std=c++14 -pthread \
		${GTEST_INCLUDE} ${GTEST_LIB} \
		-o uniqueptr

gtest:
	@# git clone https://github.com/google/googletest.git
	@# cd googletest
	@# mkdir build
	@# cd build
	@# cmake ..
	@# make

	@ls "${PWD}/googletest/build/usr/local/lib/libgtest.a"
GTEST_LIB = "${PWD}/googletest/build/usr/local/lib/libgtest.a" "${PWD}/googletest/build/usr/local/lib/libgmock.a"
GTEST_INCLUDE = -I"${PWD}/googletest/googletest/include" -I"${PWD}/googletest/googlemock/include"

clean:
	@rm -f uniqueptr