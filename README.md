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
Header 描述  

### Utility
| Include | Description |
| --- | --- |
| [macro.hpp](docs/macro.md) | Marco 定義與功能設置註記 |
| static.hpp | 靜態類別 |
| integer.hpp | 整數算法 |
| glvalue.hpp | Generalized lvalue |
| pair_int.hpp | 分為高低位儲存的整數計算 |

### Dynamic memory management
| Include | Description |
| --- | --- |
| [buddy.hpp](docs/buddy.md) | Buddy memory allocation |

### Fixed map
| Include | Description |
| --- | --- |
| [sorted_array.hpp](docs/sorted_array.md) | 可供搜尋的固定大小排序陣列 |
| [hash_table.hpp](docs/hash_table.md) | 靜態雜湊表 |
| functor_array.hpp | Function objects 索引查詢 |
| functor_map.hpp | Function objects 鍵值查詢 |
| sorted_functor.hpp | Function objects 重排序的鍵值查詢 |

### Container
| Include | Description |
| --- | --- |
| node.hpp | 連結節點 |
| const_iterator.hpp | 常數迭代器 |
| skew_heap.hpp | 斜堆積 |
| leftist_heap.hpp | 左偏樹 |
| xor_linked_list.hpp | XOR 鏈結串列 |
| singly_linked_list.hpp | 單向鏈結串列 |
| doubly_linked_list.hpp | (雙端)雙向鏈結串列 |
| double_ended/singly_linked_list.hpp | 雙端單向鏈結串列 |
| double_ended/doubly_linked_list.hpp | 雙端雙向鏈結串列 |
| circular/singly_linked_list.hpp | 環狀單向鏈結串列 |
| circular/doubly_linked_list.hpp | 環狀雙向鏈結串列 |

### Container wrapper
| Include | Description |
| --- | --- |
| stack.hpp | 以鏈結串列為基礎的堆疊封裝 |
| queue.hpp | 以鏈結串列為基礎的佇列封裝 |
| deque.hpp | 以鏈結串列為基礎的雙向佇列封裝 |
| priority_queue.hpp | 以堆積為基礎的優先級佇列封裝 |

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
