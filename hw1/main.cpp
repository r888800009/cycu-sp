// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include <fstream>
#include <iostream>
#include "lexicaler.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 2) {
    Lexicaler lexicaler;

    try {
      string filename = argv[1];
      lexicaler.loadFile(filename);
      lexicaler.lexing();
    } catch (Lexicaler::Error e) {
      if (e == Lexicaler::loading_failure)
        ;
      else
        cout << "Unknown Error!" << endl;
      return 1;

    } catch (LoadingTable::Error e) {
      if (e == LoadingTable::loading_failure)
        ;
      else
        cout << "Unknown Error!" << endl;
      return 1;
    }

  } else
    cout << argv[0] << " input.txt" << endl;

  return 0;
}
