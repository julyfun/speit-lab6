#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib.h"

int main() {
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

    return 0;
}
