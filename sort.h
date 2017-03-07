#ifndef SORT_H_
#define SORT_H_

typedef n64 ElementType;

// 插入排序
void insertion_sort(n64 *arr, int n);

// 找枢轴
n64 median3(n64 *arr, int left, int right);

// 让数组arr中第k小的元素归位
void quick_sort(int k, n64 *arr, int left, int right);

#endif
