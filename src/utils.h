//
// Created by Luca Gobbato on 04/10/16.
//

#include <string>
#include <vector>
#include <set>

using namespace std;

#ifndef COIOTE_HEURISTIC_UTILS_H
#define COIOTE_HEURISTIC_UTILS_H

string splitpath(const std::string& str) {
    vector<string> result;
    set<char> delimiters{'\\', '/'};

    char const* pch = str.c_str();
    char const* start = pch;

    for(; *pch; ++pch) {
        if (delimiters.find(*pch) != delimiters.end()) {
            if (start != pch) {
                string str(start, pch);
                result.push_back(str);
            }
            else
                result.push_back("");
            start = pch + 1;
        }
    }
    result.push_back(start);
    return result.back();
}

#endif //COIOTE_HEURISTIC_UTILS_H
