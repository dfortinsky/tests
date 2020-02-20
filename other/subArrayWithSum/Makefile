GTEST_LIBS =
GTEST_INCLUDES =

all: run

run: link
	./subArrayWithSum

link: clean gtest
	g++ subArrayWithSum.cpp -std=c++14 -O3 -pthread $(CFLAGS) \
		${GTEST_INCLUDES} ${GTEST_LIBS} \
		-o subArrayWithSum

gtest:
	@# git clone https://github.com/google/googletest.git
	@# cd googletest
	@# mkdir build
	@# cd build
	@# cmake ..
	@# make

	@ls "${PWD}/googletest/build/lib/libgtest.a"
GTEST_LIBS = "${PWD}/googletest/build/lib/libgtest.a" "${PWD}/googletest/build/lib/libgmock.a"
GTEST_INCLUDES = -I"${PWD}/googletest/googletest/include" -I"${PWD}/googletest/googlemock/include"

clean:
	@rm -f subArrayWithSum