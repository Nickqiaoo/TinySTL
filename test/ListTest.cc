#include <gtest/gtest.h>
#include "../List.h"

using namespace TinySTL;

TEST(ListTest,testPush_back){
    list<int> v;
    v.push_back(1);
    
    EXPECT_EQ(1,v.front());
}

