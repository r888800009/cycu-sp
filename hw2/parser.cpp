// 資工三甲 林詠翔
// must to use -std=c++11 or higher version

#include "parser.h"

#include <cassert>

#include "lexicaler.h"

Parser::Parser(Lexicaler *lexer, OPTab *optab) {
  this->lexer = lexer;
  this->optab = optab;
}

void Parser::setTokenString(vector<TokenData> *tokenString) {
  dataClear();
  this->tokenString = tokenString;
}

void Parser::debug() {
  printTokenValue(match.preSymbol);
  printTokenValue(tmpSymbol);
  cout << endl << "token string size:" << tokenString->size() << endl;
  cout << match.opcode << ", " << match.format << endl;

  cout << "op1" << match.op1 << endl;
  cout << "op2" << match.op1 << endl;

  cout.flush();
}

bool Parser::matchFormat1(const int r, int &l) {
  if (r >= tokenString->size()) return false;
  if (getOPData(1, l)) {
    l++;
    return true;
  }

  return false;
}

bool Parser::matchFormat2(const int r, int &l) {
  TokenData data;
  l = r;
  if (r >= tokenString->size()) return false;

  // get opcode
  if (!getOPData(2, l)) return false;
  if (r + 3 < tokenString->size()) {
    int r1 = matchRegister(l + 1), r2 = matchRegister(l + 3);
    int n1 = matchN(l + 3);

    if (matchOP("SHIFTL", l) || matchOP("SHIFTR", l)) {
      if (r1 != -1 && matchDelimiter(',', l + 2) && n1 != -1) {
        // SHIFTL {r1}, {n} or SHIFTR {r1}, {n}
        match.op1 = r1;
        match.op2 = n1 - 1;

        l += 4;
        return true;
      }
    } else if (matchOP(l)) {
      if (r1 != -1 && matchDelimiter(',', l + 2) && r2 != -1) {
        // other format2
        match.op1 = r1;
        match.op2 = r2;

        l += 4;
        return true;
      }
    }
  } else if (r + 1 < tokenString->size()) {
    // CLEAR {r1}, TIXR {r1}
    bool cond1 = (matchOP("CLEAR", l) || matchOP("TIXR", l)) &&
                 ((match.op1 = matchRegister(l + 1)) != -1);
    // SVC {n}
    bool cond2 = matchOP("SVC", l) && ((match.op1 = matchN(l + 1)) != -1);

    l += 2;
    match.op2 = 0;

    return cond1 || cond2;
  }

  return false;
}

void Parser::dataClear() {
  match.preSymbol = {-1, -1};
  match.opcode = -1;
  match.format = -1;
  match.op1 = -1;
  match.op2 = -1;
  match.x = false;

  match.stringData.value = {-1, -1};
  match.stringData.type = MatchData::StringData::null;
  match.memory = {-1, -1};
  match.literal = {-1, -1};
  match.addrType = AddressingType::null_addressing;
  match.pseudo = NOT_PSEUDO;
  match.startMatch = 0;
  match.baseMatch = {-1, -1};
  match.resMatch = {-1, -1};
  match.equMatch.clear();
}

bool Parser::matchPseudoToken(const string &pseudo, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i), lexer->pseudoExtraTable.get(pseudo));
}

bool Parser::matchConst(const int r, int &l, const string &pseudo,
                        const int max, Pseudo setPesudo) {
  if (!matchPseudoToken(pseudo, r)) return false;

  if (matchIntegerHex(l = r + 1, l) || matchString(l = r + 1, l))
    ;
  else if (matchIntegerDec(l = r + 1))
    l++;
  else
    return false;

  if (match.stringData.type != MatchData::StringData::string_data &&
      match.stringData.integer > max)
    return false;

  match.pseudo = setPesudo;

  return true;
}

