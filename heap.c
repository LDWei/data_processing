#include "common.h"

// 交换函数
void swap(FreqRec * h, n32 x, n32 y)
{
    FreqRec t;
    t = h[x];
    h[x] = h[y];
    h[y] = t;
}

// 传入一个需要向下调整的节点编号i，这里传入1，即从堆的顶点开始向下调整
n32 siftdown(n32 i, FreqRec * h, n32 n)
{
    // flag用来标记是否需要继续向下调整
    n32 t, flag = 0;
    n32 altered = 0;

    // 当i结点有儿子（其实是至少有左儿子的情况下）并且有需要继续调整的时候，循环就执行
    while (i*2 <= n && flag == 0) {
        // 首先判断它和左儿子的关系，并用t记录值较小的结点编号
        if (h[i].frequency > h[i*2].frequency) {
            t = i * 2;
        } else {
            t = i;
        }

        // 如果它有右儿子，再对右儿子进行讨论
        if (i*2+1 <= n) {
            // 如果右儿子的值更小，更新较小的结点编号
            if (h[t].frequency > h[i*2+1].frequency) {
                t = i*2+1;
            }
        }

        // 如果发现最小的结点编号不是自己，说明子结点中有比父结点更小的
        if (t != i) {
            swap(h, t, i);
            altered = 1;
            // 更新i为刚才与它交换的儿子结点的编号，便于接下来继续向下调整
            i = t;
        }
        else {
            // 否则说明当前的父结点已经比两个子结点都要小了，不需要再进行调整了
            flag = 1;
        }
    }

    return altered;
}

// 传入一个需要向上调整的结点编号i
n32 siftup(n32 i, FreqRec * h)
{
    // 用来标记是否需要继续向上调整
    n32 flag = 0;
    n32 altered = 0;

    if (i == 1) {
        // 如果是堆顶，就返回，不需要调整了
        return altered;
    }

    // 不在堆顶，并且当前结点i的值比父结点小的时候就继续向上调整
    while (i != 1 && flag == 0) {
        // 判断是否比父结点的小
        if (h[i].frequency < h[i/2].frequency) {
            // 交换它和它爸爸的位置
            swap(h, i, i/2);
            altered = 1;
        }
        else {
            // 表示已经不需要调整了，当前结点的值比父结点的值要小
            flag = 1;
        }
        // 这句话很重要，更新编号i为它父结点的编号，从而便于下一次继续向上调整
        i = i / 2;
    }

    return altered;
}

// 建立堆的函数
void create(FreqRec * h, n32 n)
{
    n32 i;
    // 从最后一个非叶结点到第1个结点依次进行向上调整
    for (i = n/2; i >= 1; i--) {
        siftdown(i, h, n);
    }
}

// 查找堆中是否有这个整数，如果有则返回它的索引，没有就返回0
n32 get_index(n64 integer, FreqRec * h, n32 n)
{
    n32 i;
    for (i = 1; i <= n; ++i) {
        if (h[i].integer == integer) {
            return i;
        }
    }
    return 0;
}
