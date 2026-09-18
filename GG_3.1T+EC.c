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

#include <stdio.h> // FILE, fopen, fget (input output)
#include <stdlib.h> // malloc, reallloc, free (memory allciation)
#include <string.h> //used for measuring, comparing, copying, and parsing null-terminated character strings
#include <ctype.h> // isspace, used inside trim()

#define MAX_SENDER 32 // max char for sender
#define MAX_SUBJECT 128 // max char for subject
#define MAX_DATE 16 // max char for date 

// Structure to represent an Email
typedef struct {
    char sender[MAX_SENDER]; // name of sender
    char subject[MAX_SUBJECT]; // email subject 
    char date_str[MAX_DATE]; // date of email
    int category_priority; // priority from sender
    long date_score; // date_str for comparason 
    int insert_id; // arrival order
} Email;

// Structure to represent the MaxHeap priority queue
typedef struct { 
    Email *array; // contigus array for email stored by value
    int capacity; // apacity of array 
    int size; // number of emails currently stored
} MaxHeap;

// Strips leading and trailing whitespace from s, in place.
void trim(char *s) { 
    char *start = s; //trims leading space
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != s) { // find leadign space
        memmove(s, start, strlen(start) + 1); // shifts str
    }
    int len = (int)strlen(s); // len of after tirm
    while (len > 0 && isspace((unsigned char)s[len - 1])) { // walk back over 
        s[len - 1] = '\0'; // chop each off
        len--;
    }
}

// Compares two emails to determine which has higher priority.
// Takes const Email* to avoid copying both structs on every call.
int compareEmails(const Email *a, const Email *b) {
    if (a->category_priority != b->category_priority) { // sends category that wins
        return a->category_priority - b->category_priority; // returns value
    }
    if (a->date_score != b->date_score) { // newest date
        return (a->date_score > b->date_score) ? 1 : -1; // returns vale
    }
    return (a->insert_id > b->insert_id) ? 1 : -1; // earlies arrval
}

MaxHeap* createHeap(int capacity) { 
    MaxHeap *h = (MaxHeap*)malloc(sizeof(MaxHeap)); // allocates the heap
    h->capacity = capacity; // stores it
    h->size = 0; // starts heap
    h->array = (Email*)malloc(sizeof(Email) * capacity); // backing array
    return h;
}

void insertHeap(MaxHeap *h, Email e) {
    if (h->size == h->capacity) { // if array is full
        h->capacity *= 2; // doouble the space
        h->array = (Email*)realloc(h->array, sizeof(Email) * h->capacity); // gros in place
    }

    h->array[h->size] = e; // place the new email at the next free slot
    int idx = h->size;  // remeber where it is
    h->size++; // heap now has one more element

    while (idx > 0) { // shiift up wap new element towards the root 
        int parent = (idx - 1) / 2; //index of parent
        if (compareEmails(&h->array[idx], &h->array[parent]) > 0) { // child >parent 
            Email temp = h->array[idx]; // swap places
            h->array[idx] = h->array[parent]; // checks value??
            h->array[parent] = temp; // temp to store place
            idx = parent; // shiting up from parents olds slot
        } else {
            break; // heap property restored
        }
    }
}

void extractMax(MaxHeap *h) {
    if (h->size <= 0) return; // nothing to remove 

    h->array[0] = h->array[h->size - 1]; // move the last element up to root
    h->size--; // heap has one less element

    int idx = 0; // shift down towards new leaves
    while (2 * idx + 1 < h->size) { // has left child?
        int left = 2 * idx + 1; //index with max heap 
        int right = 2 * idx + 2; //index with max heap 
        int largest = idx; //parent wis 

        if (compareEmails(&h->array[left], &h->array[largest]) > 0) { // if left child bigger 
            largest = left; // largest is now left
        }
        if (right < h->size && compareEmails(&h->array[right], &h->array[largest]) > 0) { // if right child bigger
            largest = right; //largest is now left
        }

        if (largest != idx) { // if largest is not eq
            Email temp = h->array[idx]; // swap parent with larger child
            h->array[idx] = h->array[largest]; //finds index?
            h->array[largest] = temp; // temp var to keep value
            idx = largest; //replace and shifts down 
        } else {
            break; // restored
        }
    }
}
void freeHeap(MaxHeap *h) {
    free(h->array);   // free the contiguous Email array
    free(h);           // free the heap struct itself
}
 
