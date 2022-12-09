# Dlou
**Dlou** 程式庫為各種 C++ 的資料結構實現

## Features
* 保留 pointer 的使用以兼容 C 語言庫
* 減少 dynamic memory 的使用
* 通過前置處理增進運行時效能
* 預設不驗證引數的正確性，由使用者驗證以避免重複驗證

## Usage
這是 C++ Header only library ，直接複製 /include 目錄至安裝路經。  
設置 include directories 後，在程式中 `#include <dlou/**.hpp>` 使用庫。

## Support
通過 `#include <macro.hpp>` 檢測是否支援程式庫  

## Reference
| Include | Description |
| --- | --- |
| [buddy.hpp](docs/buddy.md) | Buddy memory allocation 實作 |
| integer.hpp | 整數算法 |
| [macro.hpp](docs/macro.md) | Marco 定義與功能設置註記 |
| [sorted_array.hpp](docs/sorted_array.md) | 可供搜尋的固定大小排序陣列 |
| [hash_table.hpp](docs/hash_table.md) | 靜態雜湊表 |
| functor_array.hpp | Function objects 索引查詢 |
| functor_map.hpp | Function objects 鍵值查詢 |
| sorted_functor.hpp | Function objects 依鍵值重排序，並提供鍵值查詢 |
| skew_heap.hpp | 不含記憶體分配的斜堆積實現 |
| leftist_heap.hpp | 不含記憶體分配的左偏樹實現 |
| singly_linked_list.hpp | 不含記憶體分配的單向鏈結串列實現 |
| doubly_linked_list.hpp | 不含記憶體分配的雙向鏈結串列實現 |

## Index
### B
[buddy](docs/buddy.md#buddy)  
### C  
[chained_hash_table](docs/hash_table.md#chained_hash_table)  
### D
[dhash_table](docs/hash_table.md#dhash_table)  
[DLOU_CHECK_ARGS](docs/macro.md)  
[DLOU_NO_ALIAS](docs/macro.md)  
### H  
[hash_table](docs/hash_table.md#hash_table)  
### M
[make_sorted_map](docs/sorted_array.md#make_sorted_map)  
[make_sorted_set](docs/sorted_array.md#make_sorted_set)  
[merge](docs/sorted_array.md#merge)  
### S
[simple_buddy](docs/buddy.md#simple_buddy)  
[sorted_array](docs/sorted_array.md#sorted_array)  
