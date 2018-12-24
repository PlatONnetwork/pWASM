//
// Created by zhou.yang on 2018/11/26.
//

#pragma once

#include "platon/assert.h"

extern "C" {
    size_t getTestLog(char *log, size_t size);
    size_t getTestLogSize();
    void clearLog();
}
namespace platon {
    namespace test {
        std::string getLog() {
            std::vector<char> log;
            size_t size = ::getTestLogSize();
            log.resize(size);
            PlatonAssert(::getTestLog((char*)log.data(), size) == size);
            return std::string(log.data(), size);
        }
    }
}


