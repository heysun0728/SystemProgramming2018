# ACP

寫一隻程式 acp，他在複製檔案的時候會先製造一個 tmpFile，等到檔案複製結束，再用 move 的方式，將檔案移動到指定的位置，並給予指定的檔案

ex. acp file1 file2，將 file1 複製到 file2，但是複製的過程先產生暫存檔案，等複製結束，再
將暫存檔案 move 到 file2 的位置