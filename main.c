#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// ================= CONFIGURATION =================
#define ADMIN_PASS "admin123"
#define CANDIDATE_FILE "candidates.bin"
#define VOTER_FILE "voters.bin"
#define DATASET_SIZE 100 

// ================= DATA STRUCTURES =================
typedef struct {
    char voter_id[15];      
    char name[50];          
    int age;                
    char constituency[30];  
    int has_voted;          
} Voter;

typedef struct {
    int id;
    char name[50];
    char party[20];
    int vote_count;
} Candidate;

// ================= FUNCTION PROTOTYPES =================
void clearScreen();
void initSystem();
void adminMenu();
void votingProcess();
void displayResults();
void viewVoterList(); 
void searchVoter(); 
void resetSystem();
void markVoterAsVoted(long fileOffset);
void incrementVote(int candidateID);

// ================= MAIN SYSTEM =================
int main() {
    srand(time(0)); 
    initSystem();   

    int choice;
    while(1) {
        clearScreen();
        printf("================================================\n");
        printf("      NATIONAL DIGITAL POLLING STATION          \n");
        printf("      [ DATABASE SIZE: %d CITIZENS ]            \n", DATASET_SIZE);
        printf("================================================\n");
        printf("1. PROCEED TO VOTE (Voter Interface)\n");
        printf("2. ELECTION COMMISSION LOGIN (Admin)\n");
        printf("3. SHUTDOWN SYSTEM\n");
        printf("------------------------------------------------\n");
        printf("ENTER OPTION: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); continue;
        }

        switch(choice) {
            case 1: votingProcess(); break;
            case 2: adminMenu(); break;
            case 3: exit(0);
            default: printf("Invalid selection.\n");
        }
    }
    return 0;
}

// ================= LARGE DATASET GENERATOR =================
void initSystem() {
    FILE *fc = fopen(CANDIDATE_FILE, "rb");
    if (fc == NULL) {
        fc = fopen(CANDIDATE_FILE, "wb");
        Candidate c[] = {
            {1, "Narendra Modi",    "BJP", 0},
            {2, "Rahul Gandhi",     "INC", 0},
            {3, "Arvind Kejriwal",  "AAP", 0},
            {4, "Mamata Banerjee",  "TMC", 0},
            {5, "NOTA",             "None",0}
        };
        fwrite(c, sizeof(Candidate), 5, fc);
        fclose(fc);
    } else fclose(fc);

    FILE *fv = fopen(VOTER_FILE, "rb");
    if (fv == NULL) {
        fv = fopen(VOTER_FILE, "wb");
        char *fnames[] = {"Amit", "Priya", "Rahul", "Sita", "John", "Mohammed", "Anita", "Vikram", "Suresh", "Riya", "Kiran", "Deepak"};
        char *lnames[] = {"Sharma", "Verma", "Singh", "Patel", "Khan", "Reddy", "Nair", "Das", "Yadav", "Gupta", "Rao", "Mishra"};
        char *wards[]  = {"North Delhi", "South Bombay", "Bangalore East", "Chennai Central", "Hyderabad Old"};

        for(int i = 0; i < DATASET_SIZE; i++) {
            Voter v;
            sprintf(v.voter_id, "IND-2024-%03d", 100 + i); 
            sprintf(v.name, "%s %s", fnames[rand() % 12], lnames[rand() % 12]);
            v.age = 17 + (rand() % 74); 
            strcpy(v.constituency, wards[rand() % 5]);
            v.has_voted = 0;
            fwrite(&v, sizeof(Voter), 1, fv);
        }
        fclose(fv);
    } else fclose(fv);
}

