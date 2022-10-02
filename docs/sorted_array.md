# <sorted_array.hpp>
可供搜尋的固定大小排序陣列

## Types
| Name | Description |
| --- | --- |
| sorted_set | sorted_array 僅含鍵值的別名 |
| sorted_map | sorted_array 具映射值的別名 |

## Classes
| Name | Description |
| --- | --- |
| [sorted_array](#sorted_array) | 固定大小的有序常數陣列 |

## Functions
| Name | Description |
| --- | --- |
| [make_sorted_set](#make_sorted_set) | 建構僅含鍵值的有序常數陣列 |
| [make_sorted_map](#make_sorted_map) | 建構具映射值的有序常數陣列 |
| [merge](#merge) | 合併並重新排序兩組數組 |

___
### sorted_array
固定大小的有序常數陣列

```C++
template<
	size_t N,
	class Key,
	class Val = void,
	class Compare = std::less<Key>>
class sorted_array;
```

相當於陣列與下列函式的封裝：  
* std::sort
* std::lower_bound
* std::upper_bound
* std::equal_range

#### Template parameters
| Name | Description |
| --- | --- |
| N | 陣列大小 |
| Key | 排序用鍵值型別 |
| Val | 儲存的型別，若為 void 則無 |
| Compare | Key 的二元 predicate 大小比較。ex：排序由小到大 `(a < b)` |

#### Member types
| Name | Description |
| --- | --- |
| key_type | 鍵值 (Key) 型別 |
| mapped_type | 訪問元素的型別，Val != void ? Val : Key |
| key_compare | 鍵值比對 predicate  |
| value_type | 搜尋與尋訪值的型別 |
| reference | value_type 的常數引數 |
| pointer | value_type 的常數指標 |

#### Member functions
| Name | Description |
| --- | --- |
| [(constructor)](#sorted_arraysorted_array) | 初始化陣列排序 |
| [operator=](#sorted_arrayoperator-1) | 複製同大小陣列並排序 |
| [size](#sorted_arraysize) | 陣列大小 |
| [data](#sorted_arraydata) | 陣列指標 |
| [begin](#sorted_arraybegin) | 起始位置 |
| [end](#sorted_arrayend) | 結束位置 |
| [operator[]](#sorted_arrayoperator-2) | 訪問元素 |
| [at](#sorted_arrayat) | 訪問元素，無目標則回無效值 |
| [find](#sorted_arrayfind) | 取得符合條件值的開始位置 |
| [lower_bound](#sorted_arraylower_bound) | 取得近似條件值的開始位置 |
| [upper_bound](#sorted_arrayupper_bound) | 取得近似條件值的結束位置 |
| [equal_range](#sorted_arrayequal_range) | 取得符合條件值的開始與結束位置 |

#### Example
見 [make_sorted_set](#make_sorted_set)、[make_sorted_map](#make_sorted_map)、[merge](#merge)  
ex: 分析文本的項目取用
```C++
enum : size_t {
	serial_number = 0,
	id,
	name,
	fields_count
};

constexpr auto keys = sorted_array<fields_count, std::string_view, size_t>({
	{"serial_number", serial_number},
	{"id", id},
	{"name", name},
});

std::string items[fields_count];

json_parser json(json_doc);
for (auto& item : json.object()) {
	size_t idx = keys.at(item.name(), fields_count);
	if (fields_count != idx)
		items[idx] = item.value();
}

std::cout << "sn: " << items[serial_number]
	<< ", id: " << items[id]
	<< ", name: " << items[name]
	<< std::endl;

for (auto& s : items)
	s.clear();

xml_parser xml(xml_doc);
for (auto& attr : xml.find("info").attributes()) {
	auto iter = keys.find(attr.name());
	if (iter != keys.end())
		items[iter->second] = attr.value();
}

std::cout << "sn: " << items[serial_number]
	<< ", id: " << items[id]
	<< ", name: " << items[name]
	<< std::endl;
```

#### Methods
##### sorted_array::sorted_array
初始化陣列排序
```C++
constexpr sorted_array(const sorted_array& x);
constexpr sorted_array(const value_type(&x)[N]);
template<class OldCompare>
 constexpr sorted_array(const sorted_array<N, Key, Val, OldCompare>& x);
```
- **Template Parameters**  
`OldCompare` - 來源陣列的排序法  
- **Parameters**  
`x` - 同 sorted_array 大小的來源數組

##### sorted_array::operator=
複製同大小陣列並排序
```C++
constexpr sorted_array& operator =(const sorted_array&);
constexpr sorted_array& operator =(const value_type(&x)[N]);
template<class OldCompare>
 constexpr sorted_array& operator =(const sorted_array<N, Key, Val, OldCompare>& x);
```
- **Template Parameters**  
`OldCompare` - 來源陣列的排序法  
- **Parameters**  
`x` - 同 sorted_array 大小的來源數組

##### sorted_array::size
陣列大小
```C++
constexpr size_t size() const;
```
- **Return Value**  
template parameter N。

##### sorted_array::data
陣列指標
```C++
constexpr pointer data() const;
```
- **Return Value**  
陣列起始位置。

##### sorted_array::begin
起始位置
```C++
constexpr pointer begin() const;
```
- **Return Value**  
data()。

##### sorted_array::end
分配區塊
```C++
constexpr pointer end() const;
```
- **Return Value**  
data() + size()。

##### sorted_array::operator[]
訪問元素
```C++
constexpr const mapped_type& operator [](const key_type& k);
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的對應值引用，若無該元素將造成非預期錯誤。

##### sorted_array::at
訪問元素，無目標則回無效值
```C++
constexpr mapped_type at(const key_type& k, const mapped_type& invalid = mapped_type{}) const;
```
- **Parameters**  
`k` - 對映的元素鍵值  
`invalid` - 無對應時的回傳值
- **Return Value**  
等於 k 鍵值元素的對應值，若無元素則回傳 invalid 引數。

##### sorted_array::find
取得符合條件值的開始位置
```C++
constexpr pointer find(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等於 k 鍵值元素的指標，若無元素則回傳 end()。

##### sorted_array::lower_bound
取得近似條件值的開始位置
```C++
constexpr pointer lower_bound(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
不小於 k 鍵值的元素指標。

##### sorted_array::upper_bound
取得近似條件值的結束位置
```C++
constexpr pointer upper_bound(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
大於 k 鍵值的元素指標。

##### sorted_array::equal_range
取得符合條件值的開始與結束位置
```C++
constexpr std::pair<pointer, pointer> equal_range(const key_type& k) const;
```
- **Parameters**  
`k` - 對映的元素鍵值
- **Return Value**  
等效於 std::make__pair(lower_bound(k), upper_bound(k))。

##### sorted_array::range
取得符合條件範圍的開始與結束位置
```C++
constexpr std::pair<pointer, pointer> range(const key_type& min_key, const key_type& max_key) const;
```
- **Parameters**  
`min_key` - 對映元素的最小鍵值
`max_key` - 對映元素的最大鍵值
- **Return Value**  
等效於 std::make__pair(lower_bound(min_key), upper_bound(max_key))。

___
### make_sorted_set
建構僅含鍵值的有序常數陣列
```C++
template<class T, class Compare = std::less<T>, size_t N>
constexpr auto sorted_set(const T(&arr)[N]);
```

- **Template Parameters**  
`T` - 集合的元素型別  
`Compare` - 二元 predicate 大小比較。ex：排序由小到大 `(a < b)`  
`N` - 陣列大小

- **Function Parameters**  
`arr` - 集合的所有值

- **Return Value**  
sorted_set\<N, T, Compare>。

- **Example**  
```C++
// prime numbers less than 100
//  = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}
constexpr auto prime100
	= sorted_set({2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97});

if (prime100.at(67, 0) == 0)
	std::cout << "67 is prime number.\n";

if (prime100.find(57) != prime100.end())
	std::cout << "57 is prime number.\n";

std::cout << "prime numbers less than 100:\n";
for (auto v : prime100)
	std::cout << v << '\n';
```

___
### make_sorted_map
建構具映射值的有序常數陣列
```C++
template<class Key, class Val, class Compare = std::less<Key>, size_t N>
constexpr auto make_sorted_map(const std::pair<Key, Val>(&arr)[N]);
```

- **Template Parameters**  
`Key` - 排序用鍵值型別  
`Val` - 映射鍵值的型別  
`Compare` - 二元 predicate 大小比較。ex：排序由小到大 `(a < b)`  
`N` - 陣列大小

- **Function Parameters**  
`arr` - 集合的所有值

- **Return Value**  
sorted_map\<N, Key, Val, Compare>。

- **Example**  
```C++
enum {
	aaa = 0,
	bbb,
	ccc
};

// field to index
constexpr auto field__map = sorted_map<std::string_view, int>({
	{"aaa", aaa},
	{"bbb", bbb},
	{"ccc", ccc}
});

std::cout << "\"bbb\" index: " << field__map["bbb"] << '\n';
// error: field__map["abc"]

if (field__map.at("abc", -1) == -1)
	std::cout << "not found \"abc\".\n";

auto iter = field__map.find("bbb");
if (iter != field__map.end())
	std::cout << "not found \"bbb\".\n";
else
	std::cout << "find \"" << iter->first << "\" index: " << iter->second << '\n';

std::cout << "show all fields\n";
for (auto v : field__map)
	std::cout << "\"" << v.first << "\" index: " << v.second << '\n';
```

___
### merge
合併並重新排序兩組數組
```C++
template<class NewCompare = void, class Key, class Val, size_t N1, class Compare1, size_t N2, class Compare2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>& x, const sorted_array<N2, Key, Val, Compare2>& y);

template<class NewCompare = void, class Key, class Val, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, Val, Compare1>& x, const std::pair<Key, Val>(&y)[N2]);

template<class NewCompare = void, class Key, class Val, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const std::pair<Key, Val>(&x)[N2], const sorted_array<N1, Key, Val, Compare1>& y);

template<class NewCompare = void, class Key, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const sorted_array<N1, Key, void, Compare1>& x, const Key(&y)[N2]);

template<class NewCompare = void, class Key, size_t N1, class Compare1, size_t N2>
constexpr auto merge(const Key(&y)[N2], const sorted_array<N1, Key, void, Compare1>& x);
```

- **Template Parameters**  
`NewCompare` - 新的排序 predicate ，如果為 void 則以輸入參數的排序排序  
`Key` - 排序用鍵值型別  
`Val` - 映射鍵值的型別  
`N1` - 第一項參數的數組大小  
`Compare1` - 第一項參數的排序類型  
`N2` - 第二項參數的數組大小  
`Compare2` - 第二項參數的排序類型  

- **Function Parameters**  
`x` - 第一項參數  
`y` - 第二項參數

- **Return Value**  
sorted_map\<N1 + N2, Key, Val, NewCompare>。

- **Example**  
```C++
// foo = {5, 3, 1}
constexpr auto foo = sorted_set<int, std::greater<int>>({1, 3, 5});

// bar = {1, 2, 3, 4, 5, 6}
constexpr auto bar = merge<std::less<int>>(foo, {2, 4, 6});
// = merge<std::less<int>>({4, 2, 6}, foo)

// sort by foo
// bar = {6, 5, 4, 3, 2, 1}
constexpr auto baz = merge(foo, {2, 4, 6});
// = merge({4, 2, 6}, foo)

// sort by bar
// qux_less = {1, 1, 2, 3, 3, 4, 5, 5, 6}
constexpr auto qux_less = merge(bar, foo);

// sort by foo
// qux_greater = {6, 5, 5, 4, 3, 3, 2, 1, 1}
constexpr auto qux_greater = merge(foo, bar);
```