int parseEmailLine(char *ptr, Email *out, int insert_id, long lineNo) {
    char *sender = strtok(ptr, ","); // sender/category
    char *subject = strtok(NULL, ","); // subject
    char *date_str = strtok(NULL, ","); // date
 

    if (!sender || !subject || !date_str) {  // corrective fix now reports error
        fprintf(stderr, "Warning: skipping malformed EMAIL line %ld (missing field)\n", lineNo); // prints error
        return 0;
    }
 

    trim(sender); // formattes sender
    trim(subject); // formattes sender
    trim(date_str);// formattes sender
 
    if (sender[0] == '\0') {  // if sender was all whitespace
        fprintf(stderr, "Warning: skipping malformed EMAIL line %ld (empty sender)\n", lineNo);
        return 0; // ends
    }
 
    strncpy(out->sender, sender, MAX_SENDER - 1);   // bounded copy (unchanged from Gemini)
    out->sender[MAX_SENDER - 1] = '\0';              // guarantee null-termination
 
    strncpy(out->subject, subject, MAX_SUBJECT - 1); // bounded copy
    out->subject[MAX_SUBJECT - 1] = '\0';
 
    strncpy(out->date_str, date_str, MAX_DATE - 1);  // bounded copy
    out->date_str[MAX_DATE - 1] = '\0';
 
    out->insert_id = insert_id;                       // record arrival order for tie-breaking
 
    // Assign a numeric priority based on the (trimmed) sender category
    if (strcmp(sender, "Boss") == 0) out->category_priority = 5;
    else if (strcmp(sender, "Subordinate") == 0) out->category_priority = 4;
    else if (strcmp(sender, "Peer") == 0) out->category_priority = 3;
    else if (strcmp(sender, "ImportantPerson") == 0) out->category_priority = 2;
    else if (strcmp(sender, "OtherPerson") == 0) out->category_priority = 1;
    else out->category_priority = 0;   // unrecognized sender gets the lowest priority
 
    int m = 0, d = 0, y = 0;
    sscanf(date_str, "%d-%d-%d", &m, &d, &y);          // parse the three numeric parts
    out->date_score = (long)y * 10000 + (long)m * 100 + (long)d; // combine into one sortable value
 
    return 1;   // parsed successfully
}
 
int main(int argc, char *argv[]) {
    // Open the file named on the command line, or fall back to stdin
    FILE *fp = stdin;
    if (argc > 1) {
        fp = fopen(argv[1], "r"); // try to open the given file
        if (!fp) {
            printf("Error opening file %s\n", argv[1]);
            return 1; // bail out if the file
        }
    }
 
    MaxHeap *heap = createHeap(10); // start with room for 10 emails
    char line[256]; // buffer for one line of input
    int insert_counter = 0; // counts only successfully parsed emails
    long lineNo = 0; // running line number
 
    // Process the input one line at a time
    while (fgets(line, sizeof(line), fp)) {
        lineNo++; // count this line, valid or not
        line[strcspn(line, "\r\n")] = 0; // strip trailing \r and/or \n
        if (strlen(line) == 0) continue; // skip blank lines
 
        if (strncmp(line, "EMAIL ", 6) == 0) { // an EMAIL record
            Email e; // scratch struct
            if (parseEmailLine(line + 6, &e, insert_counter + 1, lineNo)) { // try to parse it
                insert_counter++; // count successful parses
                insertHeap(heap, e); // add to the heap
            }
        }
        else if (strcmp(line, "NEXT") == 0) { // peek at the top email
            if (heap->size > 0) {
                Email e = heap->array[0];// copy just for printing
                printf("Next email:\n");
                printf("    Sender: %s\n", e.sender);
                printf("    Subject: %s\n", e.subject);
                printf("    Date: %s\n\n", e.date_str);
            } else {
                printf("Next email:\n    No emails to read.\n\n"); // heap is empty
            }
        }
        else if (strcmp(line, "READ") == 0) { // remove the top email
            extractMax(heap);
        }
        else if (strcmp(line, "COUNT") == 0) {  // report queue size
            printf("There are %d emails to read.\n\n", heap->size);
        }
        else {
            // Unrecognized command line flagged rather than ignored silently
            fprintf(stderr, "Warning: ignoring unrecognized line %ld: \"%s\"\n", lineNo, line);
        }
    }
 
    if (fp != stdin) {
        fclose(fp); // close the input file if we opened one ourselves
    }
 
    freeHeap(heap); // release all heap memory before exiting
    return 0;
}
 
