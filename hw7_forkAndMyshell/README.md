# fork and myshell

## hw7-1
寫一個簡單的程式證明與計算 4 次 fork()後，總共有幾個 process?
```
int main() {
	fork();
	fork();
	fork();
	fork();
}
```

## hw7-2
修改 myShell，改用 execle 實作，要能夠用 PATH 搜尋執行檔。