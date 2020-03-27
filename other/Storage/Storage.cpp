#include <unordered_map>
#include <vector>

#include <algorithm>
#include <iterator>

#include <chrono>
#include <mutex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace ::testing;

#define LAST_MINUTES 10

namespace {

class Time {
public:
	chrono::time_point<chrono::system_clock> now() const {
		return chrono::system_clock::now();
	}
};

template <typename TimeHelper>
class StorageBase {
	// We want to guess if page was loaded by robot or real user. We store clicks for user ID and
	// if there were very low amount (or event zero) thah was an robot. (How much is enougth is
	// out of scope.)
	// We want store a click when it happens, and sometime in future to query stored amount for
	// last 10 mins for user ID and all users.

	// Map/set are not effective in insertion (O(nlogn)) because of sorted nature of store() calls and
	// quering amount of clicks cant be O(1) because of std::distance() will take O(n).

	// Open question: when to erase old clicks? Obviosly store() calls will be frequent, so maybe
	// in getClicks()?

	unordered_map<unsigned int, vector<chrono::time_point<std::chrono::system_clock> > > records;
	mutex mute;

	const TimeHelper* timeHelper;
public:
	StorageBase(const TimeHelper* helper, const unsigned int& size = 1024)
	: timeHelper(helper) {
		records.reserve(size);
	}

	void store(const unsigned int& uid) {
		if (uid == 0) {
			return;
		}

		const chrono::time_point<std::chrono::system_clock> time = timeHelper->now();
		lock_guard<mutex> lock(mute);

		auto it = records.begin();
		if ((it = records.find(uid)) != records.end()) {
			it->second.emplace_back(time);
			return;
		}

		records.emplace(uid, vector<chrono::time_point<std::chrono::system_clock> >({ time }));
		return;
	}

	unsigned int getUserClicks(const unsigned int& uid) {
		if (uid == 0) {
			return 0;
		}

		const auto& earlier = timeHelper->now() - chrono::minutes(LAST_MINUTES);
		lock_guard<mutex> lock(mute);

		if (uid != 0) {
			auto it = records.begin();
			if ((it = records.find(uid)) != records.end()) {
				auto vit = lower_bound(it->second.begin(), it->second.end(), earlier);
				return distance(vit, it->second.end());
			}
		}

		return 0;
	}

	unsigned int getAllClicks() {
		const auto& earlier = timeHelper->now() - chrono::minutes(LAST_MINUTES);
		lock_guard<mutex> lock(mute);

		unsigned int clicks = 0;
		for (auto it = records.begin(); it != records.end(); ++it) {
			auto vit = lower_bound(it->second.begin(), it->second.end(), earlier);
			clicks += distance(vit, it->second.end());
		}

		return clicks;
	}
};

class TimeMock {
public:
	MOCK_CONST_METHOD0(now, chrono::time_point<chrono::system_clock>());
};

typedef StorageBase<TimeMock> TestedStorage;

TEST(Storage, Positive_Empty) {
	TimeMock timeMock;
	TestedStorage s(&timeMock);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(chrono::system_clock::now()));
	EXPECT_EQ(0, s.getAllClicks());
}

TEST(Storage, Positive_OneClick) {
	TimeMock timeMock;
	TestedStorage s(&timeMock, 16);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(chrono::system_clock::now()));
	s.store(23);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(chrono::system_clock::now()));
	EXPECT_EQ(1, s.getUserClicks(23));

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(chrono::system_clock::now()));
	EXPECT_EQ(0, s.getUserClicks(64));

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(chrono::system_clock::now()));
	EXPECT_EQ(1, s.getAllClicks());
}

TEST(Storage, Positive_SeveralClicks) {
	TimeMock timeMock;
	TestedStorage s(&timeMock, 16);

	const chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();


	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now - chrono::hours(2)));
	s.store(78);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now - chrono::minutes(45)));
	s.store(45);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now - chrono::minutes(10)));
	s.store(67);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now - chrono::minutes(7)));
	s.store(45);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now - chrono::minutes(5)));
	s.store(78);

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now));
	s.store(78);


	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now));
	EXPECT_EQ(1, s.getUserClicks(45));

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now));
	EXPECT_EQ(1, s.getUserClicks(67));

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now));
	EXPECT_EQ(2, s.getUserClicks(78));

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now));
	EXPECT_EQ(4, s.getAllClicks());


	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now + chrono::minutes(3)));
	EXPECT_EQ(3, s.getAllClicks());

	EXPECT_CALL(timeMock, now())
		.WillOnce(Return(now + chrono::minutes(LAST_MINUTES)));
	EXPECT_EQ(1, s.getAllClicks());
}

} // namespace

typedef StorageBase<Time> Storage;

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
