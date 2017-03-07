#ifndef TYPE_H_
#define TYPE_H_

typedef int n32;
typedef unsigned int u32;
typedef long long n64;
typedef unsigned long long u64;
typedef struct s_n128 {
    u64 low;
    u64 high;
}u128;
typedef u128 n128;
typedef struct s_node {
    n64 integer;
    struct s_node * next;
}Node;
typedef Node * List;

#endif
