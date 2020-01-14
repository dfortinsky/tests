#include <string>
#include <map>
#include <vector>

#include <chrono>
#include <mutex>

#include <gtest/gtest.h>

using namespace std;
using namespace ::testing;

#define LAST_MINUTES 10

class Storage {
	struct Data {
		unsigned int uid;
		string info;

		Data(const unsigned int& uid,
			const string& data)
			// const chrono::time_point<std::chrono::system_clock>& timeStamp)
				: uid(uid), info(data) {
		}
	};

	// What's more important user id or timestamp?
	// Should I check for non-existing user?
	// Should I erase records after some time?
	// TimeStamp is always bigger then previos one? Or I should sort it for binary search in future?

	// I assume
	// 1. I don't check fraud uid
	// 2. I never erase records
	// 3. Timestamp can be random
	// 4. There is a multithreading system
	// 5. I need to return only info
	map<chrono::time_point<std::chrono::system_clock>, Data> records;
	mutex mute;
public:
	// Store some data by user id and data
	void store(const unsigned int& uid,
				const string& data,
				const chrono::time_point<std::chrono::system_clock>& timeStamp) {
		if (uid == 0) {
			return;
		}

		lock_guard<mutex> lock(mute);
		records.emplace(timeStamp, move(Data(uid, data)));
	}

	// Get all or specified user records during 10 min from function call moment
	vector<string> getRecords(const unsigned int& uid = 0) {
		const auto& now = chrono::system_clock::now() - chrono::minutes(LAST_MINUTES);

		vector<string> r;
		for (auto i = records.lower_bound(now); i != records.end(); ++i) {
			if (uid == 0 || i->second.uid == uid) {
				r.emplace_back(i->second.info);
			}
		}

		return r;
	}
};

TEST(Storage, EmptyStorage) {
	Storage s;

	EXPECT_EQ(vector<string>(), s.getRecords());
}

TEST(Storage, Positive_OneRecord) {
	Storage s;
	string data("hello");

	s.store(23, data, chrono::system_clock::now());

	EXPECT_EQ(vector<string>({data}), s.getRecords(23));
	EXPECT_EQ(vector<string>(), s.getRecords(64));
	EXPECT_EQ(vector<string>({data}), s.getRecords());
}

TEST(Storage, Positive_SeveralRecords) {
	Storage s;
	string data1("hi"), data2("chao"), data3("nihao"), data4("poka"), data5("konichuah"), data6("merci");

	s.store(45, data1, chrono::system_clock::now() - chrono::minutes(5));
	s.store(67, data2, chrono::system_clock::now() - chrono::minutes(20));
	s.store(45, data3, chrono::system_clock::now());
	s.store(78, data4, chrono::system_clock::now() - chrono::hours(2));
	s.store(78, data5, chrono::system_clock::now() - chrono::hours(63));
	s.store(78, data6, chrono::system_clock::now() - chrono::minutes(9) + chrono::seconds(57));

	EXPECT_EQ(vector<string>({data1, data3}), s.getRecords(45));
	EXPECT_EQ(vector<string>(), s.getRecords(67));
	EXPECT_EQ(vector<string>({data6}), s.getRecords(78));

	EXPECT_EQ(vector<string>(), s.getRecords(1));

	EXPECT_EQ(vector<string>({data6, data1, data3}), s.getRecords());
}

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
