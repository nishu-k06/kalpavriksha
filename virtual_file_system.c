#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants 
#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LEN 50

// Doubly Linked List for free blocks
typedef struct FreeBlock {
    int index;
    struct FreeBlock *prev, *next;
} FreeBlock;

// Circular Linked List for files and directories
typedef struct FileNode {
    char name[MAX_NAME_LEN];
    int isDirectory;
    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *child;

    int size;
    int blockCount;
    int *blockPointers;
} FileNode;

// Global Variables
unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeListHead = NULL;
FileNode *rootDir = NULL;
FileNode *currentDir = NULL;
int usedBlocks = 0;

// Validate Name
int isValidName(const char *name) {
    for (int i = 0; name[i]; i++) {
        if (name[i] == '/' || name[i] == '"' || name[i] == '\\' || name[i] == ' ')
            return 0;
    }
    return 1;
}

// Free Block Management
void initializeFreeBlocks() {
    FreeBlock *prev = NULL;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        FreeBlock *node = (FreeBlock *)malloc(sizeof(FreeBlock));
        node->index = i;
        node->prev = prev;
        node->next = NULL;
        if (prev)
            prev->next = node;
        else
            freeListHead = node;
        prev = node;
    }
}

// Allocate a free block
FreeBlock *allocateBlock() { 
    if (!freeListHead)
        return NULL;
    FreeBlock *block = freeListHead;
    freeListHead = block->next;
    if (freeListHead)
        freeListHead->prev = NULL;
    usedBlocks++;
    return block;
}

// Free a block
void freeBlock(int index) {
    FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));
    newBlock->index = index;
    newBlock->next = NULL;
    if (!freeListHead) {
        newBlock->prev = NULL;
        freeListHead = newBlock;
    } else {
        FreeBlock *tail = freeListHead;
        while (tail->next)
            tail = tail->next;
        tail->next = newBlock;
        newBlock->prev = tail;
    }
    usedBlocks--;
}

// File System Helpers
FileNode *createNode(const char *name, int isDir) {
    FileNode *node = (FileNode *)malloc(sizeof(FileNode));
    strcpy(node->name, name);
    node->isDirectory = isDir;
    node->parent = currentDir;
    node->next = node->prev = node;
    node->child = NULL;
    node->size = 0;
    node->blockCount = 0;
    node->blockPointers = NULL;
    return node;
}

// Find a node in the current directory
FileNode *findNode(const char *name) {
    if (!currentDir->child)
        return NULL;
    FileNode *temp = currentDir->child;
    do {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    } while (temp != currentDir->child);
    return NULL;
}

// Insert a node into the current directory
void insertNode(FileNode *node) {
    if (!currentDir->child) {
        currentDir->child = node;
    } else {
        FileNode *tail = currentDir->child->prev;
        tail->next = node;
        node->prev = tail;
        node->next = currentDir->child;
        currentDir->child->prev = node;
    }
}

// Remove a node from the current directory
void removeNode(FileNode *node) {
    if (node->next == node)
        currentDir->child = NULL;
    else {
        if (currentDir->child == node)
            currentDir->child = node->next;
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    if (!node->isDirectory && node->blockPointers)
        free(node->blockPointers);
    free(node);
}

// Command Implementations

//create directory
void mkdirCmd(char *dirName) {
    if (!dirName || strlen(dirName) == 0) {
        printf("Usage: mkdir <directory_name>\n");
        return;
    }
    if (strlen(dirName) > MAX_NAME_LEN) {
        printf("Error: Directory name exceeds %d characters.\n", MAX_NAME_LEN);
        return;
    }
    if (!isValidName(dirName)) {
        printf("Error: Invalid directory name.\n");
        return;
    }
    if (findNode(dirName)) {
        printf("Name already exists in current directory.\n");
        return;
    }
    FileNode *newDir = createNode(dirName, 1);
    insertNode(newDir);
    printf("Directory '%s' created successfully.\n", dirName);
}

//create file
void createCmd(char *fileName) {
    if (!fileName || strlen(fileName) == 0) {
        printf("Usage: create <file_name>\n");
        return;
    }
    if (strlen(fileName) > MAX_NAME_LEN) {
        printf("Error: File name exceeds %d characters.\n", MAX_NAME_LEN);
        return;
    }
    if (!isValidName(fileName)) {
        printf("Error: Invalid file name.\n");
        return;
    }
    if (findNode(fileName)) {
        printf("Name already exists in current directory.\n");
        return;
    }
    FileNode *newFile = createNode(fileName, 0);
    insertNode(newFile);
    printf("File '%s' created successfully.\n", fileName);
}

//write data to file
void writeCmd(char *fileName, const char *data) {
    if (!fileName || !data) {
        printf("Usage: write <file_name> \"<data>\"\n");
        return;
    }

    FileNode *file = findNode(fileName);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    int dataLen = strlen(data);
    int blocksNeeded = (dataLen + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (blocksNeeded > NUM_BLOCKS - usedBlocks) {
        printf("Error: Disk full.\n");
        return;
    }

    file->blockPointers = (int *)malloc(blocksNeeded * sizeof(int));
    for (int i = 0; i < blocksNeeded; i++) {
        FreeBlock *block = allocateBlock();
        file->blockPointers[file->blockCount++] = block->index;
        strncpy((char *)virtualDisk[block->index], data + (i * BLOCK_SIZE), BLOCK_SIZE);
        free(block);
    }

    file->size = dataLen;
    printf("Data written successfully (size=%d bytes).\n", dataLen);
}

//read data from file
void readCmd(char *fileName) {
    if (!fileName || strlen(fileName) == 0) {
        printf("Usage: read <file_name>\n");
        return;
    }
    FileNode *file = findNode(fileName);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }
    if (file->blockCount == 0) {
        printf("(empty)\n");
        return;
    }
    for (int i = 0; i < file->blockCount; i++)
        printf("%s", virtualDisk[file->blockPointers[i]]);
    printf("\n");
}

//delete file
void deleteCmd(char *fileName) {
    if (!fileName || strlen(fileName) == 0) {
        printf("Usage: delete <file_name>\n");
        return;
    }
    FileNode *file = findNode(fileName);
    if (!file || file->isDirectory) {
        printf("File not found.\n");
        return;
    }

    for (int i = 0; i < file->blockCount; i++)
        freeBlock(file->blockPointers[i]);
    free(file->blockPointers);
    removeNode(file);
    printf("File deleted successfully.\n");
}

//remove directory
void rmdirCmd(char *dirName) {
    if (!dirName || strlen(dirName) == 0) {
        printf("Usage: rmdir <directory_name>\n");
        return;
    }
    FileNode *dir = findNode(dirName);
    if (!dir || !dir->isDirectory) {
        printf("Directory not found.\n");
        return;
    }
    if (dir->child) {
        printf("Directory not empty. Remove files first.\n");
        return;
    }
    removeNode(dir);
    printf("Directory removed successfully.\n");
}

//change directory
void cdCmd(char *dirName) {
    if (!dirName) {
        printf("Usage: cd <directory_name>\n");
        return;
    }
    if (strcmp(dirName, "..") == 0) {
        if (currentDir->parent)
            currentDir = currentDir->parent;
        printf("Moved to %s\n", currentDir == rootDir ? "/" : currentDir->name);
        return;
    }

    FileNode *dir = findNode(dirName);
    if (!dir || !dir->isDirectory) {
        printf("Directory not found.\n");
        return;
    }

    currentDir = dir;
    printf("Moved to /%s\n", currentDir->name);
}

//list directory contents
void lsCmd() {
    if (!currentDir->child) {
        printf("(empty)\n");
        return;
    }
    FileNode *temp = currentDir->child;
    do {
        printf("%s%s\n", temp->name, temp->isDirectory ? "/" : "");
        temp = temp->next;
    } while (temp != currentDir->child);
}

//print working directory
void pwdCmd() {
    if (currentDir == rootDir) {
        printf("/\n");
        return;
    }
    FileNode *temp = currentDir;
    char path[256] = "";
    while (temp != rootDir) {
        char buffer[100];
        sprintf(buffer, "/%s%s", temp->name, path);
        strcpy(path, buffer);
        temp = temp->parent;
    }
    printf("%s\n", path);
}

//disk free command
void dfCmd() {
    double percent = (usedBlocks * 100.0) / NUM_BLOCKS;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\nDisk Usage: %.2f%%\n",
           NUM_BLOCKS, usedBlocks, NUM_BLOCKS - usedBlocks, percent);
}

