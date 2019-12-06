# 系統程式第二次程式作業 交叉組譯器
- 資訊三甲 10612150 林詠翔

## 開發環境與平台
- 開發平台: CPU: Intel i5-6300HQ (4) @ 3.200GHz, Memory: 12G 
- 所選擇的組合語言: SIC & SIC/XE 組合語言
- 使用的程式語言: C++ 17
- 使用開發環境: Linux 64bit (archlinux), vim, g++ (GCC) 9.2.0 C++ 17

本次作業採用c++ 17編寫，並且確保有將編譯器加上參數`-std=c++17 -DNDEBUG`，
透過指令進行編譯，採用文字編輯器編寫並透過script執行編譯指令。
``` bash
g++ *.cpp -std=c++17 -DNDEBUG 
```

## 使用方式
### 編譯
下載原始碼解壓縮並且編譯
``` bash
unzip 10612150.zip
cd 10612150
g++ *.cpp -std=c++17 -DNDEBUG 
```

檢視說明文件
``` bash
chromium README.html
``` 

輸入`./a.out`即可執行程式

### 交互模式
執行進入交互模式，可以輸入檔名，輸入`.`退出程式。
``` bash
./a.out
```

進入交互模式輸入檔名
``` plaintext
Interactive mode
or "./a.out input.txt"
type "." quit
>
```

檔名輸入後選取模式，這裡假設使用`SIC_output.txt`，這時可以輸入`1`使用SIC模式。
``` plaintext
SaveName: "SIC_output.txt"
[1] SIC
[2] SICXE
>
```

程式會輸出Assembly Listing到**stdout**與後綴為`_output.txt`的**檔案**。

## 程式設計與流程

## 功能
同時支援SIC與SICXE組譯，並且產生不同格式的object code，
並且輸出Assembly Listing。

- `EQU`: **四則運算**與**2 pass**處理，並且能在指令前面顯示`EQU`定義的地址，
並且`EQU`指令強制定義Symbol，否則syntex error
- literal: 透過`=X'f1'`或`=C'str'`的方式定義literal，
數字允許產生3 bytes或1 byte的object code。
- 可以透過`WORD`與`BYTE`偽指令定義數字或字串到記憶體中，
定義字串時能夠定義不定長度的內容
- 如果沒有定義`START`時，預設會以`0`起始PC，缺少`START`、`END`時會報錯

- `BASE`: 定義後可以採用BASE進行relocation，只在sicxe模式運作，
如果沒有定義`BASE`時，採用PC定址時超過數值則會報錯。

- 程式能夠找出Syntex error並且顯示行數，如字串或數字沒有c的情況會報錯`'123'`，
或者其他語法錯誤Parser也可以抓出來，當disp範圍不夠使用時也會報錯。


## Assembler
程式會先載入檔案，一行行透過lexer得到token，之後傳入Parser處理完成，
在透過Assembler獲得Parser的結果決定翻譯偽代碼或指令。

### pass 1
1. 載入檔案
2. 採用lexer與Parser剖析指令，使用OPTAB獲得指令格式與opcode
3. 嘗試保存symbol到SYMTAB
4. 根據指令或偽指令計算`locationCounter`，處理偽指令

pass1會剖析指令的時候會將symbol定義到symtab，
並且找出duplicated symbol，symtab遇到兩次定義則會丟出 duplicated symbol

雖然這時symtab會throw undefined symbol，
但未定義符號在pass2之後才能正確確定。因此pass1的try catch
可以忽略undefined symbol，直到pass2的時候處裡。

### pass2
1. 重頭讀取檔案
2. 採用lexer與Parser剖析指令，使用OPTAB獲得指令格式與opcode
3. 根據指令或偽指令計算`locationCounter`，
處理偽指令，並且產生Object code
4. 輸出寫檔

pass2時找出undefined symbol

