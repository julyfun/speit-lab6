## 文件结构

```
.
├── CMakeLists.txt  # CMake 文件
├── README.md
├── include
│   └── lib.h       # lib.c 源文件提供的接口
└── src
    ├── lib.c       # 栈型 bignum 主要的实现函数
    └── main.c      # 用于测试 bignum 实现的功能
```

## 栈型 `bignum` 实现方法

### 存储结构

结构体定义如下：

```c
typedef struct {
    unsigned int* digits; // 这是一个栈哈
    int top; // 即将放置的下标。最后一个有效存储下标为 top - 1
    int space;
} bignum;

void append_one_digit(bignum* a, unsigned int digit) {
    if (a->top == a->space) {
        a->space *= 2;
        a->digits = (unsigned int*)realloc(a->digits, sizeof(unsigned int) * a->space);
        // 不保证新空间清零
    }
    a->digits[a->top] = digit;
    a->top++;
}
```

采用动态空间存储数位，
- `digits` 指向一个动态的 `unsigned int` 数组空间，每一位最多存储 `10 ^ 9 - 1` 即 $9$ 位数。
- `top` 存储栈顶（栈顶不存数据，实际存储数据的下表为 `0` 到 `top - 1`），
- `space` 记录 `digits` 上分配的空间。当需要向栈顶扩展数位时，检查空间是否足够，如果不够则将当前空间翻倍。

这里扩展新数位时重新分配动态空间使用 `realloc()` 函数，空间不够时翻倍，提升运行效率（如果每次添加 $1$ 位就 `realloc()`，反复调用可能会造成额外开销）。

### 加法

首先定义对单个数位的加法：

```c
typedef struct {
    int overflow;
    int result;
} AddResult;

enum {
    MAX_DIGIT = 9,
    MAX_DIGIT_VALUE = 1000000000u,
};

static AddResult add(unsigned int a, unsigned int b) {
    assert(a < MAX_DIGIT_VALUE && b < MAX_DIGIT_VALUE);
    AddResult result;
    result.result = a + b;
    result.overflow = result.result >= MAX_DIGIT_VALUE ? result.result -= MAX_DIGIT_VALUE, 1 : 0;
    return result;
}
```

对两个小于 $10 ^ 9$ 的无符号整型进行加法，用 `AddResult` 结构体存储加法结果是否进位，若其和超过 $10 ^ 9$ 在返回值中保留减去 $10 ^ 9$ 的部分（相比取模更快），并将进位 `overflow` 设置为 $1$。

然后定义对于 `bignum` 的加法：

```c
static void refresh_big_num(bignum* a) {
    int i = a->top - 1;
    while (i >= 0 && a->digits[i] == 0) {
        i--;
    }
    a->top = i + 1; // 前缀 0 退栈，并更新栈顶，至少保留一位有效数据
}

bignum add_bignum(bignum* a, bignum* b) {
    bignum result = new_bignum();
    int i = 0;
    int carry = 0;
    while (i < a->top || i < b->top) {
        unsigned int a_digit = i < a->top ? a->digits[i] : 0;
        unsigned int b_digit = i < b->top ? b->digits[i] : 0;
        AddResult add_result = add(a_digit, b_digit);
        add_result.result += carry;
        carry = add_result.overflow;
        append_one_digit(&result, add_result.result);
        i++;
    }
    // 最高位计算仍有进位
    if (carry) {
        append_one_digit(&result, 1);
    }
    refresh_big_num(&result);
    return result;
}
```
 
对 `a` 和 `b` 两个大整数的每一位分别调用 `add()` 函数，将进位结果记录在 `carry` 并加到下一位中。若最高位相加仍有进位，则结果还需要在栈顶再扩展一位。

这里的 `refresh_big_num()` 函数检测 `result` 是否有前导 $0$ 并将其退栈，提升扩展性。

### 输出

```c
void print_bignum(bignum* a) {
    if (a->top == 0) {
        printf("0\n"); // 对 0 进行特判，因为此时栈中无元素
        return;
    }
    int i = a->top - 1;
    while (i >= 0) {
        if (i == a->top - 1) {
            printf("%u", a->digits[i]);
        } else {
            printf("%09u", a->digits[i]);
        }
        i--;
    }
    printf("\n");
}
```

从栈顶向下遍历，对于栈顶以外的数需要使用 `%09d` 保留前导零并采取 $9$ 位输出。代码中在存储数字为 $0$ 时使用空栈，所以需要特判 $0$ 的情况。

## 测试部分 `main.c`

```c
    srand(time(0)); // 用于产生随机大数
    for (int i = 0; i < 5; i++) {
        bignum a = rand_big_num(rand() % 10 + 1);
        bignum b = rand_big_num(rand() % 10 + 1);
        bignum c = add_bignum(&a, &b);
        printf("[test #%d]\n", i + 1);
        printf("a = ");
        print_bignum(&a);
        printf("b = ");
        print_bignum(&b);
        printf("a + b == ");
        print_bignum(&c);

        free_bignum_member(&a);
        free_bignum_member(&b);
        free_bignum_member(&c);
    }
    {
        printf("[test #%d]\n", 6);
        bignum a = new_bignum();
        bignum b = new_bignum();
        bignum c = add_bignum(&a, &b);
        printf("0 + 0 == ");
        print_bignum(&c);
    }
    {
        printf("[test #%d]\n", 7);
        bignum a = new_bignum();
        append_one_digit(&a, 1);
        bignum b = new_bignum();
        append_one_digit(&b, MAX_DIGIT_VALUE - 1);
        bignum c = add_bignum(&a, &b);
        printf("1 + %d == ", MAX_DIGIT_VALUE - 1);
        print_bignum(&c);
    }
```

前 $5$ 组测试，每组生成两个随机大数相加。后两组测试了输出 $0$ 和一定会进位的情况。形如：

```
[test #5]
a = 400732753058220134
b = 396987591661134313683510334846805775361963497046436607300005388234737161
a + b == 396987591661134313683510334846805775361963497046436607700738141292957295
[test #6]
0 + 0 == 0
[test #7]
1 + 999999999 == 1000000000
```

可以复制到 python 中检验~

```
Python 3.11.2 (main, Feb 16 2023, 02:51:42) [Clang 14.0.0 (clang-1400.0.29.202)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
>>> a = 188188473120433021
>>> b = 614649729355177706664313837509168422542231349612449060914206281082177678364996876
>>> a + b == 614649729355177706664313837509168422542231349612449060914206281270366151485429897
True
>>> 1 + 999999999 == 1000000000
True
>>>
```
