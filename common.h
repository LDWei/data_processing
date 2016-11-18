#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 134217728      // 64位整数的个数，这里刚好占用1G内存
#define TOP_FREQUENCY 1000000

/*#define BUF_SIZE 536870912      // 4G内存*/
typedef int n32;
typedef unsigned int u32;
typedef long long n64;
typedef unsigned long long u64;

typedef struct s_u128 {
    u64 low;
    u64 high;
}u128;

typedef struct s_frequency_record {
    u64 integer;
    n64 frequency;
}FreqRec;

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

// 交换函数
void swap(FreqRec * h, n32 x, n32 y);

// 传入一个需要向下调整的节点编号i，这里传入1，即从堆的顶点开始向下调整
n32 siftdown(n32 i, FreqRec * h, n32 n);

// 传入一个需要向上调整的结点编号i
n32 siftup(n32 i, FreqRec * h);

// 建立堆的函数
void create(FreqRec * h, n32 n);

// 查找堆中是否有这个整数，如果有则返回它的索引，没有就返回0
n32 get_index(n64 integer, FreqRec * h, n32 n);

#endif
