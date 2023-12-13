#ifndef LIB_H
#define LIB_H

typedef struct {
    int overflow;
    int result;
} AddResult;

enum {
    MAX_DIGIT = 9,
    MAX_DIGIT_VALUE = 1000000000u,
};

typedef struct {
    unsigned int* digits; // 这是一个栈哈
    int top; // 即将放置的下标。最后一个有效存储下标为 top - 1
    int space;
} bignum;

bignum new_bignum();

void append_one_digit(bignum* a, unsigned int digit);

bignum add_bignum(bignum* a, bignum* b);

void print_bignum(bignum* a);

bignum rand_big_num(int digits);

void free_bignum_member(bignum* a);

#endif
