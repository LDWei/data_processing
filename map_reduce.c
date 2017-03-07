#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "sort.h"

void reduce(n64 * m, int group, int level)
{
    FILE *fr, *fw;
    n64 count;
    size_t count_read;
    char fname[10], tmp[10];

    if ((fr = fopen("test.data", "rb")) == NULL ) {
        fprintf(stderr, "Error in open file \"test.data\" to read.\n");
        exit(EXIT_FAILURE);
    }

    count = 0ULL;
    group = 0;
    while (1) {
        // 因为内存限制，每次只读取BUF_SIZE个64位整数
        count_read = fread(buf, 8, BUF_SIZE, fr);
        if (ferror(fr)) {
            perror("ferror() was called.");
            fclose(fr);
            exit(EXIT_FAILURE);
        }

        quick_select(count_read/2, buf, 0, count_read);
        sprintf(tmp, "%d", level);
        strcpy(fname, tmp);
        sprintf(tmp, "%03d", group);
        strcat(fname, tmp);
        strcat(fname, "0");
        if ((fw = fopen(fname, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", fname);
            exit(EXIT_FAILURE);
        }
        fwrite(buf, 8, count_read/2+1, fw);
        fclose(fw);

        sprintf(tmp, "%d", level);
        strcpy(fname, tmp);
        sprintf(tmp, "%03d", group);
        strcat(fname, tmp);
        strcat(fname, "1");
        if ((fw = fopen(fname, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", fname);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read/2, 8, count_read-count_read/2, fw);
        fclose(fw);

        count += (n64)count_read;
        if (feof(fr)) {
            break;
        }
        ++group;
    }
}

int main(void)
{
    n64 * buf;
    u32 i, group, level;

    // 不能静态分配大数组，gcc会产生段错误，只能动态分配
    buf = malloc(BUF_SIZE * sizeof(n64));

    return 0;
}
