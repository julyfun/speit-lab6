#include "lib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static AddResult add(unsigned int a, unsigned int b) {
    assert(a < MAX_DIGIT_VALUE && b < MAX_DIGIT_VALUE);
    AddResult result;
    // 自然溢出法，但是很难输出
    // result.result = a + b;
    // result.overflow = result.result < a || result.result < b;
    result.result = a + b;
    result.overflow = result.result >= MAX_DIGIT_VALUE ? result.result -= MAX_DIGIT_VALUE, 1 : 0;
    return result;
}

bignum new_bignum() {
    bignum a;
    a.top = 0;
    a.space = 5;
    a.digits = (unsigned int*)malloc(sizeof(unsigned int) * a.space);
    return a;
}

// 将元素扩展到栈顶
// 如果空间不够，则将空间扩展为原来的两倍
void append_one_digit(bignum* a, unsigned int digit) {
    if (a->top == a->space) {
        a->space *= 2;
        a->digits = (unsigned int*)realloc(a->digits, sizeof(unsigned int) * a->space);
        // 不保证新空间清零
    }
    a->digits[a->top] = digit;
    a->top++;
}

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

bignum rand_big_num(int digits) {
    bignum a = new_bignum();
    for (int i = 0; i < digits; i++) {
        append_one_digit(&a, rand() % MAX_DIGIT_VALUE);
    }
    refresh_big_num(&a);
    return a;
}

void free_bignum_member(bignum* a) {
    free(a->digits);
}
