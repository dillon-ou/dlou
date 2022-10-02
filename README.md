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

## Index
### B
[buddy](docs/buddy.md#buddy)  
### D
[DLOU_CHECK_ARGS](docs/macro.md)  
[DLOU_NO_ALIAS](docs/macro.md)  
### M
[make_sorted_map](docs/sorted_array.md#make_sorted_map)  
[make_sorted_set](docs/sorted_array.md#make_sorted_set)  
[merge](docs/sorted_array.md#merge)  
### S
[simple_buddy](docs/buddy.md#simple_buddy)  
[sorted_array](docs/sorted_array.md#sorted_array)  
