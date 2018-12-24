//
// Created by zhou.yang on 2018/11/27.
//

#include "../unittest.hpp"
#include "../log.h"
#include "platon/event.hpp"
using namespace platon;

PLATON_EVENT(hello, const char*, int32_t)

class LOG_TEST {
public:
    ~LOG_TEST(){
        ::clearLog();
    }
};


TEST_CASE(test, event){
    {
        LOG_TEST logTest;
        PLATON_EMIT_EVENT(hello, "hello", 123);
        ASSERT(test::getLog() == "hello c78568656c6c6f7b", test::getLog());
    }








}

UNITTEST_MAIN() {
    RUN_TEST(test, event)
};

