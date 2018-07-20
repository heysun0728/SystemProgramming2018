# wordcount use pipe

使用 fork、pipe、execv 等函數，創造出二個 child，第一個 child 執行「cat filename」，並透過 PIPE 的方式導入到第二個 child。

## 作業要求
* 第二個 child 執行「wc」。並讓第二個 child 的結果輸出到螢幕上
* 執行檔名稱為 wordcount，該執行檔接受一個參數「filename」
* wordcount 執行結束後，需要在螢幕上印出「filename」的「行數、字數、大小」