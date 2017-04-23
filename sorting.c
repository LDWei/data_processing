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
        count_read = fread(buf, sizeof(n64), BUF_SIZE*2/3, fr);
        if (ferror(fr)) {
            perror(INPUT_FILENAME);
            fclose(fr);
            exit(EXIT_FAILURE);
        }
        if (count_read == 0) {
            break;
        }

        quick_select(count_read/2, buf, 0, count_read-1);

        curr = (Node *)malloc(sizeof(Node));

        curr->elem = (Part *)malloc(sizeof(Part));
        curr->elem->median = buf[count_read/2];
        curr->elem->flag = 0;

        strcpy(curr->elem->left.filename, "tmp_XXXXXX");
        if ((fw = mkstemp(curr->elem->left.filename)) == -1) {
            perror(curr->elem->left.filename);
            exit(EXIT_FAILURE);
        }
        if (write(fw, buf, sizeof(n64) * (count_read/2)) < 0) {
            perror(curr->elem->left.filename);
            exit(EXIT_FAILURE);
        }
        close(fw);

        strcpy(curr->elem->right.filename, "tmp_XXXXXX");
        if ((fw = mkstemp(curr->elem->right.filename)) == -1) {
            perror(curr->elem->right.filename);
            exit(EXIT_FAILURE);
        }
        if (write(fw, buf+count_read/2, sizeof(n64) * (count_read-count_read/2)) < 0) {
            perror(curr->elem->right.filename);
            exit(EXIT_FAILURE);
        }
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

Part* merge(Part *a, Part *b, n64 *buf)
{
    FILE *fp;
    // little left, little right, great left, great right
    size_t count_read_ll, count_read_lr, count_read_gl, count_read_gr;
    size_t count_read;
    Part *little, *great, *tmp;
    Part *ret = (Part *)malloc(sizeof(Part));
    if (a->median < b->median) {
        little = a;
        great = b;
    } else {
        little = b;
        great = a;
    }
    if (little->flag == 0 && great->flag == 0) {
        if ((fp = fopen(little->right.filename, "rb")) == NULL ) {
            perror(little->right.filename);
            exit(EXIT_FAILURE);
        }
        count_read_lr = fread(buf, sizeof(n64), BUF_SIZE/3, fp);
        if (ferror(fp)) {
            perror(little->right.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        if ((fp = fopen(great->left.filename, "rb")) == NULL ) {
            perror(great->left.filename);
            exit(EXIT_FAILURE);
        }
        count_read_gl = fread(buf+count_read_lr, sizeof(n64), BUF_SIZE/3, fp);
        if (ferror(fp)) {
            perror(great->left.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        count_read = count_read_lr + count_read_gl;
        quick_select(count_read/2, buf, 0, count_read-1);

        ret->median = buf[count_read/2];

        if ((fp = fopen(great->right.filename, "rb")) == NULL ) {
            perror(great->right.filename);
            exit(EXIT_FAILURE);
        }
        count_read_gr = fread(buf+count_read_lr+count_read_gl, sizeof(n64), BUF_SIZE/3, fp);
        if (ferror(fp)) {
            perror(great->right.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        count_read = count_read_gl + count_read_gr;
        quick_select(count_read/2, buf+count_read_lr, 0, count_read-1);

        if ((fp = fopen(great->left.filename, "wb")) == NULL ) {
            perror(great->left.filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read_lr, sizeof(n64), count_read/2, fp);
        if (ferror(fp)) {
            perror(great->left.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        if ((fp = fopen(great->right.filename, "wb")) == NULL ) {
            perror(great->right.filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read_lr+count_read/2, sizeof(n64), (count_read-count_read/2), fp);
        if (ferror(fp)) {
            perror(great->right.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        if ((fp = fopen(little->left.filename, "rb")) == NULL ) {
            perror(little->left.filename);
            exit(EXIT_FAILURE);
        }
        count_read_ll = fread(buf+count_read_lr, sizeof(n64), BUF_SIZE/3, fp);
        if (ferror(fp)) {
            perror(little->left.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        count_read = count_read_ll + count_read_lr;
        quick_select(count_read/2, buf, 0, count_read-1);

        if ((fp = fopen(little->left.filename, "wb")) == NULL ) {
            perror(little->left.filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf, sizeof(n64), count_read/2, fp);
        if (ferror(fp)) {
            perror(little->left.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);

        if ((fp = fopen(little->right.filename, "wb")) == NULL ) {
            perror(little->right.filename);
            exit(EXIT_FAILURE);
        }
        fwrite(buf+count_read/2, sizeof(n64), (count_read-count_read/2), fp);
        if (ferror(fp)) {
            perror(little->right.filename);
            fclose(fp);
            exit(EXIT_FAILURE);
        }
        fclose(fp);
    } else {
        tmp = merge(little->right.part, great->left.part, buf);
        little->right.part = tmp->left.part;
        great->left.part = tmp->right.part;
        ret->median = tmp->median;
        free(tmp);
    }
    ret->flag = little->flag + 1;
    ret->left.part = little;
    ret->right.part = great;
    return ret;
}

Node* find_tail(List head)
{
    if (head != NULL) {
        while (head->next != NULL) {
            head = head->next;
        }
    }
    return head;
}

int main(void)
{
    int count = 0;
    List head;
    Node *tail, *brother, *new;
    n64 *buf = (n64 *)malloc(BUF_SIZE * sizeof(n64));
    system("rm -f tmp_*");
    head = carve(buf);
    tail = find_tail(head);
    while (head != tail || count % 2) {
        if (count % 2) {
            new = (Node *)malloc(sizeof(Node));
            new->elem = merge(brother->elem, head->elem, buf);
            free(brother);
            new->next = NULL;
            tail->next = new;
            tail = tail->next;
        }
        brother = head;
        head = head->next;
        count++;
    }
    printf("%lld\n", head->elem->median);
    free(buf);
    return 0;
}
