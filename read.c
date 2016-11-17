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
void extend(u128 * integer)
{
    u64 mask;

    // 常数默认为int类型，左移63结果是错的，必须加ULL
    mask = 1ULL << 63;
    if (integer->high == 0ULL && integer->low & mask) {
        integer->high = ~0ULL;
    }
}

// sum += elem
void add_to(u128 * elem, u128 * sum)
{
    sum->low += elem->low;
    if (sum->low < elem->low) {
        sum->high += 1ULL;
    }
    sum->high += elem->high;
}

// 把integer逻辑左移times位
void shl(u128 * integer, u32 times)
{
    u32 i, loop;
    u64 mask, mask_clr, temp;

    times %= 128;
    if (times <= 63) {
        mask_clr = 1ULL << times;
        mask_clr -= 1ULL;
        mask = mask_clr << (64 - times);

        integer->high <<= times;
        temp = integer->low & mask;
        temp >>= 64 - times;
        // 不能确定是算术右移还是逻辑右移，必须把高位清零
        temp &= mask_clr;
        integer->high |= temp;

        integer->low <<= times;
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            shl(integer, 63);
        }
        if (times % 63) {
            shl(integer, times % 63);
        }
    }
}

// 把integer逻辑右移times位
void shr(u128 * integer, u32 times)
{
    u32 i, loop;
    u64 mask, mask_clr, temp;

    times %= 128;
    if (times <= 63) {
        mask = 1ULL << times;
        mask -= 1ULL;
        mask_clr = ~(mask << 64-times);

        integer->low >>= times;
        integer->low &= mask_clr;
        temp = integer->high & mask;
        temp <<= 64 - times;
        integer->low |= temp;

        integer->high >>= times;
        integer->high &= mask_clr;
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            shr(integer, 63);
        }
        if (times % 63) {
            shr(integer, times % 63);
        }
    }
}

// 把integer算术右移times位
void sar(u128 * integer, u32 times)
{
    u32 i, loop;
    u64 mask, mask_clr, temp;

    times %= 128;
    if (times <= 63) {
        mask = 1ULL << times;
        mask -= 1ULL;
        mask_clr = ~(mask << 64-times);

        integer->low >>= times;
        integer->low &= mask_clr;
        temp = integer->high & mask;
        temp <<= 64 - times;
        integer->low |= temp;

        // 分别处理符号位为0和1的两种情况
        if (integer->high & 1ULL<<63) {
            integer->high >>= times;
            integer->high &= mask_clr;
            temp = mask;
            temp <<= 64 - times;
            integer->high |= temp;
        } else {
            integer->high >>= times;
            integer->high &= mask_clr;
        }
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            sar(integer, 63);
        }
        if (times % 63) {
            sar(integer, times % 63);
        }
    }
}

// 返回integer的高bits位
u64 get_top_bits(u128 * integer, u32 bits)
{
    u64 mask, mask_clr, top;

    mask_clr = 1ULL << bits;
    mask_clr -= 1ULL;
    mask = mask_clr << 64-bits;

    top = integer->high & mask;
    top >>= 64-bits;
    top &= mask_clr;

    return top;
}

// 负数先取相反数（补码）进行运算，最后再确定商和余数的符号
void divide(u128 * quotient, u64 * remainder, u128 dividend, u64 divisor)
{
    u32 i, flag_quotient, flag_remainder, bits, times;
    u64 mask;

    mask = 1ULL << 63;
    // 无法计算有符号数的最小值，因为它的相反数（补码）是它本身
    if (dividend.high == mask && dividend.low == 0ULL || divisor == mask) {
        perror("Can't calculate!\n");
        exit(EXIT_FAILURE);
    }
    if (divisor == 0ULL) {
        perror("Division by zero!\n");
        exit(EXIT_FAILURE);
    }

    flag_quotient = 0;
    // 被除数和除数异号，则商为负
    if (dividend.high & mask ^ divisor & mask) {
        flag_quotient = 1;
    }

    // 如果除数为负，则取相反数（补码）
    if (divisor & mask) {
        divisor = ~divisor;
        divisor += 1ULL;
    }

    flag_remainder = 0;
    // 如果被除数为负，则取相反数（补码），另外余数和被除数同号
    if (dividend.high & mask) {
        dividend.high = ~dividend.high;
        dividend.low = ~dividend.low;
        dividend.low += 1ULL;
        if (dividend.low < 1ULL) {
            dividend.high += 1ULL;
        }
        flag_remainder = 1;
    }

    times = 0;
    // times是除数高位连续0的个数
    while (!(divisor<<times & mask)) {
        ++times;
    }
    // bits是除数真正占用的二进制位，例如5真正占用的只有3个二进制位
    bits = 64 - times;

    // 用做十进制除法的方法来做二进制除法
    quotient->low = 0ULL;
    quotient->high = 0ULL;
    *remainder = get_top_bits(&dividend, bits);
    shl(&dividend, bits);
    if (*remainder >= divisor) {
        quotient->low = 1ULL;
        *remainder -= divisor;
    }
    for (i = 0; i < 128-bits; ++i) {
        shl(quotient, 1);
        *remainder <<= 1;
        *remainder += get_top_bits(&dividend, 1);
        shl(&dividend, 1);
        if (*remainder >= divisor) {
            quotient->low |= 1ULL;
            *remainder -= divisor;
        }
    }

    // 对余数取相反数（补码）
    if (flag_remainder) {
        *remainder = ~*remainder;
        *remainder += 1ULL;
    }

    // 对商取相反数（补码）
    if (flag_quotient) {
        quotient->high = ~quotient->high;
        quotient->low = ~quotient->low;
        quotient->low += 1ULL;
        if (quotient->low < 1ULL) {
            quotient->high += 1ULL;
        }
    }
}

int main(void)
{
    FILE *fr;
    n64 * buf;
    u128 sum, elem, average;
    n64 remainder, count;
    size_t count_read;

    if ((fr = fopen("test.data", "rb")) == NULL ) {
        exit(EXIT_FAILURE);
    }

    // 不能静态分配大数组，gcc会产生段错误，只能动态分配
    buf = malloc(BUF_SIZE * sizeof(n64));
    count = 0ULL;
    sum.low = 0ULL;
    sum.high = 0ULL;
    while (1) {
        // 因为内存限制，每次只读取BUF_SIZE个64位整数
        count_read = fread(buf, 8, BUF_SIZE, fr);
        if (ferror(fr)) {
            perror("ferror() was called.");
            fclose(fr);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < count_read; ++i) {
            elem.low = (u64)buf[i];
            elem.high = 0ULL;
            extend(&elem);
            add_to(&elem, &sum);
        }
        count += (n64)count_read;
        if (feof(fr)) {
            break;
        }
    }

    average.high = 0ULL;
    average.low = 0ULL;
    divide(&average, &remainder, sum, (u64)count);
    printf("%lld\t%lld\t%lld\n", average.high, average.low, remainder);
    printf("%lld\t%#llx\t%llu\t%#llx\t%d\n", sum.high, sum.high, sum.low, sum.low, count);
    return 0;
}
