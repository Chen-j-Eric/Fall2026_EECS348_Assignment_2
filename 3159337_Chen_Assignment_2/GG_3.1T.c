/*
  name: EECS 348 Assignment 2
  purpose: Puts email into a Maxheap as a means of implementing a priority queue where enails can 
           shift towards the frount based on priority
  outputs: Next email, shows sender subject and dates
  collaborators: Google Gemini 3.1 Pro Thinking 
  author: Eric Chen
  creation date: 09/17/2026

  
    prologue comments
    adding comments to all lines
    Worked first time with the extensive prompt
    Resulting outputs are expected and does not crash

    handles error, ints, floats and random letters

*/



#include <stdio.h> //FILE, Fopen, printf (basic input output)
#include <stdlib.h> //malloc, realloc, free
#include <string.h> // 

#define MAX_SENDER 32
#define MAX_SUBJECT 128
#define MAX_DATE 16

// Structure to represent an Email
typedef struct {
    char sender[MAX_SENDER];
    char subject[MAX_SUBJECT];
    char date_str[MAX_DATE];
    int category_priority;
    long date_score;
    int insert_id;
} Email;

// Structure to represent the MaxHeap priority queue
typedef struct {
    Email *array;
    int capacity;
    int size;
} MaxHeap;

// Helper function to compare two emails and determine which has higher priority.
// Returns >0 if a has higher priority, <0 if b has higher priority.
int compareEmails(Email a, Email b) {
    // 1. Primary priority: Sender Category
    if (a.category_priority != b.category_priority) {
        return a.category_priority - b.category_priority;
    }
    // 2. Secondary priority: Date (Newest first)
    if (a.date_score != b.date_score) {
        return (a.date_score > b.date_score) ? 1 : -1;
    }
    // 3. Tertiary priority: Arrival/Insertion order (to handle identical dates)
    return (a.insert_id > b.insert_id) ? 1 : -1;
}

// Function to initialize a new MaxHeap
MaxHeap* createHeap(int capacity) {
    MaxHeap *h = (MaxHeap*)malloc(sizeof(MaxHeap));
    h->capacity = capacity;
    h->size = 0;
    h->array = (Email*)malloc(sizeof(Email) * capacity);
    return h;
}

// Function to insert an email into the MaxHeap
void insertHeap(MaxHeap *h, Email e) {
    // Resize array if capacity is reached
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->array = (Email*)realloc(h->array, sizeof(Email) * h->capacity);
    }
    
    // Insert at the end of the heap
    h->array[h->size] = e;
    int idx = h->size;
    h->size++;

    // Sift Up to maintain MaxHeap property
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compareEmails(h->array[idx], h->array[parent]) > 0) {
            Email temp = h->array[idx];
            h->array[idx] = h->array[parent];
            h->array[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

// Function to remove and return the highest priority email from the MaxHeap
void extractMax(MaxHeap *h) {
    if (h->size <= 0) return; // Ignore if queue is already empty
    
    // Replace root with the last element
    h->array[0] = h->array[h->size - 1];
    h->size--;

    // Sift Down to maintain MaxHeap property
    int idx = 0;
    while (2 * idx + 1 < h->size) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int largest = idx;

        if (compareEmails(h->array[left], h->array[largest]) > 0) {
            largest = left;
        }
        if (right < h->size && compareEmails(h->array[right], h->array[largest]) > 0) {
            largest = right;
        }

        if (largest != idx) {
            Email temp = h->array[idx];
            h->array[idx] = h->array[largest];
            h->array[largest] = temp;
            idx = largest;
        } else {
            break;
        }
    }
}

// Function to safely free the allocated memory for the heap
void freeHeap(MaxHeap *h) {
    free(h->array);
    free(h);
}

int main(int argc, char *argv[]) {
    // Open the test file provided as a command line argument, or default to stdin
    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (!fp) {
            printf("Error opening file %s\n", argv[1]);
            return 1;
        }
    }

    MaxHeap *heap = createHeap(10);
    char line[256];
    int insert_counter = 0;

    // Process line by line
    while (fgets(line, sizeof(line), fp)) {
        // Strip out carriage returns and newlines
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            char *ptr = line + 6;
            char *sender = strtok(ptr, ",");
            char *subject = strtok(NULL, ",");
            char *date_str = strtok(NULL, ","); 

            if (sender && subject && date_str) {
                Email e;
                strncpy(e.sender, sender, MAX_SENDER - 1);
                e.sender[MAX_SENDER - 1] = '\0';

                strncpy(e.subject, subject, MAX_SUBJECT - 1);
                e.subject[MAX_SUBJECT - 1] = '\0';

                strncpy(e.date_str, date_str, MAX_DATE - 1);
                e.date_str[MAX_DATE - 1] = '\0';
                
                // Track insertion sequence
                e.insert_id = ++insert_counter; 

                // Assign Priority Scores
                if (strcmp(sender, "Boss") == 0) e.category_priority = 5;
                else if (strcmp(sender, "Subordinate") == 0) e.category_priority = 4;
                else if (strcmp(sender, "Peer") == 0) e.category_priority = 3;
                else if (strcmp(sender, "ImportantPerson") == 0) e.category_priority = 2;
                else if (strcmp(sender, "OtherPerson") == 0) e.category_priority = 1;
                else e.category_priority = 0; // Fallback

                // Format MM-DD-YYYY to an integer YYYYMMDD for effortless numeric date comparisons
                int m = 0, d = 0, y = 0;
                sscanf(date_str, "%d-%d-%d", &m, &d, &y);
                e.date_score = (long)y * 10000 + (long)m * 100 + (long)d;

                insertHeap(heap, e);
            }
        } 
        else if (strcmp(line, "NEXT") == 0) {
            // Peek at the root (highest priority item)
            if (heap->size > 0) {
                Email e = heap->array[0];
                printf("Next email:\n");
                printf("    Sender: %s\n", e.sender);
                printf("    Subject: %s\n", e.subject);
                printf("    Date: %s\n\n", e.date_str);
            }
        } 
        else if (strcmp(line, "READ") == 0) {
            // Remove highest priority item
            extractMax(heap);
        } 
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n\n", heap->size);
        }
    }

    if (fp != stdin) {
        fclose(fp);
    }
    
    freeHeap(heap);
    return 0;
}
