//
// Created by zhou.yang on 2018/11/1.
//

#pragma once

#include "exception.h"

namespace platon{
    /**
     * @brief Platon assertion.
     *
     */
    #define PlatonAssert(A, ...) ::platon::assertAux(A, #A, __LINE__, __FILE__, __func__, ##__VA_ARGS__)
    /**
     * @brief Assertion A equals B.
     *
     */
    #define PlatonAssertEQ(A, B, ...) PlatonAssert(((A)==(B)),##__VA_ARGS__)
    /**
     * @brief Assertion A not equal to B.
     */
    #define PlatonAssertNE(A, B, ...) PlatonAssert(((A)!=(B)), ##__VA_ARGS__)

    /**
     * @brief Platon assertion implementation, assertion failure output failure location and error message.
     * @param cond      Assertion condition.
     * @param conndStr  Assertion prompt.
     * @param line      The line number of the code that failed the assertion.
     * @param file      Code file with assertion failure.
     * @param func      Function with assertion failure.
     * @param args      Argument list.
     */
    template<typename... Args>
    inline void assertAux(bool cond, const char *condStr, unsigned line, const char *file, const char *func, Args&&... args) {
        if (!cond) {
           platonThrow("Assertion failed:", condStr, "func:", func, "line:", line, "file:", file, args...);
        }
    }
}

