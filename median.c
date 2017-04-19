#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "type.h"
#include "common.h"
#include "sort.h"
Section carve(n64 * buf)
{
    FILE *fr;
    int fw;
    size_t count_read;
    Section section_list;
    NodeSection *curr, *prev;
    NodeFile *file_list;

    if ((fr = fopen(INPUT_FILENAME, "rb")) == NULL ) {
        perror(INPUT_FILENAME);
        exit(EXIT_FAILURE);
    }

    section_list = NULL;
    while (1) {
        if (feof(fr)) {
            break;
        }
        // 因为内存限制，每次只读取BUF_SIZE个64位整数
        count_read = fread(buf, 8, BUF_SIZE, fr);
        if (ferror(fr)) {
            perror(INPUT_FILENAME);
            fclose(fr);
            exit(EXIT_FAILURE);
        }
        if (count_read == 0) {
            break;
        }

        quick_select(count_read/2, buf, 0, count_read);

        curr = (NodeSection *)malloc(sizeof(NodeSection));
        curr->median = buf[count_read/2];
        curr->next = NULL;
        if (section_list == NULL) {
            section_list = curr;
        } else {
            prev->next = curr;
        }
        prev = curr;

        file_list = (NodeFile *)malloc(sizeof(NodeFile));
        strcpy(file_list->filename, "tmp_XXXXXX");
        curr->left_file_list = file_list;
        if ((fw = mkstemp(curr->left_file_list->filename)) == -1) {
            perror(curr->left_file_list->filename);
            exit(EXIT_FAILURE);
        }
        write(fw, buf, sizeof(n64) * (count_read/2));
        close(fw);

        file_list = (NodeFile *)malloc(sizeof(NodeFile));
        strcpy(file_list->filename, "tmp_XXXXXX");
        curr->right_file_list = file_list;
        if ((fw = mkstemp(curr->right_file_list->filename)) == -1) {
            perror(curr->right_file_list->filename);
            exit(EXIT_FAILURE);
        }
        write(fw, buf+count_read/2, sizeof(n64) * (count_read-count_read/2));
        close(fw);
    }
    fclose(fr);

    return section_list;
}
int main(void)
{
    n64 *buf = (n64 *)malloc(BUF_SIZE * sizeof(n64));
    carve(buf);
    free(buf);
    return 0;
}
