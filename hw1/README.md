# 系統程式第一次程式作業
- 資訊三甲 10612150 林詠翔

## 開發環境與平台
- linux (archlinux) 64bit
- gcc (GCC) 9.2.0 C++ 11
- SIC組合語言

並且確保有將編譯器加上參數`-std=c++11 -DNDEBUG`，採用文字編輯器編寫並透過script執行編譯指令。
``` bash
g++ *.cpp -std=c++11 -DNDEBUG 
```

## 使用方式
### 交互模式:
不加入任何參數執行預設則會進入交互模式，可以輸入檔名，並且輸入`.`可以退出程式。
``` bash
./a.out
```

進入交互模式
``` plaintext
Interactive mode
or "./a.out input.txt"
type "." quit
>
```

### 參數模式:
載啟動程式之前加上檔名作為參數，則程式會自動讀取此參數並進行Lexical Analysis
``` bash
./a.out SIC/SIC_input.txt
```

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

在一般指令狀態下token可以被解讀為delimiter, 指令, pseudo, register,integer以及
symbol六個，若不是前四者也不是數字的一律當成symbol，而如何判斷是否為數字採用
`[0-9]+`等正則表達式下去檢查。

### 數字或字串
數字與字串狀態，都會將所有的內容保存到各自的表，字串表中將不分大小寫並且包含whitespace的保存到string table，而數字模式的話則會保存所有

### 註解
註解模式只需要標記含有註解符號即可轉跳下行，依然保留一個狀態以利於未來擴充功能。

在切換各種模式之前，會將已經讀到的delimiter的輸出，之後另外一個模式如字串或數字則是

### 除錯的方式
