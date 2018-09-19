//
// Created by USER on 17/09/2018.
//


#include "hash/extendible_hash.h"
#include <iostream>
#include "gtest/gtest.h"

int main(){
    cmudb::ExtendibleHash<int, string> test(2);
    test.Insert(1, "a");
    test.Insert(2, "b");
    test.Insert(3, "c");
    test.Insert(4, "d");
    test.Insert(5, "e");
    test.Insert(6, "f");
    test.Insert(7, "g");
    test.Insert(8, "h");
    test.Insert(9, "i");
    EXPECT_EQ(2, test.GetLocalDepth(0));
    EXPECT_EQ(3, test.GetLocalDepth(1));
    EXPECT_EQ(2, test.GetLocalDepth(2));
    EXPECT_EQ(2, test.GetLocalDepth(3));
    return 0;
}