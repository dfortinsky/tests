#include <unordered_map>
#include <vector>

#include <boost/container_hash/hash.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace ::testing;


#ifndef INTUITIVE

pair<int, int> findSubArray(const vector<int>& nums, const int& sum = 0) {
	// Store subsum by indexes from and to:
	// memo[[from, to]] = subsum; 
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int> > > memo;
	memo.reserve(nums.size() * nums.size() / 2);
	auto it = memo.begin();

	int previous = 0;
	int subsum = 0; // store already calculated subarray here
	for (int i = 0; i < nums.size(); ++i) {
		// If current element is already equal sum (lucky) return it
		if (nums[i] == sum) {
			return {i, i};
		}

		// Check subarray [i - 1, i]
		// if ok - return,
		// else store for future
		previous = i - 1; // next to current element in order to merge it to array
		if (previous < 0) {
			continue;
		}

		if ((subsum = nums[previous] + nums[i]) == sum) {
			return {previous, i};
		}

		memo.emplace(pair<int,int>(previous, i), subsum);

		// Let's extend subarray to the left
		for(int j = previous - 1; j >= 0; --j) {
			it = memo.find(pair<int,int>(j, previous));
			if (it != memo.end() && (subsum = it->second + nums[i]) == sum) {
				return {j, i};
			}

			memo.emplace(pair<int, int>(j, i), subsum);
		}
	}

	return pair<int,int>(-1,-1);
}

#else

pair<int, int> findSubArray(const vector<int>& nums, const int& sum = 0) {
	for (int i = 0; i < nums.size(); ++i) {
		int subsum = 0;

		for (int j = i; j < nums.size(); ++j) {
			subsum += nums[j];
			if (subsum == sum) {
				return {i, j};
			}
		}
	}

	return {-1, -1};
}

#endif

TEST(FindSubArray, Positive_Zero) {
	vector<int> nums {1, -7, 2, 5};
	const auto& p = findSubArray(nums);

	EXPECT_THAT(p, Eq(pair<int,int>(1, 3)));
}

TEST(FindSubArray, Positive_Sum) {
	vector<int> nums {12, 4, -6, -15, 2, 9, 0};
	const auto& p = findSubArray(nums, 11);

	EXPECT_THAT(p, Eq(pair<int,int>(4, 5)));
}

TEST(FindSubArray, Positive_FullArray) {
	vector<int> nums {1, 1, 1, -3};
	const auto& p = findSubArray(nums);

	EXPECT_THAT(p, Eq(pair<int,int>(0, 3)));
}

TEST(FindSubArray, Positive_SingleElement) {
	vector<int> nums {15, 2, 1, -5};
	const auto& p = findSubArray(nums, 1);

	EXPECT_THAT(p, Eq(pair<int,int>(2, 2)));
}

TEST(FindSubArray, Negative_EmptyArray) {
	vector<int> nums;
	const auto& p = findSubArray(nums);

	EXPECT_THAT(p, Eq(pair<int,int>(-1, -1)));
}

TEST(FindSubArray, Negative_UnexistingSum) {
	vector<int> nums (10000, 1);
	const auto& p = findSubArray(nums, -5);

	EXPECT_THAT(p, Eq(pair<int,int>(-1, -1)));
}

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}