// Memory Cleanup
void freeAllFreeBlocks() {
    FreeBlock *temp = freeListHead;
    while (temp) {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }
}

void freeDirectory(FileNode *dir) {
    if (!dir)
        return;
    if (dir->child) {
        FileNode *temp = dir->child;
        FileNode *start = temp;
        do {
            FileNode *next = temp->next;
            freeDirectory(temp);
            temp = next;
        } while (temp != start);
    }
    if (!dir->isDirectory && dir->blockPointers)
        free(dir->blockPointers);
    free(dir);
}

// Commands List Display
void displayCommandList() {
    printf("\n Commands List:\n");
    printf("mkdir <dir>        : Create a new directory\n");
    printf("create <file>      : Create a new file\n");
    printf("write <file> \"data\": Write data to file\n");
    printf("read <file>        : Read file contents\n");
    printf("delete <file>      : Delete a file\n");
    printf("rmdir <dir>        : Remove a directory\n");
    printf("cd <dir>/..        : Change directory\n");
    printf("ls                 : List directory contents\n");
    printf("pwd                : Show current path\n");
    printf("df                 : Show disk usage\n");
    printf("exit               : Exit the program\n");
    printf("\n");
}

// Main Function
int main() {
    if (NUM_BLOCKS < 1 || NUM_BLOCKS > 5000) {
        printf("Error: NUM_BLOCKS must be between 1 and 5000.\n");
        return 1;
    }

    initializeFreeBlocks();
    rootDir = createNode("/", 1);
    rootDir->parent = NULL;
    currentDir = rootDir;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    displayCommandList();  // Show command list once

    char input[512], arg1[100], arg2[512];

    while (1) {
        printf("%s > ", currentDir == rootDir ? "/" : currentDir->name);
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "mkdir %s", arg1) == 1)
            mkdirCmd(arg1);
        else if (sscanf(input, "create %s", arg1) == 1)
            createCmd(arg1);
        else if (sscanf(input, "write %s \"%[^\"]\"", arg1, arg2) == 2)
            writeCmd(arg1, arg2);
        else if (sscanf(input, "read %s", arg1) == 1)
            readCmd(arg1);
        else if (sscanf(input, "delete %s", arg1) == 1)
            deleteCmd(arg1);
        else if (sscanf(input, "rmdir %s", arg1) == 1)
            rmdirCmd(arg1);
        else if (sscanf(input, "cd %s", arg1) == 1)
            cdCmd(arg1);
        else if (strcmp(input, "ls\n") == 0)
            lsCmd();
        else if (strcmp(input, "pwd\n") == 0)
            pwdCmd();
        else if (strcmp(input, "df\n") == 0)
            dfCmd();
        else if (strcmp(input, "exit\n") == 0) {
            printf("Memory released. Exiting program...\n");
            freeDirectory(rootDir);
            freeAllFreeBlocks();
            break;
        } else {
            printf("Invalid or incomplete command. Type 'help' to view commands.\n");
        }
    }
    return 0;
}