bool Parser::matchPseudo(const int r, int &l) {
  l = r;
  if (l >= tokenString->size()) return false;

  if (matchSymbol(l) && matchPseudoToken("EQU", l + 1)) {
    // EQU

  } else {
    // symbol
    if (matchPrefixSymbol(l)) l++;
    int baseR = l;

    if (matchPseudoToken("START", baseR) && matchIntegerHex(baseR + 1)) {
      match.pseudo = START;
      match.startMatch = match.stringData.integer;
      l = baseR + 2;
      return true;

    } else if (matchPseudoToken("END", baseR)) {
      l++;
      match.pseudo = END;
      if (matchSymbol(baseR + 1)) {
        match.memory = tmpSymbol;
        l++;
      }

      return true;

    } else if (matchConst(baseR, l, "BYTE", 0xff, BYTE) ||
               matchConst(baseR, l, "WORD", 0xffffff, WORD)) {
      return true;

    } else if (matchPseudoToken("RESB", baseR) && matchIntegerDec(baseR + 1)) {
      match.pseudo = RESB;
      match.resMatch = match.stringData.value;
      l = baseR + 2;
      return true;

    } else if (matchPseudoToken("RESW", baseR) && matchIntegerDec(baseR + 1)) {
      match.pseudo = RESW;
      match.resMatch = match.stringData.value;
      l = baseR + 2;
      return true;

    } else if (matchPseudoToken("BASE", baseR) && matchMemory(baseR + 1)) {
      match.pseudo = BASE;
      match.baseMatch = match.memory;
      l = baseR + 2;
      return true;

    } else if (matchPseudoToken("LTORG", baseR)) {
      match.pseudo = LTORG;
      l = baseR + 1;
      return true;
    }
  }

  return false;
}

bool Parser::matchFormat3(const int r, int &l) {
  l = r;
  // match opcode is fmt3
  if (!getOPData(3, l)) return false;
  l++;

  if (matchOP("RSUB", r)) return true;

  if (matchMemory(l)) {
    // memory, simple addressing
    match.addrType = AddressingType::simple_addressing;
    l++;

    // if have x register, move index and set x true
    if (matchDelimiter(',', l) && matchRegister("X", l + 1)) {
      match.x = true;
      l += 2;
    } else
      match.x = false;
    return true;
  } else if (sicxe) {
    // make sure x is false
    match.x = false;
    if (matchDelimiter('@', l) && matchMemory(l + 1)) {
      // indirect
      match.addrType = AddressingType::indirect_addressing;
      l += 2;
      return true;
    } else if (matchDelimiter('#', l) && matchMemory(l + 1)) {
      // immediate
      match.addrType = AddressingType::immediate_addressing;
      l += 2;
      return true;
    } else if (matchDelimiter('=', l)) {
      int preL = l + 1;
      if (matchString(l = preL, l) || matchIntegerHex(l = preL, l)) {
        // literal
        match.literal = match.stringData.value;
        match.addrType = AddressingType::simple_addressing;
        // matchString or matchIntegerHex would modify l to size
        return true;
      }
    }
  }

  return false;
}

bool Parser::matchFormat4(const int r, int &l) {
  l = r;
  if (l >= tokenString->size()) return false;
  if (!matchDelimiter('+', l)) return false;
  l++;

  if (matchFormat3(l, l)) {
    match.format = 4;
    return true;
  }

  return false;
}

bool Parser::matchInstruction(const int r, int &l) {
  if (matchFormat1(r, l)) return true;
  if (matchFormat2(r, l)) return true;
  if (matchFormat3(r, l)) return true;
  if (matchFormat4(r, l)) return true;

  return false;
}

bool Parser::matchPrefixSymbol(int i) {
  if (matchSymbol(i)) {
    match.preSymbol = tmpSymbol;
    return true;
  }

  return false;
}

bool Parser::matchSymbol(int i) {
  if (i >= tokenString->size()) return false;
  TokenData data = tokenString->at(i);
  if (!lexer->symbolTable.exist(data)) return false;

  tmpSymbol = data;
  return true;
}

int Parser::matchSyntax(vector<TokenData> &tokenString) {
  dataClear();
  int base = 0;

  setTokenString(&tokenString);

  // has comment or no comment at lest

  // has symbol
  // if (matchSymbol);
  // no symbol

  // pseudo

  // BYTE X'F1'
  // BYTE C'EOF'
  // WORD X'FFFFFF'
}

bool Parser::matchIntegerDec(int r) {
  TokenData data;
  string num;
  try {
    if (r < tokenString->size() &&
        lexer->integerTable.exist(data = tokenString->at(r))) {
      // check format
      num = lexer->integerTable.get(data);
      if (!regex_match(num, regex("[0-9]+"))) return false;

      match.stringData.integer = stoi(lexer->integerTable.get(data));
      match.stringData.value = data;
      match.stringData.type = MatchData::StringData::integer_dec;
      return match.stringData.integer <= 0xffffff;
    }

    // check match and range
  } catch (invalid_argument) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  } catch (out_of_range) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  }

  return false;
}

