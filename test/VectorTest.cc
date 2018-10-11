#include <gtest/gtest.h>
#include "../Vector.h"

using namespace TinySTL;

TEST(VecotrTest,testPush_back){
    vector<int> v;
    v.push_back(1);

    EXPECT_EQ(1,v[0]);
}