雖然這時symtab會throw duplicated symbol，
但實際上duplicated symbol已經在pass1處理好，因此try catch忽略此錯誤，
這時throw是在pass1定義好的symbol。

### 產生機器碼 
如果使用SIC模式時，一律採用simple addressing，SICXE模式時，
按照有無`#`或`@`來決定定址模式，`RSUB`一律採用simple addressing，
並且disp或addr填入`0`並產生機器碼，任何格式的指令都會計算locationCounter。

format3或4時可能擁有數字、literal、symbol相關數值，若有literal則在Pass1時
定義並且分配地址。

翻譯format3 object code:

- token是數字: 直接
- token是symbol或literal等，嘗試順序如下
  1. 嘗試用PC重定位
  2. 嘗試用BASE重定位(必須定義`BASE`指令)

format4則是直接填入地址(20 bit)，並且將`e` flag設為`1`。

Format1與Format2因為格式特性，實做上面透過stringstream輸出nibble產生Object code的16進位值，
產生Object file時只需要將hex code轉成binary的形式即可，而沒有透過位元運算子來計算結果。

Format3與Format4透過位元運算子進行位移，之後在將多個bit疊在一個int上，
最後產生Object code的byte String，因為每個指令的OP code的最小nibble中後兩位都是0，
實際上使用到只有前6個bit，所以後面兩個要忽略掉。

## Lexer
lexer會返回內容是token的vector，並且將token傳入Parser處理

### LoadingTable
LoadingTable採用C++ 17中STL的unordered_map，標準函數庫實做大多透過hashTable，
LoadingTable建立兩個方向兩個表，雙向查詢token的`(type, value)`與對映的token string如`add`或`sub`，
而查詢`(type, value)`時透過string作為key找到對映的`(type, value)`，
反向則是`(type, value)`可以對映到string。

### HashTable
HashTable透過字串中所有的ascii相加後取mod 100的進行索引，裡面含有一個陣列並且保存
碰撞採用linear probing

透過`empty`來判斷是否存放資料，並且透過`value`保存內容值
``` c++
typedef struct HashTableData {
  bool empty = true;
  string value;
} HashTableData;
```

而`Instruction`、`PseudoExtra`、`Register`、`Delimiter`皆是採用LoadingTable這個class處理，
而背後實際上是兩個HashTable，`Integer/real`、`String`、`Symbol`皆是採用自己編寫HashTable。

## OPTAB
透過`OPtable.tsv`保存指令助憶符號與指令格式的相關定義，
並且設計一個class來負責維護optab。

在初始化時先載入`OPtable.tsv`，之後parser或assembler可以透過optab獲取指令opcode或格式等。

``` c++
typedef struct Data {
  int opcode, format;
  bool sicxe;
} Data;
```

## SYMTAB
symtab為一個class並且能夠與透過try catch處理symbol相關的錯誤。
內部使用資料結構如下
``` c++
enum Type { absolute_address, symbol_reference, null_symbol };
typedef struct Data {
  int value = -1;
  Type addressType = null_symbol;
} Data;
```

在pass1或2的呼叫`define()`，如果遇到重複定義的symbol則會throw duplicated symbol，`getSymbolAddress()`則會throw undefined symbol。
``` c++
  void define(int value, int, Type);
  int getSymbolAddress(int value);
```

## LITTAB
採用HashTable class與另一個struct保存地址相關的資料，
當中`pass2Wait`這個flag用來標示輸出pass1定義好的literal。
``` c++
typedef struct HashTableData {
  int address;
  TokenData token;
  bool gen, empty;
  string value;
} HashTableData;

HashTable tableData1;
HashTableData tableData2[HASHTABLE_SIZE];
bool pass2Wait[HASHTABLE_SIZE];
```

不過這個部份因為採用上一次作業的lexer，要把token的字串提出來之後，
保存進入littab，因此有點繁瑣不能透過token value處理。

