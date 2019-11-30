// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <cstring>
#include <fstream>
#include <iostream>

#include "assembler.h"
#include "error.h"
#include "lexicaler.h"

#ifdef DEBUGING
#include "test.h"
#endif

using namespace std;

void loadFile(const string &filename) {
  Assembler assembler;

  try {
    assembler.loadFile(filename);

    while (1) {
      char c;
      cout << "[1] SIC" << endl;
      cout << "[2] SICXE" << endl;
      cout << "> ";
      cin >> c;
      if (c == '1') {
        assembler.setXE(false);
        break;
      } else if (c == '2') {
        assembler.setXE(true);
        break;
      }
    }

    assembler.assembling();

  } catch (Error::IOError e) {
    if (e == Error::loading_failure)
      ;
    else if (e == Error::saving_failure)
      ;
    else
      throw "Unknown Error!";
  } catch (Error::IOError e) {
    if (e == Error::loading_failure)
      ;
    else
      throw "Unknown Error!";
  }
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
