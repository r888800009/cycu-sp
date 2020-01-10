extern "C" int add_integer_c(const char *str);

#include <iostream>
#include <string>

#include "compiler.h"
using namespace std;

int add_integer_c(const char *str) {
  // cout << str << endl;
  add_integer(string(str));
}