``` c++
int getAddress(const string& value);

TokenData put(const string& value, TokenData token);
int genLiteral(int curPC, vector<TokenData>& genTokens);

int genLiteralPass2(int curPC, vector<TokenData>& genTokens);
void putPass2(const string& value);
```

`getAddress()`在pass2的時候會被呼叫，用來顯示literal定義的地址，
並且確保之前沒有輸出過literal的object code才輸出。
``` c++
int getAddress(const string& value);
```

`put(const string& value, TokenData token)`指令在剖析指令的時候，
遇到literal時會呼叫此函數，用來通知littab定義literal地址
，而`putPass2()`僅僅用來標記輸出，不重新定義literal，因為在pass 1已經定義地址。
``` c++
TokenData put(const string& value, TokenData token);
void putPass2(const string& value);
```

`genLiteral`與`genLiteralPass2`只在遇到`END`或`LTORG`時呼叫，
前者用來分配每個literal的地址，後者則產生objcode並且顯示literal定義。
``` c++
int genLiteral(int curPC, vector<TokenData>& genTokens);
int genLiteralPass2(int curPC, vector<TokenData>& genTokens);
```

## Parser
Parser為Recursive Descent Parser的設計，並且採用**深度優先搜尋**，
透過函數定義文法，實際上執行後有建立parser Tree的效果，
並且在剖析的過程中，將剖析出來的資訊如指令格式、定址模式保存起來，

Parser的語法主要透過多個函數下去呼叫，可以當成一種定義文法的資料結構，
向下Parsing嘗試批配terminal symbol，根據不同輸入內容展開成Parsing Tree。

定義一個函數作為root，並且向下展開Parsing Tree剖析指令並且回傳是否成功。

在parser這邊先完成剖析之後交給assembler處理，
assembler在透過批配的格式判斷要當成指令或者偽代碼處理，
之後會根據各種指令格式從Parser判斷是否符合Symbol或數字，
或者定義了字串等。

如Parser若遇到`,X`定義時，則會標記使用`X`暫存器，
Assembler處理時發現定義此flag，產生出來的object code會將`x`設為1。

定址模式也是在Parser進行確認，批配文法根據有無`#`、`@`決定定址模式

- 若沒有則是simple定址
- `#`: 立即值
- `@`: indirect

而指令為format3或format4也在Parser運行時進行判斷，若有`+`則是format4，反之format3。

### 結構
``` c++
  struct MatchData {
    TokenData preSymbol;
    struct StringData {
      TokenData value;
      unsigned int integer;
      string str;
      enum Type { integer_hex, integer_dec, string_data, null } type;
    } stringData;

    TokenData memory, literal;
    AddressingType addrType;
    bool x;

    Pseudo pseudo;

    unsigned int startMatch;
    TokenData baseMatch;
    TokenData resMatch;
    vector<TokenData> equMatch;

    int opcode, format;
    int op1, op2;

  } match;
```

### 特例
format2幾個指令格式比較特別，並且根據back書中定義產生

- `CLEAR r1`時`r2 = 0`
- `SHIFTL r1, n`與`SHIFTR r1, n`時`r2`會設為`n - 1`
- `SVC n`時`r1 = n`, `r2 = 0`
- `TIXR r1`時`r2 = 0`

format3的`RSUB`也需要特別提出來解決，並且simple定址，

### EQU
遇到`EQU`指令時，若為多個token，則會檢查是否為正確的運算式，
並且將運算式轉成後序式，之後將結果交給assembler處理，
Assembler則實做後序式計算機來計算地址。

## 未完成的功能
- 由於作業只需要輸出Assembler Listing，因此尚未支援轉成二進位的機器碼檔案obj file的功能。
- literal中`=x'1'`與`=c'1'`可能有所衝突，使用相同的地址但兩個objcode不應該相同。
- `EQU`與其他指令不能處理負號Symbol(`- LABLE`)，如`sym3 equ -sym1 + sym2`
- 多重pass的`EQU`

## 備註
- hi lo不需要反轉
