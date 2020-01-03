# 系統程式第三次程式作業 FRANCIS Compiler
- 資訊三甲 10612150 林詠翔

## 開發環境與平台
- 開發平台: CPU: Intel i5-6300HQ (4) @ 3.200GHz, Memory: 12G 
- 開發環境:  Linux 64bit (archlinux), vim, g++ (GCC) 9.2.0 C++ 17, 
gcc (GCC) 9.2.0, bison (GNU Bison) 3.5, flex 2.6.4

## 使用方式
檢視說明文件
``` bash
chromium README.html
```

下載原始碼解壓縮並且編譯
``` bash
unzip 10612150.zip
cd 10612150
bison main.y -d -v -o main.tab.cpp
flex lexer.l
gcc -c ./lex.yy.c -o lex.yy.o
g++ lex.yy.o *.cpp -lfl -std=c++17 -DNDEBUG 
```

執行之後可以輸入檔名產生結果，按`ctrl + C`或`ctrl + D`退出程式。
```bash
./a.out
```

執行結果會輸出到`stdout`與原檔名加上`.output`的檔案
``` plaintext
./a.out
input filename:
```

## 程式設計
程式採用`yacc`與`lex`進行語法與詞法分析。

### 功能
1. `if else`能夠支援`A GT B AND A GT B OR A GT B`多個邏輯運算子組成的寫法。
2. 支持`INPUT`與`OUTPUT`
3. 支持定義變數、陣列、label等
4. 支持對陣列或變數進行四則運算與assignment
5. `GTO`支援向前參考
6. 四則運算時，最外層的括號會被優先處裡中間碼，之後才產生同層級的四則運算。
7. 可以讀寫檔案，並且輸入任意檔名。
8. 能夠找出缺少`;`、`)`的行數，檢測`ENP`與`ENS`是否誤用，檢查錯別字如`LABLE`。
9. 能夠找出`undefined identifier`與`duplicate identifier`
10. 報錯`label statement`時，發生的錯誤如重複定義、錯誤型別等，
並且在最後檢查是否有未定義的label。
11. 檢測陣列維度錯誤
12. call function時可以檢查undefine subroutine
13. 檢查duplicate subroutine defined

### IF ELSE
`if else`在遇到`THEN`關鍵字時記錄`if`中間碼的位置並且設定true statement的中間碼，
在完成true statement時補上if中間碼指向false statement的位置並且記錄一個`GTO`中間碼位置，
避免true statement執行完成後執行false statement。

在false statement完成後，會補上false statement後一個中間碼的位置給`GTO`使用。

``` c++
int ifBeginIndex, ifTrueBranchIndex;
void putIfBeginCode(TokenData condition);
void putIfTrueEndCode();
void putIfFalseCode();
```

### 缺少分號
缺少分號時，為了標記哪種錯誤採用`check`這個變數記錄，
並且在報錯的時候呼叫報錯函數即可確認是哪種錯誤。

並且為了找到正確的行數，`semicolonIndex`這個變數用來記錄出錯行數。
``` c++
enum Check {check_disable, check_semicolon, check_brackets ,check_ens, check_enp} check;
int semicolonIndex;
```

`yacc`在定義的時候，在terminal symbol前後設定除錯資訊即可在報錯的時候找出原因。
``` yacc
semicolon: {check = check_semicolon; semicolonIndex = lineno;} ';' {check = check_disable;};
```

### 正負號
遇到負號決定產生中間碼`T = 0 - X`或`T = 0 + X`

### 括號優先
為了讓四則運算可以括號優先輸出中間碼，中間碼在剖析的時候會將它放到一個優先佇列當中，
在assignment或找到右括號的時候會依序輸出中間碼。

``` c++
typedef struct DelayQForm {
  QuadrupleForm qform;
  int bracketDeep;
  int index;
  bool operator<(const DelayQForm& dqform) const
  {
    if (bracketDeep == dqform.bracketDeep)
      return index > dqform.index;

    return bracketDeep < dqform.bracketDeep;
  }

  bool operator==(const DelayQForm& dqform) const
  {
    return bracketDeep == dqform.bracketDeep;
  }
} DelayQForm;

priority_queue<DelayQForm> delayQFormQueue;
int delayTmpIndex;

void push_bracket();
TokenData pop_bracket();
void addDelayQForm(QuadrupleForm qform);
```

但也因為延後輸出中間碼，因此暫存器`T`沒辦法確認，先踩用格外定義的暫存器，之後產生出實際的
暫存器時透過map這個資料結構對映。
``` c++
int delayTmpRegIndex;
map<int, TokenData> delayTmpMap;
TokenData getDelayReg();
bool isDelayRegToken(TokenData token);
```

## Delimiters Table與Reserved Word Table
Delimiters Table與Reserved Word Table因為已經在lex與Yacc定義相關的token資訊，
僅僅將這兩個Table的數值填上對映的Token，產生中間碼的時候根據這兩個Table的定義得出結果。
``` c++
LoadingTable delimiterTable = LoadingTable(DELIMITER_TABLE);
LoadingTable reservedTable = LoadingTable(RESERVED_WORD_TABLE);
```

## Integer table與real number table
兩個階採用ascii code總和mod 100 作為hash function，
透過正則匹配是整數或浮點數放到各自的表格當中。
``` c++
HashTable intTable(INTEGER_TABLE);
HashTable realTable(REAL_NUMBER_TABLE);
```

## Identifier Table
`idTableData`為Identifier Table本體，採用ascii code總和mod 100 作為hash function。
`undefineGTO`用來處裡`GTO`向前參考，如configuration table。
``` c++
typedef struct IDData {
  string id;
  int scope;
  int type;
  int pointer;
  bool defined;
} IDData;

IDData idTableData[HASHTABLE_SIZE];
int firstUndefineLine[HASHTABLE_SIZE];
vector<int> undefineGTO[HASHTABLE_SIZE];
```

## Information table
用於記錄陣列維度定義，`call`時保存參數到此表格中。
``` c++
vector<int> infoTable;
```

## quadruple table 
用於保存中間碼，並且在程式剖析完成或者發生錯誤時，將此表格打印出來。
``` c++
typedef struct QuadrupleForm {
  TokenData opr;         // operator
  TokenData opd1, opd2;  // operand
  TokenData result;
} QForm;

vector<QForm> qfromTable;
```

## 未完成的功能
- `if else`會有累贅的`GTO`，比方說`IF cond THEN ELSE;`會有GTO在裡面。
- 在結尾時遇到`label ENP;`或`label ENS;`可能會產生錯誤，
因為yacc匹配到`label statement;`導致錯誤，
尚未修復完成，也因此在`ENP`與`ENS`前面定義的label無法解決中間碼。
- 多個括號時，只會讓最外側的括號優先執行。
