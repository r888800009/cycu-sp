# 系統程式第一次程式作業
- 資訊三甲 10612150 林詠翔

## 開發環境與平台
- 開發平台: CPU: Intel i5-6300HQ (4) @ 3.200GHz, Memory: 12G 
- 所選擇的組合語言: SIC & SIC/XE 組合語言
- 使用的程式語言: C++ 11
- 使用開發環境: Linux 64bit (archlinux), vim, g++ (GCC) 9.2.0 C++ 11

本次作業採用c++ 11編寫，並且確保有將編譯器加上參數`-std=c++11 -DNDEBUG`，透過指令進行編譯，採用文字編輯器編寫並透過script執行編譯指令。
``` bash
g++ *.cpp -std=c++11 -DNDEBUG 
```

## 程式設計與流程
### 功能
### 流程
#### Parser
Parser為Recursive Descent Parser的設計，並且採用**深度優先搜尋**，
透過函數定義文法，實際上執行後有建立parser Tree的效果，
並且在剖析的過程中，將剖析出來的資訊如指令格式、定址模式保存起來，

#### Lexer
lexer會返回內容是token的vector

### Assembler

### 資料結構 Lexer

#### LoadingTable
LoadingTable採用C++ 11中STL的unordered_map，標準函數庫實做大多透過hashTable，
LoadingTable建立兩個方向兩個表，雙向查詢token的`(type, value)`與對映的token string如`add`或`sub`，
而查詢`(type, value)`時透過string作為key找到對映的`(type, value)`，
反向則是`(type, value)`可以對映到string。

#### HashTable
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

### Assembler
#### OPTAB
#### SYMTAB
#### LITTAB

#### 組譯的方式
Format1與Format2因為格式特性，實做上面透過stringstream輸出nibble產生Object code的16進位值，
產生Object file時只需要將hex code轉成binary的形式即可，而沒有透過位元運算子來計算結果。

Format3與Format4透過位元運算子進行位移，之後在將多個bit疊在一個int上，
最後產生Object code的byte String，因為每個指令的OP code的最小nibble中後兩位都是0，
實際上使用到只有前6個bit，所以後面兩個要忽略掉。

### Parser
format2幾個指令格式比較特別，在parser這邊先完成剖析之後交給assembler處理

- CLEAR r1
- SHIFTL r1, n
- SHIFTR r1, n
- SVC n
- TIXR r1

## 未完成的功能
- syntax
- 由於作業只需要輸出Object code的hex，因此尚未支援轉成二進位的機器碼檔案的功能。

