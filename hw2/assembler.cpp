// 資工三甲 林詠翔
// must to use -std=c++11 or higher version
#include "assembler.h"

#include <cassert>
#include <iomanip>
#include <sstream>

#include "error.h"
#include "parser.h"

using namespace std;

bool inRange(int i) {
  return (0 <= i && i <= 0x7ff) || (i < 0 && (i & ~0xfff) == 0xfffff000);
}

bool testInRange() {
  assert(inRange(-0xfff - 2) == false);
  assert(inRange(0xfff) == false);
  assert(inRange(-0xfff - 1) == true);

  assert(inRange(0x0) == true);
  assert(inRange(0x7ff) == true);
}

Assembler::Assembler() : parser(&lexer, &optab) {
  // lexer = Lexicaler(); Maybe not necessary?
  optab.loadOPTab("OPtable.tsv");  // OPTAB

  // SYMTAB
  // LOCCTR
  locationCounter = 0;
  enableBase = false;
  baseCounter = 0;
}

string Assembler::genFormat1(int opcode) {
  stringstream stream;

  stream << std::noshowbase << setfill('0') << setw(2) << uppercase << std::hex
         << opcode;

  if (stream.str().length() != 2) throw Error::syntax_error;
  return stream.str();
}

string Assembler::genFormat2(int opcode, int r1, int r2) {
  stringstream stream;

  stream << std::noshowbase << setfill('0') << setw(2) << uppercase << std::hex
         << opcode << r1 << r2;

  if (stream.str().length() != 4) throw Error::syntax_error;
  return stream.str();
}

int Assembler::flagToBinary(Flag flag) {
  int result = 0;

  result |= (flag.n) << 5;
  result |= (flag.i) << 4;
  result |= (flag.x) << 3;
  result |= (flag.b) << 2;
  result |= (flag.p) << 1;
  result |= (flag.e) << 0;

  return result;
}

string Assembler::genFormat3(int opcode, Flag flag, int disp) {
  stringstream stream;
  int result = 0;

  result |= (0b11111100 & opcode) << (12 - 2) + 6;
  result |= flagToBinary(flag) << 12;
  result |= (disp & 0b111111111111);

  stream << std::noshowbase << setfill('0') << setw(6) << uppercase << std::hex
         << result;

  if (stream.str().length() != 6) throw Error::syntax_error;
  return stream.str();
}

string Assembler::genFormat4(int opcode, Flag flag, int address) {
  stringstream stream;
  int result = 0;

  result |= (0b11111100 & opcode) << (20 - 2) + 6;
  result |= flagToBinary(flag) << 20;
  result |= (address & 0b11111111111111111111);

  stream << std::noshowbase << setfill('0') << setw(8) << uppercase << std::hex
         << result;

  if (stream.str().length() != 8) throw Error::syntax_error;
  return stream.str();
}

