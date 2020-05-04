#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


namespace test {

template<typename T>
struct default_delete {
public:
	constexpr default_delete() noexcept = default;

	void operator()(T* ptr) const {
		std::cout << "operator()(T*) " << this << std::endl;
		delete ptr;
	}
};


template<typename T, typename D = default_delete<T>>
class UniquePtr {
public:
	constexpr UniquePtr() noexcept
		: mPtr(nullptr) {
		std::cout << "UniquePtr() " << this << std::endl;
	}

	explicit UniquePtr(T* ptr) noexcept
		: mPtr(ptr) {
		std::cout << "UniquePtr(T*) " << this << std::endl;
	}

	UniquePtr(T* ptr, const D& deleter) noexcept
		: mPtr(ptr), mDeleter(deleter) {
		std::cout << "UniquePtr(T*, const D&) " << this << std::endl;
	}

	UniquePtr(T* ptr, D&& deleter) noexcept
		: mPtr(ptr), mDeleter(deleter) {
		std::cout << "UniquePtr(T*, D&&) " << this << std::endl;
	}
	
	UniquePtr(UniquePtr<T, D>&& ptr) noexcept
		: mPtr(nullptr) {
		std::cout << "UniquePtr(UniquePtr<T>&&) " << this << std::endl;
		swap(*this, ptr);
	}

	UniquePtr<T, D>& operator=(UniquePtr<T, D>&& ptr) noexcept {
		std::cout << "operator=(UniquePtr<T>&&) " << this << std::endl;
		swap(*this, ptr);

		return *this;
	}

	UniquePtr(UniquePtr<T, D>& ptr) = delete;

	UniquePtr<T, D>& operator=(UniquePtr<T, D>& ptr) = delete;

	~UniquePtr() {
		std::cout << "~UniquePtr() " << this << ", " << mPtr << std::endl;
		if (mPtr) mDeleter(mPtr);
	}


	T* get() const {
		return mPtr;
	}

	T* release() {
		T* temp = mPtr;
		mPtr = nullptr;

		return temp;
	}


	T* operator->() const {
		return mPtr;
	}

	T& operator*() const {
		return *mPtr;
	}

	explicit operator bool() const {
		return mPtr != nullptr;
	}


	friend void swap(UniquePtr<T, D>& a, UniquePtr<T, D>& b) noexcept {
		using std::swap;

		swap(a.mPtr, b.mPtr);
		swap(a.mDeleter, b.mDeleter);
	}

private:
	T* mPtr;
	D mDeleter;
};

} // namespace test

namespace {

TEST(UniquePtr, Constructor_Default) {
	test::UniquePtr<int> ptr1;
}

TEST(UniquePtr, Constructor_Pointer) {
	test::UniquePtr<int> ptr1(new int(5));
}

template<typename T>
struct custom_delete {
	void operator()(T* ptr) const noexcept {
		std::cout << "custom_delete::operator()(T*) " << this << std::endl;
		delete ptr;
	}
};

TEST(UniquePtr, Constructor_ObjectDeleter) {
	test::UniquePtr<int, custom_delete<int>> ptr1(new int(5), custom_delete<int>());
}

void deleteCustomly(int* ptr) {
	std::cout << "deleteCustomly(int*)" << std::endl;
	delete ptr;
}

TEST(UniquePtr, Constructor_FunctionalDeleter) {
	test::UniquePtr<int, void(*)(int*)> ptr1(new int(5), deleteCustomly);
}

TEST(UniquePtr, Constructor_LambdaDeleter) {
	const auto deleter = [](int* ptr) {
		std::cout << "lambda deleter(int*)" << std::endl;
		delete ptr;
	};
	test::UniquePtr<int, void(*)(int*)> ptr1(new int(5), deleter);
}


TEST(UniquePtr, MoveOwnership) {
	test::UniquePtr<int> ptr1(new int(5));
	auto ptr2(std::move(ptr1));

	EXPECT_EQ(nullptr, ptr1.get());
	EXPECT_NE(nullptr, ptr2.get());
}

} // namespace


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}