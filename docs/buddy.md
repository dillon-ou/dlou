# <buddy.hpp>
Buddy memory allocation ��@

## Classes
| Name | Description |
| --- | --- |
| [buddy](#buddy) | Buddy memory allocation |
| [simple_buddy](#simple_buddy) | buddy���A�ʸˡA��²�檺���t�һݰO����j�p�P�첾��m |

___
### buddy
Buddy memory allocation  

```C++
template<uint8_t Level>
class buddy;
```

�o�̰ѦҤF [wuwenbin](https://github.com/wuwenbin) �� [buddy2](https://github.com/wuwenbin/buddy2) ��k�A  
�إߤ@�� full binary tree ���}�C�A  
child �N�� parent �����ΪŶ��A  
node ���� subtree �i���t���̤j�Ŷ��A  
allocate �ɥ� root �� leaves ���ݭn�j�p�����t�Ŷ��A  
release �ɥ� leaf �� root ��w�g���t���Ŷ��C

#### Template parameters
| Name | Description |
| --- | --- |
| Level | �϶��h�ż� |

#### Member constants
| Name | Description |
| --- | --- |
| max_level | �϶��̤j�h�� |
| max_size | 2^max_level^ ���i���t�̤j���϶����� |

#### Member functions
| Name | Description |
| --- | --- |
| [empty](#buddyempty) | �L���t����϶� |
| [full](#buddyfull) | �L�i�ΪŶ� |
| [allocate](#buddyallocate) | ���t�϶� |
| [allocate_by_pow2](#buddyallocate_by_pow2) | ���t 2^n^ �j�p�϶� |
| [release](#buddyrelease) | ����϶� |
| [release_by_pow2](#buddyrelease_by_pow2) | �ھ� allocate_by_pow2 ������϶� |

#### Example
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

#### Methods
##### buddy::empty
�L���t����϶�
```C++
bool empty() const;
```
- **Return Value**  
`true` ���|���ϥΡA�_�h�� `false` �C

##### buddy::full
�L�i�ΪŶ�
```C++
bool full() const;
```
- **Return Value**  
`true` ���L�žl�Ŷ��A�_�h�� `false` �C

##### buddy::allocate
���t�϶�
```C++
size_t allocate(size_t siz);
```
- **Parameters**  
`siz` - �ШD�һݪŶ��j�p
- **Return Value**  
�p�G���\�h�^�Ǳq Buffer �_�l�B�첾�����j�p�ơA�_�h�^�� dlou::none�C

##### buddy::allocate_by_pow2
���t 2^n^ �j�p�϶�
```C++
size_t allocate_by_pow2(uint8_t exp);
```
- **Parameters**  
`exp` - �һ� 2^n^ �j�p�Ŷ�
- **Return Value**  
�p�G���\�h�^�Ǳq Buffer �_�l�B�첾�����j�p�ơA�_�h�^�� dlou::none�C

##### buddy::release
����϶�
```C++
void release(size_t pos);
void release(size_t pos, size_t siz);
```
- **Parameters**  
`pos` - allocate���^�ǭ�  
`siz` - allocate���Ѽƭ�

##### buddy::release_by_pow2
�ھ� allocate_by_pow2 ������϶�
```C++
void release_by_pow2(size_t pos, uint8_t exp);
```
- **Parameters**  
`pos` - allocate_by_pow2 ���^�ǭ�  
`exp` - allocate_by_pow2 ���Ѽƭ�

___
### simple_buddy
buddy���A�ʸˡA��²�檺���t�һݰO����j�p�P�첾��m
```C++
template<size_t MinSize, size_t MaxSize>
class simple_buddy;
```

#### Template parameters
| Name | Description |
| --- | --- |
| MinSize | �ШD���̤p�϶��j�p |
| MaxSize | �ШD���̤j�϶��j�p |

#### Member constants
| Name | Description |
| --- | --- |
| min_size | �i���t�̤p���϶��j�p |
| max_size | �i���t�̤j���϶��j�p |
| max_level | log~2~(max_size / min_size) |

#### Member functions
| Name | Description |
| --- | --- |
| [empty](#simple_buddyempty) | �L���t����϶� |
| [full](#simple_buddyfull) | �L�i�ΪŶ� |
| [allocate](#simple_buddyallocate) | ���t�϶� |
| [release](#simple_buddyrelease) | ����϶� |

#### Example
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

#### Methods
##### simple_buddy::empty
�L���t����϶�
```C++
bool empty() const;
```
- **Return Value**  
`true` ���|���ϥΡA�_�h�� `false` �C

##### simple_buddy::full
�L�i�ΪŶ�
```C++
bool full() const;
```
- **Return Value**  
`true` ���L�žl�Ŷ��A�_�h�� `false` �C

##### simple_buddy::allocate
���t�϶�
```C++
size_t allocate(size_t siz);
```
- **Parameters**  
`siz` - �ШD�һݪŶ��j�p
- **Return Value**  
�p�G���\�h�^�Ǳq Buffer �_�l�B�첾�� Bytes �ȡA�_�h�^�� dlou::none�C

##### simple_buddy::release
����϶�
```C++
void release(size_t pos);
void release(size_t pos, size_t siz);
```
- **Parameters**  
`pos` - allocate ���^�ǭ�  
`siz` - allocate ���Ѽƭ�

