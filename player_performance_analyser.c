#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

// Type definitions
typedef struct PlayerNode PlayerNode;
typedef struct Team Team;
typedef struct HeapItem HeapItem;
typedef struct MaxHeap MaxHeap;

// Function prototypes
int readInt(const char *prompt, int minVal, int maxVal);
double readDouble(const char *prompt, double minVal, double maxVal);
void readString(const char *prompt, char *out, int maxLength);

void initializeFromHeader(void);
PlayerNode *makePlayerNode(int id, const char *name, int role, int runs,double average, double strikeRate, int wickets, double economyRate);
void insertSortedDesc(PlayerNode **headRef, PlayerNode *node);
int teamIndexById(int teamId);
int isPlayerIdUnique(int pid);

void addPlayerToTeam(void);
void displayPlayersOfTeam(void);
void displayTeamsByAvgSR(void);
void displayTopKOfTeamByRole(void);
void displayAllPlayersOfRoleGlobal(void);

double computePerfIndex(int role, double average, double strikeRate, int wickets, double economyRate);
double teamAvgBattingSR(const Team *team);

void freeAll(void);

// Helper to flush input after scanf
void flushStdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// Data structures
struct PlayerNode {
    int id;
    char name[50];
    int role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    double perfIndex;
    PlayerNode *next;
};

struct Team {
    int teamId;
    char name[51];
    int totalPlayers;
    double sumBattingStrikeRate;
    int countBattingStrikeRate;
    PlayerNode *roleHeads[3];
};

// Global Variables
Team teamsArr[20];
int teamsInitialized = 0;

// Main function
int main(void) {
    initializeFromHeader();

    while (1) {
        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");

        int choice = readInt("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: addPlayerToTeam(); break;
            case 2: displayPlayersOfTeam(); break;
            case 3: displayTeamsByAvgSR(); break;
            case 4: displayTopKOfTeamByRole(); break;
            case 5: displayAllPlayersOfRoleGlobal(); break;
            case 6:
                freeAll();
                printf("Exiting. Memory released.\n");
                return 0;
        }
    }
}

// Reading integer
int readInt(const char *prompt, int minVal, int maxVal) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            flushStdin();
            if (value >= minVal && value <= maxVal)
                return value;

            printf("Value out of range.\n");
        } else {
            printf("Invalid integer.\n");
            flushStdin();
        }
    }
}

// Reading double
double readDouble(const char *prompt, double minVal, double maxVal) {
    double value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &value) == 1) {
            flushStdin();
            if (value >= minVal && value <= maxVal)
                return value;

            printf("Value out of range.\n");
        } else {
            printf("Invalid number.\n");
            flushStdin();
        }
    }
}

// Reading string
void readString(const char *prompt, char *out, int maxLength) {
    while (1) {
        printf("%s", prompt);

        if (scanf(" %49[^\n]", out) == 1) {
            flushStdin();

            if ((int)strlen(out) <= maxLength)
                return;

            printf("Name cannot exceed %d characters.\n", maxLength);
        } else {
            printf("Invalid input.\n");
            flushStdin();
        }
    }
}

// Role string
const char* roleIndexToStr(int role) {
    if (role == 0) return "Batsman";
    if (role == 1) return "Bowler";
    return "All-Rounder";
}

// Performance Index
double computePerfIndex(int role, double average, double strikeRate, int wickets, double economyRate) {
    if (role == 0) return (average * strikeRate) / 100.0;
    if (role == 1) return (wickets * 2.0) + (100.0 - economyRate);
    return ((average * strikeRate) / 100.0) + (wickets * 2.0);
}

// Create player node
PlayerNode *makePlayerNode(int id, const char *name, int role,
                           int runs, double average, double strikeRate, int wickets, double economyRate) {
    PlayerNode *node = malloc(sizeof(PlayerNode));
    if (!node) { perror("malloc"); exit(1); }

    node->id = id;
    strncpy(node->name, name, 50);
    node->name[49] = '\0';
    node->role = role;
    node->totalRuns = runs;
    node->battingAverage = average;
    node->strikeRate = strikeRate;
    node->wickets = wickets;
    node->economyRate = economyRate;
    node->perfIndex = computePerfIndex(role, average, strikeRate, wickets, economyRate);
    node->next = NULL;

    return node;
}

// Insert sorted desc
void insertSortedDesc(PlayerNode **headRef, PlayerNode *node) {
    if (!(*headRef) || node->perfIndex > (*headRef)->perfIndex) {
        node->next = *headRef;
        *headRef = node;
        return;
    }
    PlayerNode *currentNode = *headRef;
    while (currentNode->next && currentNode->next->perfIndex >= node->perfIndex)
        currentNode = currentNode->next;

    node->next = currentNode->next;
    currentNode->next = node;
}

// Initialize from header
void initializeFromHeader(void) {
    if (teamsInitialized) return;

    for (int i = 0; i < teamCount; i++) {
        teamsArr[i].teamId = i + 1;
        strncpy(teamsArr[i].name, teams[i], 50);
        teamsArr[i].name[50] = '\0';
        teamsArr[i].totalPlayers = 0;
        teamsArr[i].sumBattingStrikeRate = 0.0;
        teamsArr[i].countBattingStrikeRate = 0;
        teamsArr[i].roleHeads[0] = teamsArr[i].roleHeads[1] = teamsArr[i].roleHeads[2] = NULL;
    }

    for (int i = 0; i < playerCount; i++) {
        const Player *p = &players[i];

        int teamIndex = -1;
        for (int t = 0; t < teamCount; t++) {
            if (strcmp(p->team, teams[t]) == 0) { teamIndex = t; break; }
        }
        if (teamIndex == -1) continue;

        int roleIndex = 2;
        if (strcmp(p->role, "Batsman") == 0) roleIndex = 0;
        else if (strcmp(p->role, "Bowler") == 0) roleIndex = 1;

        PlayerNode *node = makePlayerNode(
            p->id, p->name, roleIndex,
            p->totalRuns, p->battingAverage,
            p->strikeRate, p->wickets, p->economyRate
        );

        insertSortedDesc(&teamsArr[teamIndex].roleHeads[roleIndex], node);
        teamsArr[teamIndex].totalPlayers++;

        if (roleIndex == 0 || roleIndex == 2) {
            teamsArr[teamIndex].sumBattingStrikeRate += p->strikeRate;
            teamsArr[teamIndex].countBattingStrikeRate++;
        }
    }

    teamsInitialized = 1;
}