void Assembler::test() {
  string code;
  // format1
  code = genFormat1(0xab);
  assert(code == "AB" && code.length() == 2);  // 2 nibble

  code = genFormat1(0x00);
  assert(code == "00" && code.length() == 2);  // 2 nibble

  // format2
  code = genFormat2(0xAB, 0xC, 0xD);
  assert(code == "ABCD" && code.length() == 4);  // 4 nibble

  code = genFormat2(0x00, 0x4, 0x9);
  assert(code == "0049" && code.length() == 4);  // 4 nibble

  code = genFormat2(0x00, 0x0, 0x0);
  assert(code == "0000" && code.length() == 4);  // 4 nibble

  // format3
  code = genFormat3(0b11111100, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF0000" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b11110000, {0, 0, 1, 1, 1, 1}, 0b111111111111);
  assert(code == "F0FFFF" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b00000000, {0, 0, 1, 1, 1, 1}, 0b111111111111);
  assert(code == "00FFFF" && code.length() == 6);  // 6 nibble

  code = genFormat3(0b00000000, {0, 0, 1, 1, 1, 1}, 0b111100001111);
  assert(code == "00FF0F" && code.length() == 6);  // 6 nibble

  // add sic
  code = genFormat3(0x18, {0, 0, 1, 1, 1, 1}, 0b111100001111);
  assert(code == "18FF0F" && code.length() == 6);  // 6 nibble

  // format4
  code = genFormat4(0b11111100, {1, 1, 0, 0, 0, 0}, 0x0);
  assert(code == "FF000000" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b11111100, {1, 1, 0, 0, 0, 0}, 0b11111111111111111111);
  assert(code == "FF0FFFFF" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b00000000, {0, 0, 1, 1, 1, 1}, 0b11111111111111111111);
  assert(code == "00FFFFFF" && code.length() == 8);  // 8 nibble

  code = genFormat4(0b00000000, {0, 0, 1, 1, 1, 1}, 0b11111111111100001111);
  assert(code == "00FFFF0F" && code.length() == 8);  // 8 nibble

  // hello world

  printHeader();
  printLine(0, 0, "COPY  START 100", "");
  printLine(5, 1000, "COPY  START 100", "");
  printLine(10, 1000, "FIRST STL RETADR", "141033");
  cout << printStream.str();

  testInRange();
}

void Assembler::setXE(bool sicxe) {
  this->sicxe = sicxe;
  parser.setXE(sicxe);
}

void Assembler::printHeader() {
  printStream << setw(OFFSET_LINE) << left << "Line" << setw(OFFSET_LOCATION)
              << left << "  Loc" << setw(OFFSET_STATEMENT) << left
              << "Source Statement" << setw(OFFSET_CODE) << left
              << "Object code" << endl;
  printStream << endl;
}

void Assembler::printLine(int line, int loc, const string &statement,
                          const string &objcode) {
  stringstream ss;
  ss << hex << uppercase << setw(4) << setfill('0') << loc;
  string output = " " + statement;

  // trim
  // regex_replace(output, regex("\t"), "        ");
  // output.erase(output.find_last_not_of(" ") + 1);

  printStream << setw(OFFSET_LINE) << right << line << setw(OFFSET_LOCATION)
              << right << ss.str() << setw(OFFSET_STATEMENT) << left << output
              << "|" << setw(OFFSET_CODE) << left << objcode << endl;
}

void Assembler::loadFile(const string &filename) {
  this->filename = filename;
  cout << "Loading file: \"" << filename << '"' << endl;
  fin.open(this->filename, ios::in);
  if (!fin) {
    cout << "Can't load file " << this->filename << endl;
    throw Error::loading_failure;
  }

  regex postFix("_input.txt$");
  string newName = filename;
  newName = regex_replace(newName, postFix, "");
  this->saveName = newName + "_output.txt";
  cout << "SaveName: \"" << saveName << '"' << endl;
}

void Assembler::genLiteral(int pass) {
  assert(pass == 1 || pass == 2);
  for (int i = 0; i < littab.size(); i++) {
    if (littab[i].address == -1) littab[i].address = curLocationCounter;

    printStream << hex << littab[i].address << " * ";
    int type = littab[i].literal.type;
    string litStr = lexer.getData(littab[i].literal);
    if (type == STRING_TABLE) {
      // n bytes
      locationCounter += litStr.size();  //+ 1;  // string with null char
    } else if (type == INTEGER_REAL_TABLE) {
      int value = stoi(lexer.getData(littab[i].literal), nullptr, 16);
      // 1 bytes
      if (value <= 0xff)
        locationCounter += 1;
      else
        locationCounter += 3;
      printStream << hex << value;
    }

    printStream << endl;
    curLocationCounter = locationCounter;
    //  printStream << hex << locationCounter << " *" << endl;
  }
  cout << "literal not working" << endl;
}

void Assembler::doPseudo(int pass) {
  assert(pass == 1 || pass == 2);
  Parser::Pseudo pseudo = parser.match.pseudo;

  switch (pseudo) {
    case Parser::Pseudo::START:
      curLocationCounter = locationCounter = parser.match.startMatch;
      break;
    case Parser::Pseudo::BYTE:
      break;
    case Parser::Pseudo::WORD:
      break;
    case Parser::Pseudo::RESB:
      locationCounter += parser.match.stringData.integer;
      break;
    case Parser::Pseudo::RESW:
      locationCounter += parser.match.stringData.integer * 3;
      break;
    case Parser::Pseudo::EQU:
      break;
    case Parser::Pseudo::BASE:
      break;
    case Parser::Pseudo::END:
      genLiteral(pass);
      break;
    case Parser::Pseudo::LTORG:
      genLiteral(pass);
      break;
  }
}

Flag Assembler::genAddressingTypeFlag() {
  Flag flag = {0};
  Parser::AddressingType addrType = parser.match.addrType;
  if (addrType == Parser::simple_addressing) {
    if (sicxe) {
      // sicxe
      flag.n = 1;
      flag.i = 1;
    } else {
      // sic
      flag.n = 0;
      flag.i = 0;
    }

    // x
    if (parser.match.x)
      flag.x = 1;
    else
      flag.x = 0;
  } else if (addrType == Parser::indirect_addressing) {
    flag.n = 1;
    flag.i = 0;
  } else if (addrType == Parser::immediate_addressing) {
    flag.n = 0;
    flag.i = 1;
  }

  return flag;
}

string Assembler::genInstruction(int pass) {
  assert(pass == 1 || pass == 2);
  string objcode = "======";

  int format = parser.match.format;
  Parser::Pseudo pseudo = parser.match.pseudo;

  // if (parser.match.memory)

  locationCounter += format;
  if (!(4 >= format && format >= 1)) cout << "bug?" << format << endl;

  if (pass == 1) {
    // match literal
    TokenData literal = parser.match.literal;
    if (literal.type == STRING_TABLE || literal.type == INTEGER_REAL_TABLE) {
      auto it =
          find_if(littab.begin(), littab.end(), [literal](const Literal &l) {
            return isTokenEqual(l.literal, literal);
          });
      if (it == littab.end()) littab.push_back({literal, -1});
    }
  } else if (pass == 2) {
    // set flag
    Flag flag = genAddressingTypeFlag();

    // gen opcode
    int opcode = parser.match.opcode;
    if (format == 1)
      objcode = genFormat1(opcode);
    else if (format == 2)
      objcode = genFormat2(opcode, parser.match.op1, parser.match.op2);
    else {
      // printStream << format << endl;
      // printStream << sicxe << endl;
      // match memory
      int disp = 0;
      TokenData literalToken = parser.match.literal;
      if (parser.match.memory.type == SYMBOL_TABLE) {
        disp = symtab.getSymbolAddress(parser.match.memory.value);
      } else if (parser.match.memory.type == INTEGER_REAL_TABLE) {
        disp = parser.match.stringData.integer;
      } else if (literalToken.type == STRING_TABLE ||
                 literalToken.type == INTEGER_REAL_TABLE) {
        auto it = find_if(littab.begin(), littab.end(),
                          [literalToken](const Literal &l) {
                            return isTokenEqual(l.literal, literalToken);
                          });

        // check
        if (it == littab.end() || it->address == -1) {
          cout << "Not defined LTORG or END?" << endl;
          throw Error::syntax_error;
        }

        disp = it->address;
      }

      if (optab.getOPCode("RSUB") == opcode) {
        disp = 0;
        flag.b = 0;
        flag.p = 0;
      } else if (format == 3) {
        if (sicxe) {
          // check range
          int pcRelocation = disp - locationCounter;
          int baseRelocation = disp - baseCounter;
          if (parser.match.memory.type == INTEGER_REAL_TABLE && 0 <= disp &&
              disp <= 4095) {
            // disp 0 ~ 4095
            // do nothing
            flag.b = 0;
            flag.p = 0;
          } else if (inRange(pcRelocation)) {
            // pc + disp
            disp = pcRelocation;

            flag.b = 0;
            flag.p = 1;
          } else if (inRange(baseRelocation) && enableBase) {
            // b + disp
            disp = baseRelocation;

            flag.b = 1;
            flag.p = 0;
          } else
            throw Error::syntax_error;
        } else {
          // sic
          // check range?
          // set b p e
          flag.b = 0b100000000000000 & disp;
          flag.p = 0b010000000000000 & disp;
          flag.e = 0b001000000000000 & disp;

          disp &= 0xfff;
        }
      } else if (format == 4 && sicxe) {
        // check range

      } else
        cout << "something worng? fmt34" << endl;

      // gen code
      if (format == 3)
        objcode = genFormat3(opcode, flag, disp);
      else if (format == 4 && sicxe) {
        flag.b = 0;
        flag.p = 0;
        flag.e = 1;

        objcode = genFormat4(opcode, flag, disp);
      }
    }
  }
  return objcode;
}

void Assembler::matchPreSymbol(const string &line) {
  try {
    if (parser.match.preSymbol.value != -1) {
      symtab.define(parser.match.preSymbol.value, locationCounter,
                    SymbolTable::absolute_address);
      // printStream << "define!" << endl;
    }

  } catch (Error::ASMError e) {
    if (e == Error::duplicate_define) {
      printStream << "duplicate_define: " << line << endl;
    }
  }
}

void Assembler::pass(int pass) {
  assert(pass == 1 || pass == 2);
  locationCounter = 0;

  if (pass == 1)
    cout << "pass1" << endl;
  else {
    cout << "pass2" << endl;
    // reset to begin
    lexer.reset();
    fin.clear();
    fin.seekg(0, ios::beg);
  }

  // loading file
  string line, token;
  fstream fout;

  fout.open(saveName, ios::out);
  if (!fout) {
    cout << "Can't open file " << saveName << endl;
    throw Error::saving_failure;
  }

  cout << "start write: \"" << saveName << '"' << endl;

  printHeader();
  while (getline(fin, line)) {
    curLocationCounter = locationCounter;
    try {
      // drop writespace
      line.erase(line.find_last_not_of(" \t\n\r") + 1);

      if (parser.matchSyntax(lexer.lexingLine(line))) {
        if (parser.isEmpty()) {
          printStream << setw(OFFSET_LINE + OFFSET_LOCATION) << " " << line
                      << endl;
          continue;
        }

        // pass1 handler first symbol and dupicate
        if (pass == 1) matchPreSymbol(line);

        if (parser.match.pseudo == Parser::Pseudo::NOT_PSEUDO) {
          string objcode = genInstruction(pass);
          printLine(0, curLocationCounter, line, objcode);
        } else {
          printLine(0, curLocationCounter, line, "");
          doPseudo(2);
        }
      } else
        throw Error::syntax_error;

    } catch (Error::ASMError e) {
      if (e == Error::undefine_symbol && pass == 2) {
        // undefine only work at pass2
        printStream << "ERROR: undefine_error" << endl;
        printStream << line << endl;
      } else if (e == Error::syntax_error)
        printStream << "ERROR: syntax_error: " << line << endl;
    }
  }

  cout << printStream.str();
  fout << printStream.str();
  fout.close();
}

void Assembler::assembling() {
  pass(1);
  pass(2);
  fin.close();
}

