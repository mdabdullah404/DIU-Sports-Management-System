#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Team {
    char teamName[50];
    char sportName[20];
    struct Team *next;
};

struct Player {
    char playerName[50];
    int jerseyNumber;
    char teamName[50];
    char sportName[20];
    struct Player *next;
};

struct Match {
    int matchID;
    char sportName[20];
    char teamA[50];
    char teamB[50];
    char date[15];
    char venue[50];
    int scoreA, scoreB;
    char status[15];
    struct Match *next;
};

struct HistoryNode {
    int actionType;
    int entityType;
    struct Team *savedTeam;
    struct Player *savedPlayer;
    struct Match *savedMatch;
    char lookupIdentifierStr[50];
    int lookupIdentifierInt;
    struct HistoryNode *next;
};

struct Team *teamHead = NULL;
struct Player *playerHead = NULL;
struct Match *matchHead = NULL;
struct HistoryNode *historyHead = NULL;

void mainMenu();
void viewerInterface();
void managementInterface();
void teamManagementMenu();
void playerManagementMenu();
void matchManagementMenu();
void viewSports();
void searchTeamMembers();
void searchIndividualPlayer();
void undoLastAction();
void freeAllMemory();
void pushHistory(int action, int entity, struct Team *t, struct Player *p, struct Match *m);

void saveDataToFile();
void loadDataFromFile();

int strCaseEquals(const char *s1, const char *s2);
int isValidSport(const char *sport);

int strCaseEquals(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 0;
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

int isValidSport(const char *sport) {
    if (strCaseEquals(sport, "Football") ||
        strCaseEquals(sport, "Cricket") ||
        strCaseEquals(sport, "Badminton") ||
        strCaseEquals(sport, "Table Tennis") ||
        strCaseEquals(sport, "Basketball")) {
        return 1;
    }
    return 0;
}

int main() {
    loadDataFromFile();

    printf("========================================\n");
    printf("   WELCOME TO DIU SPORTS MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    
    mainMenu();

    saveDataToFile();
    freeAllMemory();
    return 0;
}

void saveDataToFile() {
    FILE *ft = fopen("teams.dat", "wb");
    if (ft) {
        struct Team *curr = teamHead;
        while (curr) {
            fwrite(curr, sizeof(struct Team), 1, ft);
            curr = curr->next;
        }
        fclose(ft);
    }

    FILE *fp = fopen("players.dat", "wb");
    if (fp) {
        struct Player *curr = playerHead;
        while (curr) {
            fwrite(curr, sizeof(struct Player), 1, fp);
            curr = curr->next;
        }
        fclose(fp);
    }

    FILE *fm = fopen("matches.dat", "wb");
    if (fm) {
        struct Match *curr = matchHead;
        while (curr) {
            fwrite(curr, sizeof(struct Match), 1, fm);
            curr = curr->next;
        }
        fclose(fm);
    }
    printf("\n[SUCCESS] All data has been saved to files.\n");
}

void loadDataFromFile() {
    FILE *ft = fopen("teams.dat", "rb");
    if (ft) {
        struct Team temp;
        struct Team *tail = NULL;
        while (fread(&temp, sizeof(struct Team), 1, ft)) {
            struct Team *newNode = (struct Team *)malloc(sizeof(struct Team));
            *newNode = temp;
            newNode->next = NULL;
            if (!teamHead) teamHead = newNode;
            else tail->next = newNode;
            tail = newNode;
        }
        fclose(ft);
    }

    FILE *fp = fopen("players.dat", "rb");
    if (fp) {
        struct Player temp;
        struct Player *tail = NULL;
        while (fread(&temp, sizeof(struct Player), 1, fp)) {
            struct Player *newNode = (struct Player *)malloc(sizeof(struct Player));
            *newNode = temp;
            newNode->next = NULL;
            if (!playerHead) playerHead = newNode;
            else tail->next = newNode;
            tail = newNode;
        }
        fclose(fp);
    }

    FILE *fm = fopen("matches.dat", "rb");
    if (fm) {
        struct Match temp;
        struct Match *tail = NULL;
        while (fread(&temp, sizeof(struct Match), 1, fm)) {
            struct Match *newNode = (struct Match *)malloc(sizeof(struct Match));
            *newNode = temp;
            newNode->next = NULL;
            if (!matchHead) matchHead = newNode;
            else tail->next = newNode;
            tail = newNode;
        }
        fclose(fm);
    }
}

void pushHistory(int action, int entity, struct Team *t, struct Player *p, struct Match *m) {
    struct HistoryNode *newNode = (struct HistoryNode *)malloc(sizeof(struct HistoryNode));
    if (!newNode) return;
    
    newNode->actionType = action;
    newNode->entityType = entity;
    newNode->savedTeam = t;
    newNode->savedPlayer = p;
    newNode->savedMatch = m;
    newNode->lookupIdentifierInt = 0;
    newNode->lookupIdentifierStr[0] = '\0';
    newNode->next = historyHead;
    historyHead = newNode;
}

void undoLastAction() {
    if (historyHead == NULL) {
        printf("\n[!] No actions available to undo.\n");
        return;
    }
    
    struct HistoryNode *top = historyHead;
    historyHead = historyHead->next;

    if (top->actionType == 1) {
        if (top->entityType == 1) {
            top->savedTeam->next = teamHead;
            teamHead = top->savedTeam;
        } else if (top->entityType == 2) {
            top->savedPlayer->next = playerHead;
            playerHead = top->savedPlayer;
        } else if (top->entityType == 3) {
            top->savedMatch->next = matchHead;
            matchHead = top->savedMatch;
        }
        printf("\n[SUCCESS] Deleted item successfully restored.\n");
    } 
    else if (top->actionType == 2) {
        if (top->entityType == 1) {
            struct Team *curr = teamHead;
            while (curr != NULL) {
                if (strcmp(curr->teamName, top->lookupIdentifierStr) == 0) {
                    struct Team *nextPtr = curr->next;
                    *curr = *(top->savedTeam);
                    curr->next = nextPtr;
                    free(top->savedTeam);
                    break;
                }
                curr = curr->next;
            }
        } 
        else if (top->entityType == 2) {
            struct Player *curr = playerHead;
            while (curr != NULL) {
                if (strcmp(curr->playerName, top->lookupIdentifierStr) == 0) {
                    struct Player *nextPtr = curr->next;
                    *curr = *(top->savedPlayer);
                    curr->next = nextPtr;
                    free(top->savedPlayer);
                    break;
                }
                curr = curr->next;
            }
        } 
        else if (top->entityType == 3) {
            struct Match *curr = matchHead;
            while (curr != NULL) {
                if (curr->matchID == top->lookupIdentifierInt) {
                    struct Match *nextPtr = curr->next;
                    *curr = *(top->savedMatch);
                    curr->next = nextPtr;
                    free(top->savedMatch);
                    break;
                }
                curr = curr->next;
            }
        }
        printf("\n[SUCCESS] Modification successfully reverted.\n");
    }
    free(top);
}

void addTeam() {
    char tempSport[20];
    printf("Enter Sport Name: ");
    scanf(" %[^\n]", tempSport);

    if (!isValidSport(tempSport)) {
        printf("\n[!] This sport is not available in this app.\n");
        return;
    }

    struct Team *newTeam = (struct Team *)malloc(sizeof(struct Team));
    if (!newTeam) return;

    printf("Enter Team Name: ");
    scanf(" %[^\n]", newTeam->teamName);
    strcpy(newTeam->sportName, tempSport);

    newTeam->next = teamHead;
    teamHead = newTeam;
    printf("\n[SUCCESS] Team added.\n");
}

void viewTeams() {
    printf("\n--- LIST OF TEAMS ---\n");
    struct Team *curr = teamHead;
    
    if (curr == NULL) {
        printf("No teams available.\n");
    }
    
    while (curr != NULL) {
        printf("Team: %s | Sport: %s\n", curr->teamName, curr->sportName);
        curr = curr->next;
    }
}

void modifyTeam() {
    char searchName[50];
    printf("Enter the exact name of the Team to modify: ");
    scanf(" %[^\n]", searchName);

    struct Team *curr = teamHead;
    while (curr != NULL) {
        if (strcmp(curr->teamName, searchName) == 0) {
            char tempSport[20];
            printf("Enter new Sport Name: ");
            scanf(" %[^\n]", tempSport);
            
            if (!isValidSport(tempSport)) {
                printf("\n[!] This sport is not available in this app.\n");
                return;
            }

            struct Team *backup = (struct Team *)malloc(sizeof(struct Team));
            *backup = *curr;
            backup->next = NULL;
            pushHistory(2, 1, backup, NULL, NULL);

            printf("Enter new Team Name: ");
            scanf(" %[^\n]", curr->teamName);
            strcpy(curr->sportName, tempSport);
            strcpy(historyHead->lookupIdentifierStr, curr->teamName);
            
            printf("\n[SUCCESS] Team modified.\n");
            return;
        }
        curr = curr->next;
    }
    printf("\n[!] Team not found.\n");
}

void deleteTeam() {
    char searchName[50];
    printf("Enter the exact name of the Team to delete: ");
    scanf(" %[^\n]", searchName);

    struct Team *curr = teamHead;
    struct Team *prev = NULL;
    
    while (curr != NULL) {
        if (strcmp(curr->teamName, searchName) == 0) {
            if (prev == NULL) {
                teamHead = curr->next;
            } else {
                prev->next = curr->next;
            }
            
            curr->next = NULL;
            pushHistory(1, 1, curr, NULL, NULL);
            printf("\n[SUCCESS] Team deleted.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("\n[!] Team not found.\n");
}

void teamManagementMenu() {
    int choice;
    do {
        printf("\n--- TEAM MANAGEMENT ---\n");
        printf("1. Add Team\n2. View Teams\n3. Modify Team\n4. Delete Team\n5. Back\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: addTeam(); break;
            case 2: viewTeams(); break;
            case 3: modifyTeam(); break;
            case 4: deleteTeam(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void addPlayer() {
    char tempSport[20];
    printf("Enter Sport Name: ");
    scanf(" %[^\n]", tempSport);

    if (!isValidSport(tempSport)) {
        printf("\n[!] This sport is not available in this app.\n");
        return;
    }

    struct Player *newPlayer = (struct Player *)malloc(sizeof(struct Player));
    if (!newPlayer) return;

    printf("Enter Player Name: ");
    scanf(" %[^\n]", newPlayer->playerName);
    printf("Enter Jersey Number: ");
    scanf("%d", &newPlayer->jerseyNumber);
    printf("Enter Team Name: ");
    scanf(" %[^\n]", newPlayer->teamName);
    strcpy(newPlayer->sportName, tempSport);

    newPlayer->next = playerHead;
    playerHead = newPlayer;
    printf("\n[SUCCESS] Player added.\n");
}

void viewPlayers() {
    printf("\n--- LIST OF PLAYERS ---\n");
    struct Player *curr = playerHead;
    
    if (curr == NULL) {
        printf("No players available.\n");
    }
    
    while (curr != NULL) {
        printf("Name: %s | Jersey: %d | Team: %s | Sport: %s\n",
               curr->playerName, curr->jerseyNumber, curr->teamName, curr->sportName);
        curr = curr->next;
    }
}

void modifyPlayer() {
    char searchName[50];
    printf("Enter Player Name to modify: ");
    scanf(" %[^\n]", searchName);

    struct Player *curr = playerHead;
    while (curr != NULL) {
        if (strcmp(curr->playerName, searchName) == 0) {
            char tempSport[20];
            printf("Enter new Sport Name: ");
            scanf(" %[^\n]", tempSport);
            
            if (!isValidSport(tempSport)) {
                printf("\n[!] This sport is not available in this app.\n");
                return;
            }

            struct Player *backup = (struct Player *)malloc(sizeof(struct Player));
            *backup = *curr;
            backup->next = NULL;
            pushHistory(2, 2, NULL, backup, NULL);

            printf("Enter new Player Name: ");
            scanf(" %[^\n]", curr->playerName);
            printf("Enter new Jersey Number: ");
            scanf("%d", &curr->jerseyNumber);
            printf("Enter new Team Name: ");
            scanf(" %[^\n]", curr->teamName);
            strcpy(curr->sportName, tempSport);
            strcpy(historyHead->lookupIdentifierStr, curr->playerName);
            
            printf("\n[SUCCESS] Player modified.\n");
            return;
        }
        curr = curr->next;
    }
    printf("\n[!] Player not found.\n");
}

void deletePlayer() {
    char searchName[50];
    printf("Enter Player Name to delete: ");
    scanf(" %[^\n]", searchName);

    struct Player *curr = playerHead;
    struct Player *prev = NULL;
    
    while (curr != NULL) {
        if (strcmp(curr->playerName, searchName) == 0) {
            if (prev == NULL) {
                playerHead = curr->next;
            } else {
                prev->next = curr->next;
            }

            curr->next = NULL;
            pushHistory(1, 2, NULL, curr, NULL);
            printf("\n[SUCCESS] Player deleted.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("\n[!] Player not found.\n");
}

void playerManagementMenu() {
    int choice;
    do {
        printf("\n--- PLAYER MANAGEMENT ---\n");
        printf("1. Add Player\n2. View Players\n3. Modify Player\n4. Delete Player\n5. Back\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: addPlayer(); break;
            case 2: viewPlayers(); break;
            case 3: modifyPlayer(); break;
            case 4: deletePlayer(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void addMatch() {
    char tempSport[20];
    printf("Enter Sport Name: ");
    scanf(" %[^\n]", tempSport);

    if (!isValidSport(tempSport)) {
        printf("\n[!] This sport is not available in this app.\n");
        return;
    }

    struct Match *newMatch = (struct Match *)malloc(sizeof(struct Match));
    if (!newMatch) return;

    printf("Enter Match ID (Number): ");
    scanf("%d", &newMatch->matchID);
    strcpy(newMatch->sportName, tempSport);
    printf("Enter Team A: ");
    scanf(" %[^\n]", newMatch->teamA);
    printf("Enter Team B: ");
    scanf(" %[^\n]", newMatch->teamB);
    printf("Enter Date (DD/MM/YYYY): ");
    scanf(" %[^\n]", newMatch->date);
    printf("Enter Venue: ");
    scanf(" %[^\n]", newMatch->venue);
    
    newMatch->scoreA = 0;
    newMatch->scoreB = 0;
    strcpy(newMatch->status, "Scheduled");

    newMatch->next = matchHead;
    matchHead = newMatch;
    printf("\n[SUCCESS] Match Scheduled.\n");
}

void viewMatches() {
    printf("\n--- MATCH FIXTURES & RESULTS ---\n");
    struct Match *curr = matchHead;
    
    if (curr == NULL) {
        printf("No matches available.\n");
    }
    
    while (curr != NULL) {
        printf("ID: %d | %s vs %s | %s | %s | Status: %s",
               curr->matchID, curr->teamA, curr->teamB, curr->date, curr->venue, curr->status);
        if (strcmp(curr->status, "Played") == 0) {
            printf(" | Score: %d - %d", curr->scoreA, curr->scoreB);
        }
        printf("\n");
        curr = curr->next;
    }
}

void modifyMatch() {
    int sID;
    printf("Enter Match ID to modify: ");
    scanf("%d", &sID);

    struct Match *curr = matchHead;
    while (curr != NULL) {
        if (curr->matchID == sID) {
            char tempSport[20];
            printf("Enter Sport: ");
            scanf(" %[^\n]", tempSport);
            
            if (!isValidSport(tempSport)) {
                printf("\n[!] This sport is not available in this app.\n");
                return;
            }

            struct Match *backup = (struct Match *)malloc(sizeof(struct Match));
            *backup = *curr;
            backup->next = NULL;
            pushHistory(2, 3, NULL, NULL, backup);

            printf("Enter new Match ID: ");
            scanf("%d", &curr->matchID);
            strcpy(curr->sportName, tempSport);
            printf("Enter Team A: ");
            scanf(" %[^\n]", curr->teamA);
            printf("Enter Team B: ");
            scanf(" %[^\n]", curr->teamB);
            printf("Enter Status (Scheduled / Played): ");
            scanf(" %[^\n]", curr->status);
            
            if (strcmp(curr->status, "Played") == 0) {
                printf("Enter Score for Team A: ");
                scanf("%d", &curr->scoreA);
                printf("Enter Score for Team B: ");
                scanf("%d", &curr->scoreB);
            }

            historyHead->lookupIdentifierInt = curr->matchID;
            printf("\n[SUCCESS] Match updated.\n");
            return;
        }
        curr = curr->next;
    }
    printf("\n[!] Match ID not found.\n");
}

void deleteMatch() {
    int sID;
    printf("Enter Match ID to delete: ");
    scanf("%d", &sID);

    struct Match *curr = matchHead;
    struct Match *prev = NULL;
    
    while (curr != NULL) {
        if (curr->matchID == sID) {
            if (prev == NULL) {
                matchHead = curr->next;
            } else {
                prev->next = curr->next;
            }

            curr->next = NULL;
            pushHistory(1, 3, NULL, NULL, curr);
            printf("\n[SUCCESS] Match deleted.\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("\n[!] Match not found.\n");
}

void matchManagementMenu() {
    int choice;
    do {
        printf("\n--- MATCH MANAGEMENT ---\n");
        printf("1. Add Match\n2. View Matches\n3. Modify Match\n4. Delete Match\n5. Back\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: addMatch(); break;
            case 2: viewMatches(); break;
            case 3: modifyMatch(); break;
            case 4: deleteMatch(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

void searchTeamMembers() {
    char searchTeam[50];
    printf("Enter Team Name to search players: ");
    scanf(" %[^\n]", searchTeam);

    printf("\n--- PLAYERS IN TEAM: %s ---\n", searchTeam);
    struct Player *curr = playerHead;
    int count = 0;
    
    while (curr != NULL) {
        if (strCaseEquals(curr->teamName, searchTeam)) {
            count++;
            printf("%d. Name: %s | Jersey: %d | Sport: %s\n",
                   count, curr->playerName, curr->jerseyNumber, curr->sportName);
        }
        curr = curr->next;
    }
    
    if (count == 0) {
        printf("No players found for team '%s'.\n", searchTeam);
    } else {
        printf("----------------------------------------\n");
        printf("Total Members: %d\n", count);
    }
}

void searchIndividualPlayer() {
    char searchName[50];
    printf("Enter Player Name to search: ");
    scanf(" %[^\n]", searchName);

    struct Player *curr = playerHead;
    int found = 0;

    printf("\n--- SEARCH RESULT ---\n");
    while (curr != NULL) {
        if (strCaseEquals(curr->playerName, searchName)) {
            printf("Name: %s\n", curr->playerName);
            printf("Jersey Number: %d\n", curr->jerseyNumber);
            printf("Team Name: %s\n", curr->teamName);
            printf("Sport: %s\n", curr->sportName);
            printf("------------------------\n");
            found = 1;
        }
        curr = curr->next;
    }

    if (!found) {
        printf("Player '%s' not found in the system.\n", searchName);
    }
}

void viewSports() {
    printf("\n---------- AVAILABLE SPORTS ----------\n");
    printf("1. Football\n2. Cricket\n3. Badminton\n4. Table Tennis\n5. Basketball\n");
    printf("---------------------------------------\n");
}

void viewerInterface() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("             VIEWER INTERFACE\n");
        printf("========================================\n");
        printf("1. View Sports\n");
        printf("2. View Teams\n");
        printf("3. View Players\n");
        printf("4. View Match Fixtures\n");
        printf("5. Search Team Members\n");
        printf("6. Search Individual Player\n");
        printf("7. Back to Main Menu\n");
        printf("----------------------------------------\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: viewSports(); break;
            case 2: viewTeams(); break;
            case 3: viewPlayers(); break;
            case 4: viewMatches(); break;
            case 5: searchTeamMembers(); break;
            case 6: searchIndividualPlayer(); break;
            case 7: printf("\nReturning to Main Menu...\n"); break;
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 7);
}

void managementInterface() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("         MANAGEMENT INTERFACE\n");
        printf("========================================\n");
        printf("1. Team Management\n");
        printf("2. Player Management\n");
        printf("3. Match Management\n");
        printf("4. Search Team Members\n");
        printf("5. Search Individual Player\n");
        printf("6. Undo Last Action\n");
        printf("7. Logout\n");
        printf("----------------------------------------\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: teamManagementMenu(); break;
            case 2: playerManagementMenu(); break;
            case 3: matchManagementMenu(); break;
            case 4: searchTeamMembers(); break;
            case 5: searchIndividualPlayer(); break;
            case 6: undoLastAction(); break;
            case 7: printf("\nLogging out...\n"); break;
            default: printf("\nInvalid choice!\n");
        }
    } while (choice != 7);
}

void mainMenu() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("              MAIN MENU\n");
        printf("========================================\n");
        printf("1. Viewer Interface\n");
        printf("2. Management Interface\n");
        printf("3. Save & Exit\n");
        printf("----------------------------------------\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); choice = -1;
        }
        switch (choice) {
            case 1: viewerInterface(); break;
            case 2: managementInterface(); break;
            case 3: printf("\nExiting program and saving data... Thank you!\n"); break;
            default: printf("\nInvalid choice! Please enter 1, 2, or 3.\n");
        }
    } while (choice != 3);
}

void freeAllMemory() {
    struct Team *t;
    while (teamHead != NULL) {
        t = teamHead;
        teamHead = teamHead->next;
        free(t);
    }
    struct Player *p;
    while (playerHead != NULL) {
        p = playerHead;
        playerHead = playerHead->next;
        free(p);
    }
    struct Match *m;
    while (matchHead != NULL) {
        m = matchHead;
        matchHead = matchHead->next;
        free(m);
    }
    struct HistoryNode *h;
    while (historyHead != NULL) {
        h = historyHead;
        historyHead = historyHead->next;
        if (h->savedTeam) free(h->savedTeam);
        if (h->savedPlayer) free(h->savedPlayer);
        if (h->savedMatch) free(h->savedMatch);
        free(h);
    }
}