// team index by binary search
int teamIndexById(int teamId) {
    int low = 0, high = teamCount - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        if (teamsArr[mid].teamId == teamId) return mid;
        if (teamsArr[mid].teamId < teamId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// Unique ID check
int isPlayerIdUnique(int pid) {
    for (int t = 0; t < teamCount; t++) {
        for (int r = 0; r < 3; r++) {
            PlayerNode *p = teamsArr[t].roleHeads[r];
            while (p) {
                if (p->id == pid) return 0;
                p = p->next;
            }
        }
    }
    return 1;
}

// Add player to team
void addPlayerToTeam(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    int teamIndex = teamIndexById(teamId);

    if (teamIndex == -1) {
        printf("Team not found.\n");
        return;
    }

    Team *tm = &teamsArr[teamIndex];

    if (tm->totalPlayers >= 50) {
        printf("Team %s already has maximum permitted players (50).\n", tm->name);
        return;
    }

    int pid = readInt("Enter Player ID: ", 1, 1500);

    if (!isPlayerIdUnique(pid)) {
        printf("Player ID already exists.\n");
        return;
    }

    char pname[51];
while (1) {
    readString("Name: ", pname, 50);

    int valid = 1;
    int len = strlen(pname);

    // name cannot be empty
    if (len == 0) {
        printf("Name cannot be empty.\n");
        valid = 0;
    }

    // first character must be alphabet
    else if (!isalpha(pname[0])) {
        printf("Name must start with an alphabet.\n");
        valid = 0;
    }

    // check each character
    else {
        for (int i = 0; i < len; i++) {
            if (!isalpha(pname[i]) && pname[i] != ' ') {
                valid = 0;
                printf("Name can only contain alphabets and spaces.\n");
                break;
            }
        }
    }

    if (valid)
        break;
}


    int roleIn = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int roleIndex = roleIn - 1;

    int runs = readInt("Total Runs: ", 0, 1000000);
    double battAvg = readDouble("Batting Average: ", 0.0, 10000.0);
    double strikeRate = readDouble("Strike Rate: ", 0.0, 10000.0);
    int wickets = readInt("Wickets: ", 0, 1000000);
    double economyRate = readDouble("Economy Rate: ", 0.0, 10000.0);

    PlayerNode *node = makePlayerNode(pid, pname, roleIndex, runs, battAvg, strikeRate, wickets, economyRate);

    insertSortedDesc(&tm->roleHeads[roleIndex], node);

    tm->totalPlayers++;
    if (roleIndex == 0 || roleIndex == 2) {
        tm->sumBattingStrikeRate += strikeRate;
        tm->countBattingStrikeRate++;
    }

    printf("Player added successfully to Team %s!\n", tm->name);
}

// Team avg SR
double teamAvgBattingSR(const Team *team) {
    if (team->countBattingStrikeRate == 0) return 0.0;
    return team->sumBattingStrikeRate / team->countBattingStrikeRate;
}

// Display players of a team
void displayPlayersOfTeam(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    int teamIndex = teamIndexById(teamId);
    if (teamIndex == -1) { printf("Team not found.\n"); return; }

    Team *tm = &teamsArr[teamIndex];

    printf("\nPlayers of Team %s:\n", tm->name);
    printf("====================================================================================\n");
    printf("ID   %-20s %-12s %-6s %-5s %-5s %-5s %-5s %s\n",
           "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");

    for (int r = 0; r < 3; r++) {
        PlayerNode *p = tm->roleHeads[r];
        while (p) {
            printf("%-4d %-20s %-12s %-6d %-5.1f %-5.1f %-5d %-5.1f %.2f\n",
                   p->id, p->name, roleIndexToStr(p->role),
                   p->totalRuns, p->battingAverage,
                   p->strikeRate, p->wickets, p->economyRate, p->perfIndex);

            p = p->next;
        }
    }

    printf("====================================================================================\n");
    printf("Total Players: %d\n", tm->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamAvgBattingSR(tm));
}

// Display teams sorted by avg strike rate
void displayTeamsByAvgSR(void) {
    Team *arr[20];
    for (int i = 0; i < teamCount; i++) arr[i] = &teamsArr[i];

    for (int i = 0; i < teamCount - 1; i++) {
        int best = i;
        for (int j = i + 1; j < teamCount; j++) {
            if (teamAvgBattingSR(arr[j]) > teamAvgBattingSR(arr[best]))
                best = j;
        }
        Team *tmp = arr[i];
        arr[i] = arr[best];
        arr[best] = tmp;
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID  %-25s %-12s %s\n", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");

    for (int i = 0; i < teamCount; i++) {
        printf("%-3d %-25s %-12.2f %d\n",
               arr[i]->teamId, arr[i]->name,
               teamAvgBattingSR(arr[i]), arr[i]->totalPlayers);
    }

    printf("=========================================================\n");
}

// Display top K players
void displayTopKOfTeamByRole(void) {
    int teamId = readInt("Enter Team ID: ", 1, teamCount);
    int teamIndex = teamIndexById(teamId);
    if (teamIndex == -1) { printf("Team not found.\n"); return; }

    Team *tm = &teamsArr[teamIndex];

    int roleIn = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int roleIndex = roleIn - 1;

    PlayerNode *p = tm->roleHeads[roleIndex];
    if (!p) { printf("No players of that role in this team.\n"); return; }

    int available = 0;
    PlayerNode *tmp = p;
    while (tmp) { available++; tmp = tmp->next; }

    int k = readInt("Enter number of players: ", 1, available);

    printf("\nTop %d %s of Team %s:\n", k, roleIndexToStr(roleIndex), tm->name);
    printf("====================================================================================\n");
    printf("ID   %-20s %-12s %-6s %-5s %-5s %-5s %-5s %s\n",
           "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");

    int printed = 0;
    p = tm->roleHeads[roleIndex];

    while (p && printed < k) {
        printf("%-4d %-20s %-12s %-6d %-5.1f %-5.1f %-5d %-5.1f %.2f\n",
               p->id, p->name, roleIndexToStr(p->role),
               p->totalRuns, p->battingAverage, p->strikeRate,
               p->wickets, p->economyRate, p->perfIndex);

        p = p->next;
        printed++;
    }

    printf("====================================================================================\n");
}

// Heap structures and functions
struct HeapItem {
    PlayerNode *node;
    int teamIndex;
};

struct MaxHeap {
    HeapItem *items;
    int size;
    int capacity;
};

MaxHeap *heapCreate(int capacity) {
    MaxHeap *h = malloc(sizeof(MaxHeap));
    h->items = malloc(sizeof(HeapItem) * capacity);
    h->size = 0;
    h->capacity = capacity;
    return h;
}

void heapSwap(HeapItem *a, HeapItem *b) {
    HeapItem t = *a;
    *a = *b;
    *b = t;
}

void heapFree(MaxHeap *h) {
    if (!h) return;
    free(h->items);
    free(h);
}

void heapPush(MaxHeap *h, PlayerNode *node, int teamIndex) {
    if (h->size >= h->capacity) return;

    int i = h->size++;
    h->items[i].node = node;
    h->items[i].teamIndex = teamIndex;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->items[parent].node->perfIndex >= h->items[i].node->perfIndex)
            break;

        heapSwap(&h->items[parent], &h->items[i]);
        i = parent;
    }
}

HeapItem heapPop(MaxHeap *h) {
    HeapItem out = h->items[0];

    h->items[0] = h->items[--h->size];

    int i = 0;

    while (1) {
        int l = 2*i + 1, r = 2*i + 2, largest = i;

        if (l < h->size && h->items[l].node->perfIndex > h->items[largest].node->perfIndex)
            largest = l;

        if (r < h->size && h->items[r].node->perfIndex > h->items[largest].node->perfIndex)
            largest = r;

        if (largest == i) break;

        heapSwap(&h->items[i], &h->items[largest]);
        i = largest;
    }

    return out;
}

// Display all players of a role globally
void displayAllPlayersOfRoleGlobal(void) {
    int roleIn = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int roleIndex = roleIn - 1;

    MaxHeap *heap = heapCreate(teamCount + 5);

    for (int t = 0; t < teamCount; t++) {
        PlayerNode *head = teamsArr[t].roleHeads[roleIndex];
        if (head)
            heapPush(heap, head, t);
    }

    if (heap->size == 0) {
        printf("No players of that role found.\n");
        heapFree(heap);
        return;
    }

    printf("\nAll %s of all teams:\n", roleIndexToStr(roleIndex));
    printf("======================================================================================\n");
    printf("ID   %-20s %-15s %-12s %-6s %-5s %-5s %-5s %-5s %s\n",
           "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("======================================================================================\n");

    while (heap->size > 0) {
        HeapItem it = heapPop(heap);

        PlayerNode *node = it.node;
        int teamIndexLocal = it.teamIndex;

        printf("%-4d %-20s %-15s %-12s %-6d %-5.1f %-5.1f %-5d %-5.1f %.2f\n",
               node->id, node->name,
               teamsArr[teamIndexLocal].name, roleIndexToStr(node->role),
               node->totalRuns, node->battingAverage, node->strikeRate,
               node->wickets, node->economyRate, node->perfIndex);

        if (node->next)
            heapPush(heap, node->next, teamIndexLocal);
    }

    printf("======================================================================================\n");

    heapFree(heap);
}

// Free all memory
void freeAll(void) {
    for (int t = 0; t < teamCount; t++) {
        for (int r = 0; r < 3; r++) {
            PlayerNode *p = teamsArr[t].roleHeads[r];
            while (p) {
                PlayerNode *n = p->next;
                free(p);
                p = n;
            }
            teamsArr[t].roleHeads[r] = NULL;
        }
    }
}
