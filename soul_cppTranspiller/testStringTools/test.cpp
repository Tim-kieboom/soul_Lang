#include "pch.h"

#include <iostream>

#include "../soul_cppTranspiller/stringTools.h"
#include "../soul_cppTranspiller/string_contain.cpp"
#include "../soul_cppTranspiller/string_copy.cpp"
#include "../soul_cppTranspiller/string_find.cpp"
#include "../soul_cppTranspiller/string_hash.cpp"
#include "../soul_cppTranspiller/string_remove.cpp"
#include "../soul_cppTranspiller/string_replace.cpp"
#include "../soul_cppTranspiller/string_split.cpp"

using namespace std;

TEST(removeSpan, removeNormalSpan)
{
	bool success = true;

	string str1 = "hello world";
	EXPECT_TRUE(string_removeSpan(str1, 0, 2));
	cout << "0..2: " << str1 << endl;
	EXPECT_TRUE(str1._Equal("llo world"));

	string str2 = "hello world";
	EXPECT_TRUE(string_removeSpan(str2, 2, 5));
	cout << "2..5: " << str2 << endl;
	EXPECT_TRUE(str2._Equal("he world"));

	string str3 = "hello world";
	EXPECT_TRUE(string_removeSpan(str3, 5, 11));
	cout << "5..10: " << str3 << endl;
	EXPECT_TRUE(str3._Equal("hello"));

	string str4 = "check out of bound";
	EXPECT_FALSE(string_removeSpan(str4, 5, 19));
	EXPECT_FALSE(string_removeSpan(str4, 19, 5));
	EXPECT_FALSE(string_removeSpan(str4, 19, 19));
}

TEST(removeSpan, removeNegativeSpan)
{
	bool success = true;
	string str1 = "hello world";
	EXPECT_TRUE(string_removeSpan(str1, 1, -1));
	cout << "5..-1: " << str1 << endl;
	EXPECT_TRUE(str1._Equal("h"));

	string str2 = "hello world";
	EXPECT_TRUE(string_removeSpan(str2, 0, -1));
	cout << "0..-1: " << str2 << endl;
	EXPECT_TRUE(str2._Equal(""));

	string str3 = "hello world";
	EXPECT_TRUE(string_removeSpan(str3, -4, -1));
	cout << "-4..-1: " << str3 << endl;
	EXPECT_TRUE(str3._Equal("hello w"));

	string str4 = "out of bounds";
	EXPECT_FALSE(string_removeSpan(str4, 0, -12));
	EXPECT_FALSE(string_removeSpan(str4, -12, 0));
	EXPECT_FALSE(string_removeSpan(str4, -12, -12));
}

TEST(stringReplace, does_string_replace_work)
{
	string str = "hello world word otherWord";

	uint32_t replacedCount = string_replace(str, ' ', '#');
	EXPECT_EQ(replacedCount, 3);
	EXPECT_TRUE(str._Equal("hello#world#word#otherWord"));
}

TEST(stringReplace_str, does_string_replace_work)
{
	string str = "hello world word otherWord";

	uint32_t replacedCount = string_replace(str, " ", "#");
	cout << str;
	EXPECT_EQ(replacedCount, 3);
	EXPECT_TRUE(str._Equal("hello#world#word#otherWord"));
}
TEST(stringCount, does_string_count_work)
{
	string str = "hello world word otherWord";

	uint32_t amounOfSpaces = string_count(str, ' ');
	EXPECT_EQ(amounOfSpaces, 3);
}

TEST(stringSplit_char, does_string_split_work_with_char)
{
	string str = "hello world word otherWord";

	vector<string> splits = string_split(str, ' ');

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;
	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello"));
	EXPECT_TRUE(splits.at(1)._Equal("world"));
	EXPECT_TRUE(splits.at(2)._Equal("word"));
	EXPECT_TRUE(splits.at(3)._Equal("otherWord"));
		);
}

TEST(stringSplit_string, does_string_split_work_with_string)
{
	string str = "hello world word otherWord";

	vector<string> splits = string_split(str, "world");

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;

	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello "));
	EXPECT_TRUE(splits.at(1)._Equal(" word otherWord"));
		);
}

TEST(stringSplit_charList, does_string_split_work_with_char)
{
	string str = "hello world foo#bar word otherWord";

	vector<string> splits = string_split(str, { ' ', '#' });

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;
	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello"));
	EXPECT_TRUE(splits.at(1)._Equal("world"));
	EXPECT_TRUE(splits.at(2)._Equal("foo"));
	EXPECT_TRUE(splits.at(3)._Equal("bar"));
	EXPECT_TRUE(splits.at(4)._Equal("word"));
	EXPECT_TRUE(splits.at(5)._Equal("otherWord"));
		);
}

