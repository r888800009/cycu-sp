# 系統程式第一次程式作業
- 資訊三甲 10612150 林詠翔

## 開發環境與平台
- 開發平台: CPU: Intel i5-6300HQ (4) @ 3.200GHz, Memory: 12G 
- 所選擇的組合語言: SIC組合語言
- 使用的程式語言: C++ 11
- 使用開發環境: Linux 64bit (archlinux), vim, g++ (GCC) 9.2.0 C++ 11

本次作業採用c++ 11編寫，並且確保有將編譯器加上參數`-std=c++11 -DNDEBUG`，透過指令進行編譯，採用文字編輯器編寫並透過script執行編譯指令。
``` bash
g++ *.cpp -std=c++11 -DNDEBUG 
```

## 使用方式
### 交互模式:
不加入任何參數執行預設則會進入交互模式，可以輸入檔名，並且輸入`.`可以退出程式。
``` bash
./a.out
```

進入交互模式，輸入檔名
``` plaintext
Interactive mode
or "./a.out input.txt"
type "." quit
>
```
之後會輸出一個後綴為`_output.txt`的檔案

### 參數模式:
載啟動程式之前加上檔名作為參數，則程式會自動讀取此參數並進行Lexical Analysis
``` bash
./a.out SIC/SIC_input.txt
```

之後會輸出一個`SIC_output.txt`檔案

## 程式設計與流程
### 編寫的方法
透過限狀態機實做lexer，
並且有**一般指令**、**字串**、**數字**、**註解**等四個狀態，

### 一般指令
在一般指令的狀態下，會不斷讀取字元，並且保存到token變數中，
遇到whitespace時判斷token是否有內容，若有則保存並輸出type與value，
又或者讀到delimiter時，依然判斷前面使否有內容，若delimiter為單引號`'`
時要判斷前方式否為`x`或`c`並且**不考慮大小寫**，若為`c`則切換到string
狀態，若為`x`則切換到數字狀態。

在一般指令狀態下token可以被解讀為delimiter, 指令, pseudo, register, integer以及
symbol六個，若不是前四者也不是數字的一律當成symbol，而如何判斷是否為數字採用
`[0-9]+`等正則表達式下去檢查。

### 數字或字串
數字與字串狀態，都會將所有的內容保存到各自的表，字串表中將不分大小寫並且包含whitespace
的保存到string table，而數字模式的話則會保存所有

### 註解
註解模式只需要標記含有註解符號即可轉跳下行，依然保留一個狀態以利於未來擴充功能。

在切換各種模式之前，會將已經讀到的delimiter的輸出，之後另外一個模式如字串或數字則是

### 資料結構
 
LoadingTable採用C++ 11中STL的unordered_map，標準函數庫實做大多透過hashTable，
LoadingTable建立兩個方向兩個表，雙向查詢token的`(type, value)`與對映的token string如`add`或`sub`，
而查詢`(type, value)`時透過string作為key找到對映的`(type, value)`，
反向則是`(type, value)`可以對映到string。

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

### 除錯的方式
這裡的除錯採用檔案進行完整測試以及單元測試，單元測試寫在`test.cpp`當中，
並且需要定義`#define DEBUGING`才會執行，並且在裡面編寫容易出錯的測試，
並且在單元測試檔案裡面編寫斷言，在程式出錯的時候會自動在該地點停下。

檔案測試是透過一些SIC或其他架構的組合語言，並且檢查確定測資合理之後將檔案保存下來，
之後可以在修改程式或重構的時候，立即抓到相關的錯誤。

## 未完成的功能
大致完成，但是如果輸入非預期的文字如中文字可能會崩潰。
