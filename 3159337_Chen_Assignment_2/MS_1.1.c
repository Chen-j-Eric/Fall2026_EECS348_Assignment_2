/*
  name: EECS 348 Assignment 2
  purpose: Puts email into a Maxheap as a means of implementing a priority queue where enails can 
           shift towards the frount based on priority
  outputs: Next email, shows sender subject and dates
  collaborators: Meta Spark Thinking 1.1, Eric Chen
  author: Eric Chen
  creation date: 09/17/2026

  
    prologue comments
    adding comments to all lines
    Worked first time with the extensive prompt
    Resulting outputs are expected and does not crash

    handles error, ints, floats and random letters but does not read non cap letters
*/
    

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 16
#define MAX_LINE 1024

typedef struct {
    char sender[32];
    char subject[256];
    char dateStr[16];
    int dateVal;
    int catPriority;
    long seq;
} Email;

typedef struct {
    Email **arr;
    int size;
    int capacity;
} MaxHeap;

void trim(char *s) {
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start)+1);
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) {
        s[len-1] = '\0';
        len--;
    }
}

int getCatPriority(const char *cat) {
    if (strcmp(cat, "Boss") == 0) return 5;
    if (strcmp(cat, "Subordinate") == 0) return 4;
    if (strcmp(cat, "Peer") == 0) return 3;
    if (strcmp(cat, "ImportantPerson") == 0) return 2;
    if (strcmp(cat, "OtherPerson") == 0) return 1;
    return 0;
}

int parseDateVal(const char *dateStr) {
    int mm, dd, yyyy;
    if (sscanf(dateStr, "%d-%d-%d", &mm, &dd, &yyyy) != 3) return 0;
    return yyyy * 10000 + mm * 100 + dd;
}

int compareEmail(Email *a, Email *b) {
    if (a->catPriority != b->catPriority)
        return a->catPriority - b->catPriority;
    if (a->dateVal != b->dateVal)
        return a->dateVal - b->dateVal;
    if (a->seq != b->seq)
        return (a->seq > b->seq) ? 1 : -1;
    return 0;
}

MaxHeap* createHeap() {
    MaxHeap *h = (MaxHeap*)malloc(sizeof(MaxHeap));
    h->capacity = INITIAL_CAPACITY;
    h->size = 0;
    h->arr = (Email**)malloc(sizeof(Email*) * h->capacity);
    return h;
}

void swapEmail(Email **a, Email **b) {
    Email *t = *a;
    *a = *b;
    *b = t;
}

void heapifyUp(MaxHeap *h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compareEmail(h->arr[idx], h->arr[parent]) > 0) {
            swapEmail(&h->arr[idx], &h->arr[parent]);
            idx = parent;
        } else break;
    }
}

void heapifyDown(MaxHeap *h, int idx) {
    while (1) {
        int left = 2*idx + 1;
        int right = 2*idx + 2;
        int largest = idx;
        if (left < h->size && compareEmail(h->arr[left], h->arr[largest]) > 0)
            largest = left;
        if (right < h->size && compareEmail(h->arr[right], h->arr[largest]) > 0)
            largest = right;
        if (largest != idx) {
            swapEmail(&h->arr[idx], &h->arr[largest]);
            idx = largest;
        } else break;
    }
}

void heapInsert(MaxHeap *h, Email *e) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->arr = (Email**)realloc(h->arr, sizeof(Email*) * h->capacity);
    }
    h->arr[h->size] = e;
    heapifyUp(h, h->size);
    h->size++;
}

Email* heapPeek(MaxHeap *h) {
    if (h->size == 0) return NULL;
    return h->arr[0];
}

Email* heapExtractMax(MaxHeap *h) {
    if (h->size == 0) return NULL;
    Email *max = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    if (h->size > 0) heapifyDown(h, 0);
    return max;
}

int main(int argc, char *argv[]) {
    FILE *fp = stdin;
    if (argc >= 2) {
        fp = fopen(argv[1], "r");
        if (!fp) {
            perror("Could not open file");
            return 1;
        }
    }

    MaxHeap *heap = createHeap();
    char line[MAX_LINE];
    long globalSeq = 0;

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            char *rest = line + 6;
            char *firstComma = strchr(rest, ',');
            char *lastComma = strrchr(rest, ',');
            if (!firstComma || !lastComma || firstComma == lastComma) continue;

            char sender[32] = {0};
            strncpy(sender, rest, firstComma - rest);
            sender[firstComma - rest] = '\0';

            char dateStr[16] = {0};
            strcpy(dateStr, lastComma + 1);

            char subject[256] = {0};
            int subjLen = lastComma - (firstComma + 1);
            strncpy(subject, firstComma + 1, subjLen);
            subject[subjLen] = '\0';

            trim(sender);
            trim(subject);
            trim(dateStr);

            Email *e = (Email*)malloc(sizeof(Email));
            strcpy(e->sender, sender);
            strcpy(e->subject, subject);
            strcpy(e->dateStr, dateStr);
            e->dateVal = parseDateVal(dateStr);
            e->catPriority = getCatPriority(sender);
            e->seq = globalSeq++;

            heapInsert(heap, e);
        }
        else if (strncmp(line, "COUNT", 5) == 0) {
            printf("There are %d emails to read.\n\n", heap->size);
        }
        else if (strncmp(line, "NEXT", 4) == 0) {
            Email *top = heapPeek(heap);
            if (!top) {
                printf("Next email:\n    No emails to read.\n\n");
            } else {
                printf("Next email:\n");
                printf("    Sender: %s\n", top->sender);
                printf("    Subject: %s\n", top->subject);
                printf("    Date: %s\n\n", top->dateStr);
            }
        }
        else if (strncmp(line, "READ", 4) == 0) {
            Email *removed = heapExtractMax(heap);
            if (removed) free(removed);
        }
    }

    for (int i = 0; i < heap->size; i++) free(heap->arr[i]);
    free(heap->arr);
    free(heap);
    if (fp != stdin) fclose(fp);
    return 0;
}

