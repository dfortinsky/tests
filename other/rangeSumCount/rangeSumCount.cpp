// Given an integer array nums, return the number of range sums that lie in [lower, upper] inclusive.
// Range sum S(i, j) is defined as the sum of the elements in nums between indices i and j (i ≤ j), inclusive.

// Note:
// A naive algorithm of O(n2) is trivial. You MUST do better than that.

// Example:

// Input: nums = [-2,5,-1], lower = -2, upper = 2,
// Output: 3 
// Explanation: The three ranges are : [0,0], [2,2], [0,2] and their respective sums are: -2, -1, 2.

#include <vector>
#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace ::testing;

namespace {

class OrderStatisticsSet {
	struct Node {
		long long val;
		Node* left;
		Node* right;
		size_t amount;  // a number of copies
		size_t size;  // subtree size

		Node(long long value)
			: val(value)
			, left(nullptr)
			, right(nullptr)
			, amount(1)
			, size(1) {}
	};

public:
	OrderStatisticsSet()
		: mRoot(nullptr) {
	}

	~OrderStatisticsSet() {
		vector<Node*> v;

		if (mRoot) v.push_back(mRoot);

		while (v.size()) {
			Node* node = v.back();
			v.pop_back();

			if (node->left) v.push_back(node->left);
			if (node->right) v.push_back(node->right);

			delete node;
		}
	}

	void insert(long long key) {
		++mSize;

		if (mRoot == nullptr) {
			mRoot = new Node(key);
			return;
		}

		Node* temp = mRoot;

		while (true) {
			++temp->size;

			if (temp->val == key) {
				++temp->amount;
				return;
			} else if (temp->val < key) {
				if (temp->right) {
					temp = temp->right;
					continue;
				} else {
					temp->right = new Node(key);
					return;
				}
			} else {
				if (temp->left) {
					temp = temp->left;
					continue;
				} else {
					temp->left = new Node(key);
					return;
				}
			}
		}

		// TODO: balance!
	}

	// Find rank of key in set
	// - key may not be contained
	// - equal = collect equal values in rank (e.g. {4, 5}, rank(4) => 2)
	int rank(const long long& key, bool equal) {
		size_t count = 0;
		Node* temp = mRoot;

		while (temp) {
			if (temp->val == key) {
				if (temp->left) count += temp->left->size;
				if (equal) count += temp->amount;

				return count;
			} else if (temp->val < key) {
				if (temp->left) count += temp->left->size;
				count += temp->amount;

				temp = temp->right;
			} else {
				temp = temp->left;
			}
		}

		return count;
	}

	size_t size() const {
		return mSize;
	}

private:
	Node* mRoot;
	size_t mSize;
};

TEST(OrderStatisticsSet, Positive_Simple) {
	OrderStatisticsSet set;
	set.insert(1);
	set.insert(5);
	set.insert(-7);
	set.insert(9);
	set.insert(2);

	EXPECT_EQ(3, set.rank(4, false));
	EXPECT_EQ(0, set.rank(-10, false));
	EXPECT_EQ(4, set.rank(9, false));

	EXPECT_EQ(4, set.rank(5, true));
}

TEST(OrderStatisticsSet, Positive_Duplicates) {
	OrderStatisticsSet set;
	set.insert(3);
	set.insert(3);
	set.insert(-1);
	set.insert(8);

	EXPECT_EQ(3, set.rank(4, false));
	EXPECT_EQ(1, set.rank(1, false));

	EXPECT_EQ(3, set.rank(3, true));
}

}

int countRangeSum(const vector<int>& nums, const int& lower, const int& upper) {
	if (nums.empty()) return 0;

	vector<long long> prefixSums(nums.size(), 0);
	prefixSums[0] = nums.front();

	for(int i = 1; i < nums.size(); ++i) {
		prefixSums[i] = prefixSums[i - 1] + nums[i];
	}

	int sum = 0;
	OrderStatisticsSet oSSet;
	for(const auto& p : prefixSums) {
		if (p >= lower && p <= upper) ++sum;

		// p - lb >= lower
		// p - ub <= upper
		long long lb = p - lower;
		long long ub = p - upper;
		
		int from = oSSet.rank(ub, false);
		int to = oSSet.rank(lb, true);
		sum += to - from;

		oSSet.insert(p);
	}

    return sum;
}

TEST(CountRangeSum, Positive_Simple) {
	vector<int> v = { -2 , 5, -1 };

	EXPECT_EQ(3, countRangeSum(v, -2, 2));
}

TEST(CountRangeSum, Positive_ArrayWithZero) {
	vector<int> nums { 12, 4, -6, -15, 2, 9, 0 };

	EXPECT_EQ(1, countRangeSum(nums, -1, 1));
	EXPECT_EQ(3, countRangeSum(nums, 3, 7));
	EXPECT_EQ(5, countRangeSum(nums, -3, 5));
}

TEST(CountRangeSum, Positive_LargeNumbers) {
	vector<int> nums { -2147483647, 0, -2147483647, 2147483647 };

	EXPECT_EQ(3, countRangeSum(nums, -564, 3864));
}

int main(int argc, char **argv) {
	InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}