TEST(stringSplit_stringList, does_string_split_work_with_string)
{
	string str = "hello world word foo otherWord";

	vector<string> splits = string_split(str, { "world", "foo" });

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;

	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello "));
	EXPECT_TRUE(splits.at(1)._Equal(" word "));
	EXPECT_TRUE(splits.at(2)._Equal(" otherWord"));
		);
}

TEST(stringSplitOn_char, does_string_split_work_with_char)
{
	string str = "hello world word otherWord";

	vector<string> splits = string_splitOn(str, ' ');

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;
	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello"));
	EXPECT_TRUE(splits.at(1)._Equal(" "));
	EXPECT_TRUE(splits.at(2)._Equal("world"));
	EXPECT_TRUE(splits.at(3)._Equal(" "));
	EXPECT_TRUE(splits.at(4)._Equal("word"));
	EXPECT_TRUE(splits.at(5)._Equal(" "));
	EXPECT_TRUE(splits.at(6)._Equal("otherWord"));
		);
}

TEST(stringSplitOn_string, does_string_split_work_with_string)
{
	string str = "hello world word otherWord";

	vector<string> splits = string_splitOn(str, "world");
	EXPECT_EQ(splits.size(), 3);

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;

	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello "));
	EXPECT_TRUE(splits.at(1)._Equal("world"));
	EXPECT_TRUE(splits.at(2)._Equal(" word otherWord"));
		);
}

TEST(stringSplitOn_charList, does_string_split_work_with_char)
{
	string str = "hello world foo#bar word otherWord";

	vector<string> splits = string_splitOn(str, { ' ', '#' });

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;
	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello"));
	EXPECT_TRUE(splits.at(1)._Equal(" "));
	EXPECT_TRUE(splits.at(2)._Equal("world"));
	EXPECT_TRUE(splits.at(3)._Equal(" "));
	EXPECT_TRUE(splits.at(4)._Equal("foo"));
	EXPECT_TRUE(splits.at(5)._Equal("#"));
	EXPECT_TRUE(splits.at(6)._Equal("bar"));
	EXPECT_TRUE(splits.at(7)._Equal(" "));
	EXPECT_TRUE(splits.at(8)._Equal("word"));
	EXPECT_TRUE(splits.at(9)._Equal(" "));
	EXPECT_TRUE(splits.at(10)._Equal("otherWord"));
		);
}

TEST(stringSplitOn_stringList, does_string_split_work_with_string)
{
	string str = "hello world word foo otherWord";

	vector<string> splits = string_splitOn(str, { "world", "foo" });
	EXPECT_EQ(splits.size(), 5);

	for (auto split : splits)
		cout << split << "\n";
	cout << endl;

	EXPECT_NO_THROW
	(
		EXPECT_TRUE(splits.at(0)._Equal("hello "));
	EXPECT_TRUE(splits.at(1)._Equal("world"));
	EXPECT_TRUE(splits.at(2)._Equal(" word "));
	EXPECT_TRUE(splits.at(3)._Equal("foo"));
	EXPECT_TRUE(splits.at(4)._Equal(" otherWord"));
		);
}

TEST(stringReplaceAt, does_string_replace_work)
{
	string str = "hello world word otherWord";
	string replace = "foo";

	EXPECT_TRUE(string_replaceAt(str, 6, 11, replace));
	cout << str;
	EXPECT_TRUE(str._Equal("hello foo word otherWord"));
}

TEST(stringFind_str, does_string_find_work)
{
	string str = "foo foo bar bar foo bar oq;priehfgwoe gifwdeuqy";
	vector<uint32_t> finds = string_find(str, "foo");

	EXPECT_EQ(finds.size(), 3);

	EXPECT_NO_THROW
	(
		EXPECT_EQ(finds.at(0), 0);
		EXPECT_EQ(finds.at(1), 4);
		EXPECT_EQ(finds.at(2), 16);
	);
}

TEST(stringReplaceInSpan, does_string_replaceInSpan_work)
{
	string str = "foo foo bar bar foo bar oq;priehfgwoe gifwdeuqy";

	bool success = string_replaceInSpan(str, "foo", "###__###_###", 13, 30);

	cout << str;
	EXPECT_TRUE(str._Equal("foo foo bar bar ###__###_### bar oq;priehfgwoe gifwdeuqy"));
	EXPECT_TRUE(success);

}