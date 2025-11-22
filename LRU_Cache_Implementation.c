#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


#define HASH_SIZE 1009   // any prime number greater than capacity(1000) to reduce collisions

// node for doubly linked list
typedef struct node {
    int key;
    char *value;
    struct node *prev;
    struct node *next;
} node;

// Hash table entry (for separate chaining)
typedef struct HashEntry {
    int key;
    node *node;
    struct HashEntry *next;
} HashEntry;

// LRU Cache structure
typedef struct {
    int capacity;
    int size;
    node *head;   
    node *tail;   
    HashEntry **table;
} LRUCache;


// simple hash function
int hash(int key) {
    if (key < 0) key =-key;//to avoid negative keys
    return key % HASH_SIZE;
}

// find a key in hash table
HashEntry* findKey(LRUCache *cache, int key) {
    int h = hash(key);
    HashEntry *curr = cache->table[h];

    while (curr) {
        if (curr->key == key)
            return curr; //key found
        curr = curr->next;
    }
    return NULL;
}

// insert into hash table
void insertEntry(LRUCache *cache, int key, node *node) {
    int h = hash(key);
    HashEntry *newKey = malloc(sizeof(HashEntry));

    newKey->key = key;
    newKey->node = node;
    newKey->next = cache -> table[h];   // insert at head of chain
    cache->table[h] = newKey ;
}

// delete from hash table
void deleteEntry(LRUCache *cache, int key) {
    int h = hash(key);

    HashEntry *curr = cache->table[h];
    HashEntry *prev = NULL;

    while (curr) {
        if (curr->key == key) {
            if (prev)
                prev->next = curr->next;
            else
                cache->table[h] = curr->next;

            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}



// create a new doubly linked list node
node* createNode(int key, const char *value) {
    node *n = malloc(sizeof(node));
    n->key = key;

    n->value = malloc(strlen(value)+1);
    strcpy(n->value, value); //copying data

    n->prev = n->next = NULL;
    return n;
}

// add node to the front 
void moveFront(LRUCache *cache, node *n) {
    n->prev = NULL;
    n->next = cache->head;

    if (cache->head)
        cache->head->prev = n;

    cache->head = n;

    if (cache->tail == NULL)
        cache->tail = n;  // first element
}

// remove a node from anywhere in the list
void removeNode(LRUCache *cache, node *n) {
    if (n->prev)
        n->prev->next = n->next;
    else
        cache->head = n->next;

    if (n->next)
        n->next->prev = n->prev;
    else
        cache->tail = n->prev;
}

// move an existing node to front 
void moveToFront(LRUCache *cache, node *n) {
    if (cache->head == n) return; // already MRU

    removeNode(cache, n);
    moveFront(cache, n);
}

// remove least recently used 
node* removeLRU(LRUCache *cache) {
    node *lru = cache->tail;
    removeNode(cache, lru);
    return lru;
}


// create LRU Cache
LRUCache* createCache(int capacity) {
    LRUCache *cache = malloc(sizeof(LRUCache));

    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;

    cache->table = calloc(HASH_SIZE, sizeof(HashEntry*));//cretate hash table with all entries NULL

    return cache;
}

// get value for a key
char* get(LRUCache *cache, int key) {
    HashEntry *entry = findKey(cache, key);

    if (!entry)
        return NULL;

    moveToFront(cache, entry->node);
    return entry->node->value;
}

// put key/value
void put(LRUCache *cache, int key, const char *value) {

    HashEntry *entry = findKey(cache, key);

    // key already exists  update
    if (entry) {

        free(entry->node->value);

        entry->node->value = malloc(strlen(value)+1); 
        strcpy(entry->node->value, value); 

        moveToFront(cache, entry->node);
        return;
    }

    // if cache full → remove LRU
    if (cache->size == cache->capacity) {
        node *tail = removeLRU(cache);
        deleteEntry(cache, tail->key);
        free(tail->value);
        free(tail);
        cache->size--;
    }

    // insert new node
    node *n = createNode(key, value);
    moveFront(cache, n);
    insertEntry(cache, key, n);

    cache->size++;
}


// free the entire cache
void freeCache(LRUCache *cache) {
    node *curr = cache->head;

    while (curr) {
        node *next = curr->next;
        free(curr->value);
        free(curr);
        curr = next;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *e = cache->table[i];
        while (e) {
            HashEntry *next = e->next;
            free(e);
            e = next;
        }
    }
    free(cache->table);
    free(cache);
}

//main function
int main(){
    LRUCache *cache = NULL; 
    char command[20];

    printf("commands: createCache <capacity>,\n put <key> <value>,\n get <key>,\n exit\n");

    while(1){

        printf("Enter command: \n");

        scanf("%19s",command);

         //create new cache
         if (strcmp(command, "createCache") == 0) {
            if (cache) freeCache(cache);
            int capacity;

            while(1){
                if(scanf("%d", &capacity) != 1){
                    printf("enter an integer \n");
                    while(getchar() != '\n'); // clear invalid input
                } 
                else if(capacity < 1 || capacity > 1000){
                    printf("enter between 1 and 1000\n");
                }
                 else {
                    break;
                }
            }
            cache = createCache(capacity);
            printf("Cache created.\n");
        }

         //insert key-value into cache
        else if (strcmp(command, "put") == 0) { 
            if (!cache) {                       
                printf("Cache not created yet!\n");
                continue;
            }

            int key;
            char value[100];

            while(1){
                if(scanf("%d", &key) == 1) break;
                
                else printf("Enter a valid integer.\n");
                while (getchar() != '\n');
            }

            while(1){
                if (scanf("%99s", value) == 1) break;
                
                else printf("Enetr a valid string.\n");
                while (getchar() != '\n');
            }

            put(cache,key,value);
            printf("Key - Value Inserted.\n");
        }

        else if (strcmp(command, "get") == 0) {
            if (!cache) {
                printf("cache not created yet!\n");
                continue;
            }
            
            int key;
            while(1){
                if(scanf("%d", &key) != 1){
                    printf("enter an integer \n");
                    while(getchar() != '\n');
                } 
                else break;
            }

            char *val = get(cache, key);
            if (val) printf("%s\n", val);
            else printf("NULL\n");
        }

        else if (strcmp(command, "exit") == 0) {
            if (cache) freeCache(cache);
            break;
        }
        else{
            printf("Invalid command! \nRe-enter valid command,\n");

            while(getchar() != '\n');//clearing input buffer
        }
    }

    return 0;
}
