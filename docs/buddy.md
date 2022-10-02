# <buddy.hpp>
Buddy memory allocation 實作

## Classes
| Name | Description |
| --- | --- |
| [buddy](#buddy) | Buddy memory allocation |
| [simple_buddy](#simple_buddy) | buddy的再封裝，更簡單的分配所需記憶體大小與位移位置 |

___
## buddy
Buddy memory allocation  

```C++
template<uint8_t Level>
class buddy;
```

這裡參考了 [wuwenbin](https://github.com/wuwenbin) 的 [buddy2](https://github.com/wuwenbin/buddy2) 算法，  
建立一個 full binary tree 的陣列，  
child 代表 parent 的分割空間，  
node 紀錄 subtree 可分配的最大空間，  
allocate 時由 root 往 leaves 找到需要大小的分配空間，  
release 時由 leaf 往 root 找已經分配的空間。

### Template parameters
| Name | Description |
| --- | --- |
| Level | 區塊層級數 |

### Member constants
| Name | Description |
| --- | --- |
| max_level | 區塊最大層級 |
| max_size | 2^max_level^ 為可分配最大的區塊單位數 |

### Member functions
| Name | Description |
| --- | --- |
| [empty](#buddyempty) | 無分配任何區塊 |
| [full](#buddyfull) | 無可用空間 |
| [allocate](#buddyallocate) | 分配區塊 |
| [allocate_by_pow2](#buddyallocate_by_pow2) | 分配 2^n^ 大小區塊 |
| [release](#buddyrelease) | 釋放區塊 |
| [release_by_pow2](#buddyrelease_by_pow2) | 根據 allocate_by_pow2 來釋放區塊 |

### Example
```C++
// Buddy memory allocator(256 ~ 4096)
// Base: 256
// Level: 4 (4096 = 256 * pow2(4))
char* block = new char[4096];
buddy<4> head; // pow2(4) = 4096 / 256

// allocate 1024 bytes
size_t offset1 = head.allocate_by_pow2(2); // pow2(2) = 1024 / 256
char* buf1 = (none == offset1) ? nullptr : block + 256 * offset1;

// allocate 512 bytes
size_t offset2 = head.allocate(512 / 256);
char* buf2 = (none == offset2) ? nullptr : block + 256 * offset2;

// deallocate
if (buf2)
	head.release(offset2);
if (buf1)
	head.release_by_pow2(offset1, 2); //head.release(offset1);

delete[]block;
```

### buddy::empty
無分配任何區塊
```C++
bool empty() const;
```
- **Return Value**  
`true` 為尚未使用，否則為 `false` 。

### buddy::full
無可用空間
```C++
bool full() const;
```
- **Return Value**  
`true` 為無空餘空間，否則為 `false` 。

### buddy::allocate
分配區塊
```C++
size_t allocate(size_t siz);
```
- **Parameters**  
`siz` - 請求所需空間大小
- **Return Value**  
如果成功則回傳從 Buffer 起始處位移的單位大小數，否則回傳 dlou::none。

### buddy::allocate_by_pow2
分配 2^n^ 大小區塊
```C++
size_t allocate_by_pow2(uint8_t exp);
```
- **Parameters**  
`exp` - 所需 2^n^ 大小空間
- **Return Value**  
如果成功則回傳從 Buffer 起始處位移的單位大小數，否則回傳 dlou::none。

### buddy::release
釋放區塊
```C++
void release(size_t pos);
void release(size_t pos, size_t siz);
```
- **Parameters**  
`pos` - allocate的回傳值  
`siz` - allocate的參數值

### buddy::release_by_pow2
根據 allocate_by_pow2 來釋放區塊
```C++
void release_by_pow2(size_t pos, uint8_t exp);
```
- **Parameters**  
`pos` - allocate_by_pow2 的回傳值  
`exp` - allocate_by_pow2 的參數值

___
## simple_buddy
buddy的再封裝，更簡單的分配所需記憶體大小與位移位置
```C++
template<size_t MinSize, size_t MaxSize>
class simple_buddy;
```

### Template parameters
| Name | Description |
| --- | --- |
| MinSize | 請求的最小區塊大小 |
| MaxSize | 請求的最大區塊大小 |

### Member constants
| Name | Description |
| --- | --- |
| min_size | 可分配最小的區塊大小 |
| max_size | 可分配最大的區塊大小 |
| max_level | log~2~(max_size / min_size) |

### Member functions
| Name | Description |
| --- | --- |
| [empty](#simple_buddyempty) | 無分配任何區塊 |
| [full](#simple_buddyfull) | 無可用空間 |
| [allocate](#simple_buddyallocate) | 分配區塊 |
| [release](#simple_buddyrelease) | 釋放區塊 |

### Example
```C++
// Buddy memory allocator(256 ~ 4096)
char* block = new char[4096];
simple_buddy<256, 4096> head;

// allocate 1024 bytes
size_t offset = head.allocate(1024);
char* buf = (none == offset) ? nullptr : block + offset;

// deallocate
if (buf)
	head.release(offset);

delete[]block;
```

### simple_buddy::empty
無分配任何區塊
```C++
bool empty() const;
```
- **Return Value**  
`true` 為尚未使用，否則為 `false` 。

### simple_buddy::full
無可用空間
```C++
bool full() const;
```
- **Return Value**  
`true` 為無空餘空間，否則為 `false` 。

### simple_buddy::allocate
分配區塊
```C++
size_t allocate(size_t siz);
```
- **Parameters**  
`siz` - 請求所需空間大小
- **Return Value**  
如果成功則回傳從 Buffer 起始處位移的 Bytes 值，否則回傳 dlou::none。

### simple_buddy::release
釋放區塊
```C++
void release(size_t pos);
void release(size_t pos, size_t siz);
```
- **Parameters**  
`pos` - allocate 的回傳值  
`siz` - allocate 的參數值

