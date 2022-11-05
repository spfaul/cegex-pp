#pragma once
#ifndef _CEGEX_PP_HPP_LIB_
#define _CEGEX_PP_HPP_LIB_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <deque>
#include <stack>
#include <map>

enum ExprType {
    FIXED, // 0
    STR_START, // 1
    STR_END, // 2
    WILDCARD, // 3
    WORD, // 4
    DIGIT, // 5
    WHITESPACE, // 6
    CAPTURE_OPEN, // 7
    CAPTURE_CLOSE, // 8
    CAPTURE, // 9
    REPEAT_NONE_OR_MORE, // 10
    REPEAT_ONCE_OR_MORE, // 11
    REPEAT_NONE_OR_ONCE, // 12
    CHAR_RANGE, // 13
    CHARSET // 14
};

struct Expr {
    ExprType type;
    std::string content = "";
    std::deque<Expr> children = {};
};

using re_t = std::deque<Expr>;

struct ReMatch {
    int start_idx;
    unsigned int size;
    std::vector<std::string> captures;
};

re_t compile_repattern(std::string s);

ReMatch match_repattern(re_t &pattern, std::string &text);

#endif