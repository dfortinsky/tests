#include <unordered_map>
#include <list>

#include <stdexcept>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace ::testing;


class LRUcache {
	unordered_map<int, list<pair<int, int> >::iterator> mHashMap;
	list<pair<int, int> > mList;

	const size_t mCapacity;
public:
	LRUcache(const size_t capacity)
	: mCapacity(capacity) {
		if (capacity == 0) throw invalid_argument("zero capacity");

		mHashMap.reserve(capacity);
	}

	int find(const int key) {
		auto it = mHashMap.find(key);
		if (it == mHashMap.end()) {
			return -1;
		}

		mList.insert(mList.begin(), move(*(it->second)));
		mList.erase(it->second);
		it->second = mList.begin();

		return mList.front().second;
	}

	void store(const int key, const int value) {
		auto it = mHashMap.find(key);
		if (it != mHashMap.end()) {
			mList.insert(mList.begin(), make_pair(key, value));
			mList.erase(it->second);
			it->second = mList.begin();

			return;
		}

		if (mHashMap.size() == mCapacity) {
			const auto& lit = --mList.end();
			mHashMap.erase(lit->first);
			mList.erase(lit);
		}

		mList.insert(mList.begin(), make_pair(key, value));
		mHashMap.emplace(key, mList.begin());

		return;
	}

	size_t capacity() const {
		return mCapacity;
	}
};

TEST(LRUcache, Positive_SimpleQuery) {
	LRUcache cache(8);

	cache.store(5, 78);
	cache.store(9, 345);

	EXPECT_EQ(345, cache.find(9));
	EXPECT_EQ(78, cache.find(5));
}

TEST(LRUcache, Positive_ChechLRU) {
	LRUcache cache(32);

	vector<pair<int, int> > values;
	values.reserve(50);

	for (int i = 0; i < 50; ++i) {
		values.emplace_back(make_pair(i + 456, 3 * i - 14));
	}


	for (int i = 0; i < values.size(); ++i) {
		cache.store(values[i].first, values[i].second);
	}

	for (int i = values.size() - 1; i >= 0; --i) {
		if (i >= values.size() - cache.capacity()) {
			EXPECT_EQ(values[i].second, cache.find(values[i].first));
		} else {
			EXPECT_EQ(-1, cache.find(values[i].first));
		}
	}
}

int main(int argc, char** argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}