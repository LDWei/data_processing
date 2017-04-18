#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "common.h"
#include "sort.h"
Section carve(n64 * buf)
{
    FILE *fr, *fw;
    size_t count_read;
    Section section_list;
    NodeSection *curr, *prev;
    NodeFile *file_list;

    if ((fr = fopen("test.data", "rb")) == NULL ) {
        fprintf(stderr, "Error in open file \"test.data\" to read.\n");
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
            perror("ferror() was called.");
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
        mkstemp(curr->left_file_list->filename);
        if ((fw = fopen(curr->left_file_list->filename, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", curr->left_file_list->filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf, 8, count_read/2, fw);
        fclose(fw);

        file_list = (NodeFile *)malloc(sizeof(NodeFile));
        strcpy(file_list->filename, "tmp_XXXXXX");
        curr->right_file_list = file_list;
        mkstemp(curr->right_file_list->filename);
        if ((fw = fopen(curr->right_file_list->filename, "wb")) == NULL) {
            fprintf(stderr, "Error in open file \"%s\" to write.\n", curr->right_file_list->filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read/2, 8, count_read-count_read/2, fw);
        fclose(fw);
    }

    return section_list;
}
int main(void)
{
    n64 *buf = (n64 *)malloc(BUF_SIZE * sizeof(n64));
    carve(buf);
    return 0;
}