// ================= VOTING LOGIC =================
void votingProcess() {
    char inputID[15];
    long fileOffset = -1;
    Voter currentVoter;
    int found = 0;

    clearScreen();
    printf("--- VOTER AUTHENTICATION ---\n");
    printf("Sample IDs: IND-2024-100, IND-2024-105\n");
    printf("Enter Voter ID: ");
    scanf("%s", inputID);

    FILE *fp = fopen(VOTER_FILE, "rb");
    while(fread(&currentVoter, sizeof(Voter), 1, fp)) {
        if(strcmp(currentVoter.voter_id, inputID) == 0) {
            fileOffset = ftell(fp) - sizeof(Voter); 
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\n[ERROR] ID NOT FOUND.\nPress Enter..."); getchar(); getchar(); return;
    }

    printf("\n--- IDENTITY VERIFIED ---\nName: %s\nAge: %d\nConstituency: %s\n", currentVoter.name, currentVoter.age, currentVoter.constituency);
    
    if (currentVoter.age < 18) {
        printf("\n[REJECTED] Underage.\nPress Enter..."); getchar(); getchar(); return;
    }
    if (currentVoter.has_voted) {
        printf("\n[ALERT] ALREADY VOTED.\nPress Enter..."); getchar(); getchar(); return;
    }

    printf("\nPress Enter to Vote..."); getchar(); getchar();

    clearScreen();
    printf("--- EVM INTERFACE ---\n\n");
    fp = fopen(CANDIDATE_FILE, "rb");
    Candidate c;
    while(fread(&c, sizeof(Candidate), 1, fp)) {
        printf("[%d] %-20s (%s)\n", c.id, c.name, c.party);
    }
    fclose(fp);

    int choice;
    printf("\nPress Button (Enter ID): ");
    scanf("%d", &choice);

    int valid = 0;
    fp = fopen(CANDIDATE_FILE, "rb");
    while(fread(&c, sizeof(Candidate), 1, fp)) {
        if(c.id == choice) { valid = 1; break; }
    }
    fclose(fp);

    if (valid) {
        incrementVote(choice);
        markVoterAsVoted(fileOffset);
        printf("\n[BEEP] VOTE RECORDED.\n");
    } else {
        printf("\n[ERROR] Invalid Button.\n");
    }
    getchar(); getchar();
}

// ================= ADMIN LOGIC =================
void adminMenu() {
    char pwd[20];
    printf("\n[ADMIN] Enter Passcode: ");
    scanf("%s", pwd);

    if(strcmp(pwd, ADMIN_PASS) != 0) {
        printf("Invalid Passcode.\n"); getchar(); getchar(); return;
    }

    int choice;
    do {
        clearScreen();
        printf("--- ADMIN PANEL ---\n1. View Results\n2. View Voter Database\n3. Search Voter\n4. Reset Election\n5. Back\nChoice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: displayResults(); break;
            case 2: viewVoterList(); break;
            case 3: searchVoter(); break;
            case 4: resetSystem(); break;
            case 5: return;
        }
    } while(choice != 5);
}

void viewVoterList() {
    clearScreen();
    printf("--- REGISTERED CITIZENS ---\n");
    FILE *fp = fopen(VOTER_FILE, "rb");
    Voter v;
    int count = 0;
    while(fread(&v, sizeof(Voter), 1, fp)) {
        printf("%-15s | %-20s | %d | %s\n", v.voter_id, v.name, v.age, v.has_voted ? "VOTED" : " -- ");
        if(++count % 20 == 0) { printf("--- Press Enter ---\n"); getchar(); getchar(); }
    }
    fclose(fp);
    printf("\nEnd.\nPress Enter..."); getchar(); getchar();
}

void searchVoter() {
    char searchID[15];
    printf("\nEnter Voter ID: ");
    scanf("%s", searchID);
    FILE *fp = fopen(VOTER_FILE, "rb");
    Voter v;
    int found = 0;
    while(fread(&v, sizeof(Voter), 1, fp)) {
        if(strcmp(v.voter_id, searchID) == 0) {
            printf("\nFOUND: %s, %s, Age: %d\n", v.voter_id, v.name, v.age);
            found = 1; break;
        }
    }
    fclose(fp);
    if(!found) printf("Not found.\n");
    getchar(); getchar();
}

void displayResults() {
    clearScreen();
    printf("--- RESULTS ---\n");
    FILE *fp = fopen(CANDIDATE_FILE, "rb");
    Candidate c;
    while(fread(&c, sizeof(Candidate), 1, fp)) {
        printf("%s (%s): %d\n", c.name, c.party, c.vote_count);
    }
    fclose(fp);
    printf("\nPress Enter..."); getchar(); getchar();
}

void markVoterAsVoted(long fileOffset) {
    FILE *fp = fopen(VOTER_FILE, "rb+");
    Voter v;
    fseek(fp, fileOffset, SEEK_SET);
    fread(&v, sizeof(Voter), 1, fp);
    v.has_voted = 1;
    fseek(fp, fileOffset, SEEK_SET);
    fwrite(&v, sizeof(Voter), 1, fp);
    fclose(fp);
}

void incrementVote(int candidateID) {
    FILE *fp = fopen(CANDIDATE_FILE, "rb+");
    Candidate c;
    while(fread(&c, sizeof(Candidate), 1, fp)) {
        if(c.id == candidateID) {
            c.vote_count++;
            fseek(fp, -sizeof(Candidate), SEEK_CUR);
            fwrite(&c, sizeof(Candidate), 1, fp);
            break;
        }
    }
    fclose(fp);
}

void resetSystem() {
    char c;
    printf("\nWipe all data? (y/n): ");
    scanf(" %c", &c);
    if(c=='y') {
        remove(CANDIDATE_FILE);
        remove(VOTER_FILE);
        initSystem();
        printf("Reset complete.\n"); getchar(); getchar();
    }
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
