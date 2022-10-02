# <sorted_array.hpp>
�i�ѷj�M���T�w�j�p�Ƨǰ}�C

## Types
| Name | Description |
| --- | --- |
| sorted_set | sorted_array �ȧt��Ȫ��O�W |
| sorted_map | sorted_array ��M�g�Ȫ��O�W |

## Classes
| Name | Description |
| --- | --- |
| [sorted_array](#sorted_array) | �T�w�j�p�����Ǳ`�ư}�C |

## Functions
| Name | Description |
| --- | --- |
| [make_sorted_set](#make_sorted_set) | �غc�ȧt��Ȫ����Ǳ`�ư}�C |
| [make_sorted_map](#make_sorted_map) | �غc��M�g�Ȫ����Ǳ`�ư}�C |
| [merge](#merge) | �X�֨í��s�ƧǨ�ռƲ� |

___
### sorted_array
�T�w�j�p�����Ǳ`�ư}�C

```C++
template<
	size_t N,
	class Key,
	class Val = void,
	class Compare = std::less<Key>>
class sorted_array;
```

�۷��}�C�P�U�C�禡���ʸˡG  
* std::sort
* std::lower_bound
* std::upper_bound
* std::equal_range

#### Template parameters
| Name | Description |
| --- | --- |
| N | �}�C�j�p |
| Key | �Ƨǥ���ȫ��O |
| Val | �x�s�����O�A�Y�� void �h�L |
| Compare | Key ���G�� predicate �j�p����Cex�G�ƧǥѤp��j `(a < b)` |

#### Member types
| Name | Description |
| --- | --- |
| key_type | ��� (Key) ���O |
| mapped_type | �X�ݤ��������O�AVal != void ? Val : Key |
| key_compare | ��Ȥ�� predicate  |
| value_type | �j�M�P�M�X�Ȫ����O |
| reference | value_type ���`�Ƥ޼� |
| pointer | value_type ���`�ƫ��� |

#### Member functions
| Name | Description |
| --- | --- |
| [(constructor)](#sorted_arraysorted_array) | ��l�ư}�C�Ƨ� |
| [operator=](#sorted_arrayoperator-1) | �ƻs�P�j�p�}�C�ñƧ� |
| [size](#sorted_arraysize) | �}�C�j�p |
| [data](#sorted_arraydata) | �}�C���� |
| [begin](#sorted_arraybegin) | �_�l��m |
| [end](#sorted_arrayend) | ������m |
| [operator[]](#sorted_arrayoperator-2) | �X�ݤ��� |
| [at](#sorted_arrayat) | �X�ݤ����A�L�ؼЫh�^�L�ĭ� |
| [find](#sorted_arrayfind) | ���o�ŦX����Ȫ��}�l��m |
| [lower_bound](#sorted_arraylower_bound) | ���o�������Ȫ��}�l��m |
| [upper_bound](#sorted_arrayupper_bound) | ���o�������Ȫ�������m |
| [equal_range](#sorted_arrayequal_range) | ���o�ŦX����Ȫ��}�l�P������m |

#### Example
�� [make_sorted_set](#make_sorted_set)�B[make_sorted_map](#make_sorted_map)�B[merge](#merge)  
ex: ���R�奻�����ب���
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
��l�ư}�C�Ƨ�
```C++
constexpr sorted_array(const sorted_array& x);
constexpr sorted_array(const value_type(&x)[N]);
template<class OldCompare>
 constexpr sorted_array(const sorted_array<N, Key, Val, OldCompare>& x);
```
- **Template Parameters**  
`OldCompare` - �ӷ��}�C���ƧǪk  
- **Parameters**  
`x` - �P sorted_array �j�p���ӷ��Ʋ�

##### sorted_array::operator=
�ƻs�P�j�p�}�C�ñƧ�
```C++
constexpr sorted_array& operator =(const sorted_array&);
constexpr sorted_array& operator =(const value_type(&x)[N]);
template<class OldCompare>
 constexpr sorted_array& operator =(const sorted_array<N, Key, Val, OldCompare>& x);
```
- **Template Parameters**  
`OldCompare` - �ӷ��}�C���ƧǪk  
- **Parameters**  
`x` - �P sorted_array �j�p���ӷ��Ʋ�

##### sorted_array::size
�}�C�j�p
```C++
constexpr size_t size() const;
```
- **Return Value**  
template parameter N�C

##### sorted_array::data
�}�C����
```C++
constexpr pointer data() const;
```
- **Return Value**  
�}�C�_�l��m�C

##### sorted_array::begin
�_�l��m
```C++
constexpr pointer begin() const;
```
- **Return Value**  
data()�C

##### sorted_array::end
���t�϶�
```C++
constexpr pointer end() const;
```
- **Return Value**  
data() + size()�C

##### sorted_array::operator[]
�X�ݤ���
```C++
constexpr const mapped_type& operator [](const key_type& k);
```
- **Parameters**  
`k` - ��M���������
- **Return Value**  
���� k ��Ȥ����������ȤޥΡA�Y�L�Ӥ����N�y���D�w�����~�C

##### sorted_array::at
�X�ݤ����A�L�ؼЫh�^�L�ĭ�
```C++
constexpr mapped_type at(const key_type& k, const mapped_type& invalid = mapped_type{}) const;
```
- **Parameters**  
`k` - ��M���������  
`invalid` - �L�����ɪ��^�ǭ�
- **Return Value**  
���� k ��Ȥ����������ȡA�Y�L�����h�^�� invalid �޼ơC

##### sorted_array::find
���o�ŦX����Ȫ��}�l��m
```C++
constexpr pointer find(const key_type& k) const;
```
- **Parameters**  
`k` - ��M���������
- **Return Value**  
���� k ��Ȥ��������СA�Y�L�����h�^�� end()�C

##### sorted_array::lower_bound
���o�������Ȫ��}�l��m
```C++
constexpr pointer lower_bound(const key_type& k) const;
```
- **Parameters**  
`k` - ��M���������
- **Return Value**  
���p�� k ��Ȫ��������СC

##### sorted_array::upper_bound
���o�������Ȫ�������m
```C++
constexpr pointer upper_bound(const key_type& k) const;
```
- **Parameters**  
`k` - ��M���������
- **Return Value**  
�j�� k ��Ȫ��������СC

##### sorted_array::equal_range
���o�ŦX����Ȫ��}�l�P������m
```C++
constexpr std::pair<pointer, pointer> equal_range(const key_type& k) const;
```
- **Parameters**  
`k` - ��M���������
- **Return Value**  
���ĩ� std::make__pair(lower_bound(k), upper_bound(k))�C

##### sorted_array::range
���o�ŦX����d�򪺶}�l�P������m
```C++
constexpr std::pair<pointer, pointer> range(const key_type& min_key, const key_type& max_key) const;
```
- **Parameters**  
`min_key` - ��M�������̤p���
`max_key` - ��M�������̤j���
- **Return Value**  
���ĩ� std::make__pair(lower_bound(min_key), upper_bound(max_key))�C

___
### make_sorted_set
�غc�ȧt��Ȫ����Ǳ`�ư}�C
```C++
template<class T, class Compare = std::less<T>, size_t N>
constexpr auto sorted_set(const T(&arr)[N]);
```

- **Template Parameters**  
`T` - ���X���������O  
`Compare` - �G�� predicate �j�p����Cex�G�ƧǥѤp��j `(a < b)`  
`N` - �}�C�j�p

- **Function Parameters**  
`arr` - ���X���Ҧ���

- **Return Value**  
sorted_set\<N, T, Compare>�C

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
�غc��M�g�Ȫ����Ǳ`�ư}�C
```C++
template<class Key, class Val, class Compare = std::less<Key>, size_t N>
constexpr auto make_sorted_map(const std::pair<Key, Val>(&arr)[N]);
```

- **Template Parameters**  
`Key` - �Ƨǥ���ȫ��O  
`Val` - �M�g��Ȫ����O  
`Compare` - �G�� predicate �j�p����Cex�G�ƧǥѤp��j `(a < b)`  
`N` - �}�C�j�p

- **Function Parameters**  
`arr` - ���X���Ҧ���

- **Return Value**  
sorted_map\<N, Key, Val, Compare>�C

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
�X�֨í��s�ƧǨ�ռƲ�
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
`NewCompare` - �s���Ƨ� predicate �A�p�G�� void �h�H��J�Ѽƪ��ƧǱƧ�  
`Key` - �Ƨǥ���ȫ��O  
`Val` - �M�g��Ȫ����O  
`N1` - �Ĥ@���Ѽƪ��Ʋդj�p  
`Compare1` - �Ĥ@���Ѽƪ��Ƨ�����  
`N2` - �ĤG���Ѽƪ��Ʋդj�p  
`Compare2` - �ĤG���Ѽƪ��Ƨ�����  

- **Function Parameters**  
`x` - �Ĥ@���Ѽ�  
`y` - �ĤG���Ѽ�

- **Return Value**  
sorted_map\<N1 + N2, Key, Val, NewCompare>�C

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
