#include <string>
#include <map>
#include <vector>

#include <chrono>
#include <mutex>

using namespace std;

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

int main() {
	return 0;
}
