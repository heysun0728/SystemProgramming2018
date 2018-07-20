# shell use pipe

修改之前作業shell_sigaction，將新的 shell 名為shell_pipe，新的shell_pipe 可以使用 pipe 串接數個程式，新的例如：
```
ls -R --color / | sort | more
```

將使用 pipe 串接的程式設定為同一 process group，新的使得使用者按下 ctr-c 時可以同時中斷所有
程式。

ex.以 ls -R --color / | sort | more 為例，新的必須讓 ls、sort、more 成為同一個 process group，當使用者按下 ctr-c 時必須送出 ctr-c 給這個 process group