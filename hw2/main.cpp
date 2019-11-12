// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <cstring>
#include <fstream>
#include <iostream>

#include "assembler.h"
#include "lexicaler.h"

#ifdef DEBUGING
#include "test.h"
#endif

using namespace std;

void loadFile(const string &filename) {
  Assembler assembler;

  assembler.setFile(filename);
}

int main(int argc, char *argv[]) {
#ifdef DEBUGING
  test();
#endif

  if (argc == 2) {
#ifdef DEBUGING
    if (strcmp("--debuging", argv[1]) == 0) return 0;
#endif
    loadFile(argv[1]);
  } else {
    cout << "Interactive mode" << endl;
    cout << "or \"" << argv[0] << " input.txt\"" << endl;
    cout << "type \".\" quit" << endl;
    string filename;

    cout << "> ";
    while (cin >> filename && filename != ".") {
      loadFile(filename);
      cout << "> ";
    }
  }

  return 0;
}
