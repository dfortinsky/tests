#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace ::testing;


#ifndef INTUITIVE

pair<int, int> findSubArray(const vector<int>& nums, const int& sum = 0) {
	unordered_map<int, int> memo;
	memo.reserve(nums.size());
	auto it = memo.begin();

	int subsum = 0,
		required = 0;
	for (int i = 0; i < nums.size(); ++i) {
		subsum += nums[i];
		if (subsum == sum) {
			return {0, i};
		}

		required = subsum - sum;
		if ((it = memo.find(required)) != memo.end()) {
			return {it->second + 1, i};
		}

		memo.emplace(subsum, i);
	}

	return {-1, -1};
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
	vector<int> nums (1000000, 1);
	const auto& p = findSubArray(nums, -5);

	EXPECT_THAT(p, Eq(pair<int,int>(-1, -1)));
}

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}