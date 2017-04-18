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
typedef struct s_node_file {
    char filename[11];
    struct s_node_file *next;
}NodeFile;
typedef struct s_node_section {
    NodeFile *left_file_list;
    n64 median;
    NodeFile *right_file_list;
    struct s_node_section *next;
}NodeSection;
typedef NodeSection *Section;

#endif
