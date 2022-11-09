#include <iostream>
#include <regex>
#include <string>
using namespace std;


int main() {
    if (regex_match("www.google.com/asd/", regex(R"(^(www\.)?[a-z0-9]+\.[a-z]+(/[a-zA-Z0-9]*)*$)"))) {
        cout << "Matches!" << endl;
    }
    return 0;
}