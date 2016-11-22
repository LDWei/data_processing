#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 134217728      // 64位整数的个数，这里刚好占用1G内存
/*#define BUF_SIZE 536870912      // 4G内存*/
typedef int n32;
typedef unsigned int u32;
typedef long long n64;
typedef unsigned long long u64;
typedef struct s_u128 {
    u64 low;
    u64 high;
}u128;

// 把64位有符号数扩展成128位有符号数
void extend(u128 * integer);

// sum += elem
void add_to(u128 * elem, u128 * sum);

// 把integer逻辑左移times位
void shl(u128 * integer, u32 times);

// 把integer逻辑右移times位
void shr(u128 * integer, u32 times);

// 把integer算术右移times位
void sar(u128 * integer, u32 times);

// 返回integer的高bits位
u64 get_top_bits(u128 * integer, u32 bits);

// 负数先取相反数（补码）进行运算，最后再确定商和余数的符号
void divide(u128 * quotient, u64 * remainder, u128 dividend, u64 divisor);

#endif
