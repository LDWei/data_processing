#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "type.h"
#include "common.h"
#include "sort.h"
List carve(n64 * buf)
{
    FILE *fr;
    int fw;
    size_t count_read;
    List part_list;
    Node *curr, *prev;

    if ((fr = fopen(INPUT_FILENAME, "rb")) == NULL ) {
        perror(INPUT_FILENAME);
        exit(EXIT_FAILURE);
    }

    part_list = NULL;
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

        curr = (Node *)malloc(sizeof(Node));
        curr->elem.median = buf[count_read/2];
        curr->elem.flag = 0;

        strcpy(curr->elem.left.filename, "tmp_XXXXXX");
        if ((fw = mkstemp(curr->elem.left.filename)) == -1) {
            perror(curr->elem.left.filename);
            exit(EXIT_FAILURE);
        }
        write(fw, buf, sizeof(n64) * (count_read/2));
        close(fw);

        strcpy(curr->elem.right.filename, "tmp_XXXXXX");
        if ((fw = mkstemp(curr->elem.right.filename)) == -1) {
            perror(curr->elem.right.filename);
            exit(EXIT_FAILURE);
        }
        write(fw, buf+count_read/2, sizeof(n64) * (count_read-count_read/2));
        close(fw);

        curr->next = NULL;
        if (part_list == NULL) {
            part_list = curr;
        } else {
            prev->next = curr;
        }
        prev = curr;
    }
    fclose(fr);

    return part_list;
}

Part* merge(Part *a, Part *b)
{

}

int main(void)
{
    n64 *buf = (n64 *)malloc(BUF_SIZE * sizeof(n64));
    carve(buf);
    free(buf);
    return 0;
}
