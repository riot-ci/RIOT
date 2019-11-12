/*
 * Copyright (C) 2013 Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Simple malloc/free test
 *
 *
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CHUNK_SIZE
#define CHUNK_SIZE          (16U)
#endif

#ifndef NUMBER_OF_TESTS
#define NUMBER_OF_TESTS     (3U)
#endif

#ifndef MAX_NUMBER_BLOCKS
#define MAX_NUMBER_BLOCKS   (1024U)
#endif

struct node {
    struct node *next;
    void *ptr;
};

static int total = 0;

static void fill_memory(struct node *head)
{
    unsigned aux = 0;
    if (head) {
        head->next = NULL;
    }

    while ((aux < MAX_NUMBER_BLOCKS) && head && (head->ptr = malloc(CHUNK_SIZE))) {
        printf("\tAllocated %d Bytes at %p, total %d\n",
               CHUNK_SIZE, head->ptr, total += CHUNK_SIZE);
        memset(head->ptr, '@', CHUNK_SIZE);
        head = head->next = malloc(sizeof(struct node));
        if (head) {
            total += sizeof(struct node);
            head->ptr  = 0;
            head->next = 0;
        }
        aux++;
    }
}

static void free_memory(struct node *head)
{
    struct node *old_head;

    while (head) {
        if (head->ptr) {
            printf("\tFree %d Bytes at %p, total %d\n",
                   CHUNK_SIZE, head->ptr, total -= CHUNK_SIZE);
            free(head->ptr);
        }

        if (head->next) {
            old_head = head;
            head = head->next;
            free(old_head);
        }
        else {
            free(head);
            head = 0;
        }

        total -= sizeof(struct node);
    }
}

int main(void)
{
    printf("CHUNK_SIZE: %d\n", CHUNK_SIZE);
    printf("NUMBER_OF_TESTS: %d\n", NUMBER_OF_TESTS);
    printf("MAX_NUMBER_BLOCKS: %d\n", MAX_NUMBER_BLOCKS);

    unsigned count = 0;
    while (count < NUMBER_OF_TESTS) {
        printf("TEST #%d:\n", count + 1);
        struct node *head = malloc(sizeof(struct node));
        total += sizeof(struct node);

        fill_memory(head);
        free_memory(head);
        count++;
    }

    puts("[SUCCESS]");

    return 0;
}