bool Parser::matchIntegerHex(int i) {
  TokenData data;
  string num;
  try {
    if (i < tokenString->size()) {
      data = tokenString->at(i);
      // check format
      num = lexer->getData(data);
      if (!regex_match(num, regex("[0-9a-fA-F]+"))) return false;

      match.stringData.integer = stoi(num, nullptr, 16);
      match.stringData.value = data;
      match.stringData.type = MatchData::StringData::integer_hex;
      return match.stringData.integer <= 0xffff;
    }

    // check match and range
  } catch (invalid_argument) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  } catch (out_of_range) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  }

  return false;
}

bool Parser::matchIntegerHex(const int r, int &l) {
  int hexSize = 3;
  TokenData data;
  string num;
  try {
    l = r;
    if (r + hexSize - 1 < tokenString->size() &&
        (matchDelimiter('\'', r) &&
         lexer->integerTable.exist(data = tokenString->at(r + 1)) &&
         matchDelimiter('\'', r + 2))) {
      // check format
      num = lexer->integerTable.get(data);
      if (!regex_match(num, regex("[0-9a-fA-F]+"))) return false;

      match.stringData.integer = stoi(num, nullptr, 16);
      match.stringData.value = data;
      match.stringData.type = MatchData::StringData::integer_hex;
      l = r + hexSize;
      return match.stringData.integer <= 0xffffff;
    }

    // check match and range
  } catch (invalid_argument) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  } catch (out_of_range) {
#ifdef DEBUGING
    cout << "do what?" << endl;
    cout << lexer->integerTable.get(data) << endl;
#endif
    return false;
  }

  return false;
}

bool Parser::matchString(const int r, int &l) {
  int size = 3;
  TokenData data;
  if (r + size - 1 < tokenString->size() &&
      (matchDelimiter('\'', r) &&
       lexer->stringTable.exist(data = tokenString->at(r + 1)) &&
       matchDelimiter('\'', r + 2))) {
    match.stringData.str = lexer->stringTable.get(data);
    match.stringData.value = data;
    match.stringData.type = MatchData::StringData::string_data;
    l = r + size;
    return true;
  }

  return false;
}

bool Parser::matchDelimiter(char c, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i), lexer->delimiterTable.get(c));
}

bool Parser::matchRegister(const string &reg, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i), lexer->registerTable.get(reg));
}

bool Parser::matchOP(const string &mnemonic, int i) {
  if (i >= tokenString->size()) return false;
  return isTokenEqual(tokenString->at(i),
                      lexer->instructionTable.get(mnemonic));
}

bool Parser::matchOP(int i) {
  if (i >= tokenString->size()) return false;
  return lexer->instructionTable.get(tokenString->at(i)) != "";
}

bool Parser::getOPData(int format, int i) {
  if (i >= tokenString->size()) return false;

  TokenData data = tokenString->at(i);
  string mnemonic = lexer->instructionTable.get(data);

  match.opcode = optab->getOPCode(mnemonic);
  match.format = optab->getFormat(mnemonic);

  if (sicxe == false && optab->isSICXE(mnemonic)) return false;

  return match.opcode != -1 && match.format == format;
}

int Parser::matchN(int i) {
  if (i >= tokenString->size()) return -1;

  TokenData data = tokenString->at(i);
  string num = lexer->integerTable.get(data);
  if (num == "") return -1;

  // check dec
  if (!regex_match(num, regex("[0-9]+"))) return -1;

  // check range 1 ~ 16
  int result;
  try {
    result = stoi(num);
  } catch (out_of_range e) {
    return -1;
  }

  if (1 <= result && result <= 16) return result;

  return -1;
}

bool Parser::matchMemory(const int r) {
  if (r >= tokenString->size()) return false;

  if (matchSymbol(r) || matchIntegerDec(r)) {
    match.memory = tokenString->at(r);
    return true;
  }

  return false;
}

int Parser::matchRegister(int i) {
  if (i >= tokenString->size()) return -1;
  TokenData data = tokenString->at(i);
  if (lexer->registerTable.get(data) == "")
    return -1;
  else
    return data.value - 1;
}

void Parser::setXE(bool XE) { sicxe = XE; }
