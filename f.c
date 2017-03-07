#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "sort.h"
List map(n64 * buf, n32 level)
{
    FILE *fr, *fw;
    n64 count;
    size_t count_read;
    n32 group;
    char fname[10], tname[10];
    List m;
    Node *curr, *prev;

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

        quick_sort(count_read/2, buf, 0, count_read);

        curr = (Node *)malloc(sizeof(Node));
        curr->integer = buf[count_read/2];
        curr->next = NULL;
        if (group == 0) {
            m = curr;
        } else {
            prev->next = curr;
        }
        prev = curr;

        sprintf(tname, "%d", level);
        strcpy(fname, tname);
        sprintf(tname, "%03d", group);
        strcat(fname, tname);
        strcat(fname, "0");
        if ((fw = fopen(fname, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", fname);
            exit(EXIT_FAILURE);
        }
        fwrite(buf, 8, count_read/2, fw);
        fclose(fw);

        sprintf(tname, "%d", level);
        strcpy(fname, tname);
        sprintf(tname, "%03d", group);
        strcat(fname, tname);
        strcat(fname, "1");
        if ((fw = fopen(fname, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", fname);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read/2+1, 8, count_read-count_read/2-1, fw);
        fclose(fw);

        count += (n64)count_read;
        if (feof(fr)) {
            break;
        }
        ++group;
    }

    return m;
}

void reduce(n64 * buf, n32 level, n32 group)
{
    n32 i;
    n64 m[50];
    if (level == 0) {
        for (i = 0; i < group/2; ++i) {
            if (m[2*i] < m[2*i+1]) {
            } else if (m[2*i] > m[2*i+1]) {
            } else {
            }
        }
    } else {
        map(buf, level);
    }
}
