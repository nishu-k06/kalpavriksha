#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define tableSize 1009 

//node for doubly linked list
typedef struct node{
    int key;
    char *value;
    struct node *next;
    struct node *prev;
}node; 

//hash table structure
typedef struct hashTable{
    int key;
    node *node;
    struct hashTable *next; 

}hashTable;

//cache structure
typedef struct lruCache{
    int capacity;
    int size;
    node *head;
    node *tail;
    hashTable **table;
    
}lruCache;

//creating cache 
lruCache *createCache(int capacity){
    lruCache *cache = malloc(sizeof(lruCache));
    if(!cache){
        printf("memory allocation failed\n");
        return NULL;
    }       
     
    cache->capacity=capacity;
    cache->size=0;
    cache->head=NULL;
    cache->tail=NULL;

    cache->table = calloc(tableSize,sizeof(hashTable*));

    return cache;
}

int hash(int key){
    if (key<0) key = -key;
    return key%tableSize;
}

//creating node
node* createNode(int key, char *value){
    node *newnode = malloc(sizeof(node));

    newnode->key=key;
    newnode->value=malloc(strlen(value)+1);
    strcpy(newnode->value,value);

    newnode->next=NULL;
    newnode->prev=NULL;

    return newnode;
}

//add new node to front
void addFront(lruCache *cache,node *newnode){
    newnode->prev=NULL;
    newnode->next=cache->head;

    if(cache->head) cache->head->prev=newnode;
    
    cache->head=newnode;
    
    //only for first element
    if(cache->tail==NULL){
        cache->tail=newnode;
    }
}


//inserting into hashTable
void insertValue(lruCache *cache, int key,node *newnode){
    hashTable *newEntry = malloc(sizeof(hashTable));

    int index = hash(key);

    newEntry->key=key;
    newEntry->node=newnode;
    newEntry->next = cache->table[index];   
    cache->table[index] = newEntry;

}

//retrieving key
hashTable *getkey(lruCache* cache, int key){
    int index = hash(key);
    
    hashTable * current = cache->table[index];
    
    while(current!= NULL){
        if(current->key==key){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//removing node
void removeNode(lruCache *cache, node *leastUsed){
    if(leastUsed->prev!=NULL){
        leastUsed->prev->next=leastUsed->next;
    }
    else{
        cache->head=leastUsed->next;
    }

    if (leastUsed->next!=NULL){
        leastUsed->next->prev=leastUsed->prev;
    }
    else
    {
        cache->tail=leastUsed->prev;
    }
}

//moving node front when accessed 
void moveFront(lruCache *cache,node *current){
    if (cache->head==current) return;

    removeNode(cache,current);
    addFront(cache,current);
}

//get Least Recently Used
node *getLru(lruCache *cache){
    node *leastUsed = cache->tail;
    removeNode(cache, leastUsed);
    return leastUsed;
}

//freeing the LRU
void deleteKey(lruCache *cache, int key){
    int index = hash(key);

    hashTable *current = cache->table[index];
    hashTable *prev = NULL;
    
    while(current!=NULL){
        if(current->key==key){
            if(prev!=NULL){
                prev->next=current->next;
            }
            else {
                cache->table[index]=current->next;
            }
            free(current);
            return;
        }
        prev=current;
        current=current->next;
    }

}

//adding  value to hashmap
void put (lruCache *cache,int key ,char *value){
        
    hashTable *bucket = getkey(cache, key);

    if(bucket != NULL){
        free(bucket->node->value);

        bucket->node->value=malloc(strlen(value)+1);
        strcpy(bucket->node->value,value);

        moveFront(cache,bucket->node);
        return;
    }


    if(cache->capacity==cache->size){
        node *tail= getLru(cache);
        deleteKey(cache,tail->key);
        free(tail->value);
        free(tail);
        
        cache->size--;

    }

    node *newNode = createNode(key, value); 
    addFront(cache,newNode);
    insertValue(cache,key,newNode);

    cache->size++;
    return;
}


//retrieving  value
char* getValue(lruCache* cache,int key){
    hashTable *bucket = getkey(cache,key);
    
    if(!bucket) return NULL;

    moveFront(cache,bucket->node);
    return bucket->node->value;

}

//freeing memory on exit
void freeCache(lruCache *cache){
    node *current = cache->head;

    while(current!=NULL){
        node *next = current->next;
        free(current->value);
        free(current);
        current =next;
    }

    for (int i=0; i<tableSize ; i++){
        hashTable *bucket = cache->table[i];
        while(bucket!=NULL){
            hashTable *next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }
    free(cache->table);
    free(cache);
}


int main(){
    lruCache *cache = NULL; 
    char command[20];

    printf("commands: createCache <capacity>,\n put <key> <value>,\n get <key>,\n exit\n");

    while(1){

        printf("\nEnter command: \n");

        scanf("%19s",command);

         //create new cache
         if (strcmp(command, "createCache") == 0) {
            if (cache) freeCache(cache);
            int capacity;

            while(1){
                if(scanf("%d", &capacity) != 1){
                    printf("Enter an integer \n");
                    while(getchar() != '\n'); 
                } 
                else if(capacity < 1 || capacity > 1000){
                    printf("Enter between 1 and 1000\n");
                }
                 else {
                    break;
                }
            }
            cache = createCache(capacity);
            if (!cache) {                       
                printf("Cache not created yet!\n");
                continue;
            }
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
                while (getchar() !='\n');
            }

            while(1){
                if (scanf("%99s", value)==1) break;
                
                else printf("Enetr a valid string.\n");
                while (getchar()!='\n');
            }

            put(cache,key,value);
            printf("Key - Value Inserted.\n");
        }

        else if (strcmp(command, "get")== 0) {
            if (!cache) {
                printf("cache not created yet!\n");
                continue;
            }
            
            int key;
            while(1){
                if(scanf("%d", &key) != 1){
                    printf("enter an integer \n");
                    while(getchar()!='\n');
                } 
                else break;
            }

            char *val = getValue(cache, key);
            if (val) printf("%s\n", val);
            else printf("NULL\n");
        }

        else if (strcmp(command, "exit") == 0) {
            if (cache) freeCache(cache);
            break;
        }
        else{
            printf("Invalid command! \nRe-enter valid command,\n");

            while(getchar()!='\n');
        }
    }

    return 0;
}