# <hash_table.hpp>
靜態雜湊表

## Classes
| Name | Description |
| --- | --- |
| [chained_hash_table](#chained_hash_table) | Separate Chaining Hashing 雜湊表 |
| [hash_table](#hash_table) | Open Addressing - Quadratic Probing 雜湊表 |
| [dhash_table](#dhash_table) | Open Addressing - Double Hashing 雜湊表 |

## Functions
| Name | Description |
| --- | --- |
| [make_hash_set](#make_sorted_set) | 建構僅含鍵值的雜湊表 |
| [make_hash_map](#make_sorted_map) | 建構具映射值的雜湊表 |

___
## chained_hash_table
Separate Chaining Hashing 雜湊表

```C++
template<
	size_t N,
	class Key,
	class Val = void,
	class Hash = std::hash<Key>,
	class Pred = std::equal_to<Key>,
	size_t Bucket = base2::ceil<size_t>(N * 2)>
class chained_hash_table;
```

### Template parameters
| Name | Description |
| --- | --- |
| N | 元素數量 |
| Key | 鍵值型別 |
| Val | 儲存的型別，若為 void 則無 |
| Hash | 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值 |
| Pred | 二元謂詞，以 Key 型別為兩個參數回傳 bool 型別，當表達式為 pred(a,b) 且 a 相等 b 時回傳 true ，反之回傳 false |
| Bucket | 表的大小，當有一個良好的 Hash 時， Bucket - N 值越大碰撞 (Hash collision) 機會越小 |

### Member types
| Name | Description |
| --- | --- |
| key_type | 鍵值 (Key) 型別 |
| mapped_type | 訪問元素的型別，Val != void ? Val : Key |
| hasher | 同 Hash |
| key_equal | 同 Pred |
| value_type | 搜尋與尋訪值的型別 |
| reference | value_type 的常數引數 |
| pointer | value_type 的常數指標 |

### Member functions
| Name | Description |
| --- | --- |
| [(constructor)](#chained_hash_tablechained_hash_table) | Hash Table 建構 |
| [operator=](#chained_hash_tableoperator) | 重新設定 Hash Table |
| [size](#chained_hash_tablesize) | 元素數量 |
| [operator\[\]](#chained_hash_tableoperator-1) | 訪問元素 |
| [at](#chained_hash_tableat) | 訪問元素，無目標則回無效值 |
| [find](#chained_hash_tablefind) | 取得符合條件值的位置 |

### chained_hash_table::chained_hash_table
Hash Table 建構
```C++
constexpr chained_hash_table(const chained_hash_table& x);

constexpr chained_hash_table(const value_type(&x)[N]);

template<class OldHash, class OldPred, size_t OldBucket>
constexpr chained_hash_table(const chained_hash_table<N, Key, Val, OldHash, OldPred, OldBucket>& x)
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldPred` - 來源陣列的 Pred  
`OldBucket` - 來源陣列的 Bucket  
- **Parameters**  
`x` - 同 chained_hash_table 元素數的來源

### chained_hash_table::operator=
重新設定 Hash Table
```C++
constexpr chained_hash_table& operator =(const chained_hash_table&);

constexpr chained_hash_table& operator =(const value_type(&x)[N]);

template<class OldHash, class OldPred, size_t OldBucket>
constexpr chained_hash_table& operator =(const chained_hash_table<N, Key, Val, OldHash, OldPred, OldBucket>& x)
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldPred` - 來源陣列的 Pred  
`OldBucket` - 來源陣列的 Bucket  
- **Parameters**  
`x` - 同 chained_hash_table 元素數的來源

### chained_hash_table::size
元素數量
```C++
constexpr size_t size() const;
```
- **Return Value**  
template parameter N。

### chained_hash_table::operator[]
訪問元素
```C++
constexpr const mapped_type& operator [](const key_type& k);
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的對應值引用，若無該元素將造成非預期錯誤。

### chained_hash_table::at
訪問元素，無目標則回無效值
```C++
constexpr mapped_type at(const key_type& k, const mapped_type& invalid = mapped_type{}) const;
```
- **Parameters**  
`k` - 對映的元素鍵值  
`invalid` - 無對應時的回傳值
- **Return Value**  
等於 k 鍵值元素的對應值，若無元素則回傳 invalid 引數。

### chained_hash_table::find
取得符合條件值的開始位置
```C++
constexpr pointer find(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的指標，若無元素則回傳 nullptr。

___
## hash_table
Open Addressing - Quadratic Probing 雜湊表

```C++
template<
	size_t N,
	class Key,
	class Val = void,
	class Hash = std::hash<Key>,
	class Pred = std::equal_to<Key>,
	size_t Slots = N * 2>
class hash_table;
```

### Template parameters
| Name | Description |
| --- | --- |
| N | 元素數量 |
| Key | 鍵值型別 |
| Val | 儲存的型別，若為 void 則無 |
| Hash | 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值 |
| Pred | 二元謂詞，以 Key 型別為兩個參數回傳 bool 型別，當表達式為 pred(a,b) 且 a 相等 b 時回傳 true ，反之回傳 false |
| Slots | 表的大小，當有一個良好的 Hash 時， Slots - N 值越大碰撞 (Hash collision) 機會越小 |

### Member types
| Name | Description |
| --- | --- |
| key_type | 鍵值 (Key) 型別 |
| mapped_type | 訪問元素的型別，Val != void ? Val : Key |
| hasher | 同 Hash |
| key_equal | 同 Pred |
| value_type | 搜尋與尋訪值的型別 |
| reference | value_type 的常數引數 |
| pointer | value_type 的常數指標 |

### Member functions
| Name | Description |
| --- | --- |
| [(constructor)](#hash_tablehash_table) | Hash Table 建構 |
| [operator=](#hash_tableoperator) | 重新設定 Hash Table |
| [size](#hash_tablesize) | 元素數量 |
| [data](#hash_tabledata) | 儲存的元素陣列指標 |
| [begin](#hash_tablebegin) | 起始位置 |
| [end](#hash_tableend) | 結束位置 |
| [operator\[\]](#hash_tableoperator-1) | 訪問元素 |
| [at](#hash_tableat) | 訪問元素，無目標則回無效值 |
| [find](#hash_tablefind) | 取得符合條件值的位置 |

### hash_table::hash_table
Hash Table 建構
```C++
constexpr hash_table(const hash_table& x);

constexpr hash_table(const value_type(&x)[N]);

template<class OldHash, class OldPred, size_t OldSlots>
constexpr hash_table(const hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x);
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldPred` - 來源陣列的 Pred  
`OldSlots` - 來源陣列的 Slots  
- **Parameters**  
`x` - 同 hash_table 元素數的來源

### hash_table::operator=
重新設定 Hash Table
```C++
constexpr hash_table& operator =(const hash_table&);

constexpr hash_table& operator =(const value_type(&x)[N]);

template<class OldHash, class OldPred, size_t OldSlots>
constexpr hash_table& operator =(const hash_table<N, Key, Val, OldHash, OldPred, OldSlots>& x);
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldPred` - 來源陣列的 Pred  
`OldSlots` - 來源陣列的 Slots  
- **Parameters**  
`x` - 同 hash_table 元素數的來源

### hash_table::size
元素數量
```C++
constexpr size_t size() const;
```
- **Return Value**  
template parameter N。

### hash_table::data
儲存的元素陣列指標
```C++
constexpr pointer data() const;
```
- **Return Value**  
陣列起始位置。

### hash_table::begin
起始位置
```C++
constexpr pointer begin() const;
```
- **Return Value**  
data()。

### hash_table::end
結束位置
```C++
constexpr pointer end() const;
```
- **Return Value**  
data() + size()。

### hash_table::operator[]
訪問元素
```C++
constexpr const mapped_type& operator [](const key_type& k);
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的對應值引用，若無該元素將造成非預期錯誤。

### hash_table::at
訪問元素，無目標則回無效值
```C++
constexpr mapped_type at(const key_type& k, const mapped_type& invalid = mapped_type{}) const;
```
- **Parameters**  
`k` - 對映的元素鍵值  
`invalid` - 無對應時的回傳值
- **Return Value**  
等於 k 鍵值元素的對應值，若無元素則回傳 invalid 引數。

### hash_table::find
取得符合條件值的開始位置
```C++
constexpr pointer find(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的指標，若無元素則回傳 end()。

___
## dhash_table
Open Addressing - Double Hashing 雜湊表

```C++
template<
	size_t N,
	class Key,
	class Val = void,
	class Hash = std::hash<Key>,
	class Hash2 = std::hash<Key>,
	class Pred = std::equal_to<Key>,
	size_t Slots = base2::ceil<size_t>(N * 2)>
class dhash_table;
```

### Template parameters
| Name | Description |
| --- | --- |
| N | 元素數量 |
| Key | 鍵值型別 |
| Val | 儲存的型別，若為 void 則無 |
| Hash | 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值 |
| Hash2 | 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值，碰撞 (Hash collision) 時使用，建議不同於 Hash |
| Pred | 二元謂詞，以 Key 型別為兩個參數回傳 bool 型別，當表達式為 pred(a,b) 且 a 相等 b 時回傳 true ，反之回傳 false |
| Slots | 表的大小，當有一個良好的 Hash 時， Slots - N 值越大碰撞 (Hash collision) 機會越小 |

### Member types
| Name | Description |
| --- | --- |
| key_type | 鍵值 (Key) 型別 |
| mapped_type | 訪問元素的型別，Val != void ? Val : Key |
| hasher | 同 Hash |
| hasher2 | 同 Hash2 |
| key_equal | 同 Pred |
| value_type | 搜尋與尋訪值的型別 |
| reference | value_type 的常數引數 |
| pointer | value_type 的常數指標 |

### Member functions
| Name | Description |
| --- | --- |
| [(constructor)](#dhash_tabledhash_table) | Hash Table 建構 |
| [operator=](#dhash_tableoperator) | 重新設定 Hash Table |
| [size](#dhash_tablesize) | 元素數量 |
| [data](#dhash_tabledata) | 儲存的元素陣列指標 |
| [begin](#dhash_tablebegin) | 起始位置 |
| [end](#dhash_tableend) | 結束位置 |
| [operator[]](#dhash_tableoperator-1) | 訪問元素 |
| [at](#dhash_tableat) | 訪問元素，無目標則回無效值 |
| [find](#dhash_tablefind) | 取得符合條件值的位置 |

### dhash_table::dhash_table
Hash Table 建構
```C++
constexpr dhash_table(const dhash_table& x);

constexpr dhash_table(const value_type(&x)[N]);

template<class OldHash, class OldHash2, class OldPred, size_t OldSlots>
constexpr dhash_table(const dhash_table<N, Key, Val, OldHash, OldHash2, OldPred, OldSlots>& x);
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldHash2` - 來源陣列的 Hash2  
`OldPred` - 來源陣列的 Pred  
`OldSlots` - 來源陣列的 Slots  
- **Parameters**  
`x` - 同 dhash_table 元素數的來源

### dhash_table::operator=
重新設定 Hash Table
```C++
constexpr dhash_table& operator =(const dhash_table&);

constexpr dhash_table& operator =(const value_type(&x)[N]);

template<class OldHash, class OldHash2, class OldPred, size_t OldSlots>
constexpr dhash_table& operator =(const dhash_table<N, Key, Val, OldHash, OldHash2, OldPred, OldSlots>& x);
```
- **Template Parameters**  
`OldHash` - 來源陣列的 Hash  
`OldHash2` - 來源陣列的 Hash2  
`OldPred` - 來源陣列的 Pred  
`OldSlots` - 來源陣列的 Slots  
- **Parameters**  
`x` - 同 dhash_table 元素數的來源

### dhash_table::size
元素數量
```C++
constexpr size_t size() const;
```
- **Return Value**  
template parameter N。

### dhash_table::data
儲存的元素陣列指標
```C++
constexpr pointer data() const;
```
- **Return Value**  
陣列起始位置。

### dhash_table::begin
起始位置
```C++
constexpr pointer begin() const;
```
- **Return Value**  
data()。

### dhash_table::end
結束位置
```C++
constexpr pointer end() const;
```
- **Return Value**  
data() + size()。

### dhash_table::operator[]
訪問元素
```C++
constexpr const mapped_type& operator [](const key_type& k);
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的對應值引用，若無該元素將造成非預期錯誤。

### dhash_table::at
訪問元素，無目標則回無效值
```C++
constexpr mapped_type at(const key_type& k, const mapped_type& invalid = mapped_type{}) const;
```
- **Parameters**  
`k` - 對映的元素鍵值  
`invalid` - 無對應時的回傳值
- **Return Value**  
等於 k 鍵值元素的對應值，若無元素則回傳 invalid 引數。

### dhash_table::find
取得符合條件值的開始位置
```C++
constexpr pointer find(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的指標，若無元素則回傳 end()。

___
## make_hash_set
建構僅含鍵值的雜湊表
```C++
template<class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t N>
constexpr auto make_hash_set(const Key(&arr)[N]);
```

- **Template Parameters**  
`Key` - 集合的元素型別  
`Hash` - 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值  
`Pred` - 二元謂詞，以 Key 型別為兩個參數回傳 bool 型別，當表達式為 pred(a,b) 且 a 相等 b 時回傳 true ，反之回傳 false  
`N` - 陣列大小

- **Function Parameters**  
`arr` - 集合的所有值

- **Return Value**  
hash_table\<N, Key, void, Hash, Pred>。

___
## make_hash_map
建構具映射值的雜湊表
```C++
template<class Key, class Val, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, size_t N>
constexpr auto make_hash_map(const std::pair<Key, Val>(&arr)[N]);
```

- **Template Parameters**  
`Key` - 鍵值型別  
`Val` - 儲存的型別，若為 void 則無  
`Hash` - 一元函數對像類型，以 Key 型別為參數回傳 size_t 型別的雜湊值  
`Pred` - 二元謂詞，以 Key 型別為兩個參數回傳 bool 型別，當表達式為 pred(a,b) 且 a 相等 b 時回傳 true ，反之回傳 false  
`N` - 陣列大小

- **Function Parameters**  
`arr` - 集合的所有值

- **Return Value**  
hash_table\<N, Key, Val, Hash, Pred>。
