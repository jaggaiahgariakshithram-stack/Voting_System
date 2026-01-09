#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<conio.h>
struct Candidate {
 int id;
 char name[50];
 int votes;
 struct Candidate *next;
};
struct Voter {
 int id;
 int hasVoted;
 struct Voter *next;
};
struct Candidate *candidateHead = NULL;
struct Voter *voterHead = NULL;
int candidateCount = 0;
int voterCount = 0;
void getMaskedInput(char *password) {
 int i = 0;
 char ch;
 while ((ch = getch()) != 13) { // 13 = ENTER key
 if (ch == 8) { // backspace
 if (i > 0) {
 i--;
 printf("\b \b");
 }
 } else {
 password[i++] = ch;
 printf("*");
 }
 }
 password[i] = '\0';
 printf("\n");
}
void initializePassword(){
 FILE *fp = fopen("admin_pass.txt", "r");
 if (fp == NULL) {
 fp = fopen("admin_pass.txt", "w");
 fprintf(fp, "admin123");
 fclose(fp);
 return;
 }
 fclose(fp);
}
void getAdminPassword(char *password) {
 FILE *fp = fopen("admin_pass.txt", "r");
 // If file missing or empty → create default password
 if (fp == NULL) {
 fp = fopen("admin_pass.txt", "w");
 fprintf(fp, "admin123");
 fclose(fp);
 strcpy(password, "admin123");
 return;
 }
 if (fscanf(fp, "%s", password) != 1) {
 // File exists but no valid password → reset it
 fclose(fp);
 fp = fopen("admin_pass.txt", "w");
 fprintf(fp, "admin123");
 fclose(fp);
 strcpy(password, "admin123");
 return;
 }
 fclose(fp);
}
int adminLogin() {
 char storedPass[50];
 char enteredPass[50];
 getAdminPassword(storedPass);
 int attempts = 3;
 while (attempts > 0) {
 printf("Enter admin password: ");
 getMaskedInput(enteredPass);
 if (strcmp(storedPass, enteredPass) == 0) {
 printf("Login successful!\n");
 return 1; // success
 } else {
 attempts--;
 printf("Incorrect password! Attempts left: %d\n", attempts);
 }
 }
 printf("Too many failed attempts! Access denied.\n");
 return 0; // failed login
}
int getCandidateArray(struct Candidate **arr) {
 int count = 0;
 struct Candidate *temp = candidateHead;
 while (temp != NULL) {
 arr[count++] = temp;
 temp = temp->next;
 }
 return count;
}
void sortCandidatesByVotes(struct Candidate **arr, int n) {
 for (int i = 0; i < n - 1; i++) {
 for (int j = 0; j < n - i - 1; j++) {
 if (arr[j]->votes < arr[j + 1]->votes) {
 struct Candidate *t = arr[j];
 arr[j] = arr[j + 1];
 arr[j + 1] = t;
 }
 }
 }
}
void sortCandidatesById(struct Candidate **arr, int n) {
 for (int i = 0; i < n - 1; i++) {
 for (int j = 0; j < n - i - 1; j++) {
 if (arr[j]->id > arr[j + 1]->id) {
 struct Candidate *t = arr[j];
 arr[j] = arr[j + 1];
 arr[j + 1] = t;
 }
 }
 }
}
// ---------------- FILE HANDLING -------------------
void saveCandidates() {
 FILE *fp = fopen("candidates.txt", "w");
 struct Candidate *temp = candidateHead;
 while (temp != NULL) {
 fprintf(fp, "%d %s %d\n", temp->id, temp->name, temp->votes);
 temp = temp->next;
 }
 fclose(fp);
}
void loadCandidates() {
 FILE *fp = fopen("candidates.txt", "r");
 if (!fp) return;
 while (!feof(fp)) {
 struct Candidate *c = (struct Candidate*) malloc(sizeof(struct Candidate));
 if (fscanf(fp, "%d %s %d", &c->id, c->name, &c->votes) == 3) {
 c->next = candidateHead;
 candidateHead = c;
 candidateCount++;
 }
 }
 fclose(fp);
}
void saveVoters() {
 FILE *fp = fopen("voters.txt", "w");
 struct Voter *temp = voterHead;
 while (temp != NULL) {
 fprintf(fp, "%d %d\n", temp->id, temp->hasVoted);
 temp = temp->next;
 }
 fclose(fp);
}
void loadVoters() {
 FILE *fp = fopen("voters.txt", "r");
 if (!fp) return;
 while (!feof(fp)) {
 struct Voter *v = (struct Voter*) malloc(sizeof(struct Voter));
 if (fscanf(fp, "%d %d", &v->id, &v->hasVoted) == 2) {
 v->next = voterHead;
 voterHead = v;
 voterCount++;
 }
 }
 fclose(fp);
}
void saveResults() {
 if(candidateHead == NULL) {
 printf("No candidates available to save results.\n");
 return;
 }
 struct Candidate *arr[100];
 int n = getCandidateArray(arr);
 sortCandidatesByVotes(arr, n);
 FILE *fp = fopen("results.txt", "w");
 fprintf(fp, "-------- FINAL RESULTS --------\n");
 for(int i = 0; i < n; i++) {
 fprintf(fp, "ID: %d | Name: %s | Votes: %d\n", arr[i]->id, arr[i]->name, arr[i]->votes);
 }
 fclose(fp);
 printf("Results saved to results.txt\n");
}
// ---------------- ADMIN FUNCTIONS -------------------
int isCandidateExists(char name[]) {
 struct Candidate *temp = candidateHead;
 while (temp != NULL) {
 if(strcmp(temp->name,name) == 0)
 return 1;
 temp = temp->next;
 }
 return 0;
}
void addCandidate() {
 char name[50];
 printf("Enter candidate name: ");
 scanf("%s", name);
 if(isCandidateExists(name)) {
 printf("Candidate with name %s already exists!\n", name);
 return;
 }
 struct Candidate *c = (struct Candidate*) malloc(sizeof(struct Candidate));
 strcpy(c->name, name);
 c->id = ++candidateCount;
 c->votes = 0;
 c->next = NULL;
 if(candidateHead == NULL) {
 candidateHead = c;
 } else {
 struct Candidate *temp = candidateHead;
 while (temp->next != NULL) {
 temp = temp->next;
 }
 temp->next = c;
 }
 saveCandidates();
 printf("Candidate added successfully!\n");
}
void removeCandidate() {
 if(candidateHead == NULL) {
 printf("No candidates available to remove.\n");
 return;
 }
 int id;
 printf("Enter candidate ID to remove: ");
 scanf("%d", &id);
 struct Candidate *temp = candidateHead;
 struct Candidate *prev = NULL;
 while (temp != NULL && temp->id != id) {
 prev = temp;
 temp = temp->next;
 }
 if (temp == NULL) {
 printf("Candidate not found!\n");
 return;
 }
 if (prev == NULL)
 candidateHead = temp->next;
 else
 prev->next = temp->next;
 free(temp);
 printf("Candidate removed.\n");
 saveCandidates();
}
void viewCandidates() {
 struct Candidate *temp = candidateHead;
 printf("\n--- Candidate List ---\n");
 printf("ID\tName\t\tVotes\n");
 while (temp != NULL) {
 printf("%d\t%-15s\t%d\n", temp->id, temp->name, temp->votes);
 temp = temp->next;
 }
}
void generateVoters() {
 if(voterHead != NULL) {
 printf("Voters already generated.\n");
 return;
 }
 printf("Enter number of voters: ");
 scanf("%d", &voterCount);
 for (int i = 0; i < voterCount; i++) {
 struct Voter *v = (struct Voter*) malloc(sizeof(struct Voter));
 v->id = 10000 + i;
 v->hasVoted = 0;
 v->next = NULL;
 if(voterHead == NULL) {
 voterHead = v;
 } else {
 struct Voter *temp = voterHead;
 while (temp->next != NULL) {
 temp = temp->next;
 }
 temp->next = v;
 }
 }
 saveVoters();
 printf("Voters generated successfully.\n");
}
void viewVoters() {
 if(voterHead == NULL) {
 printf("No voters available.\n");
 return;
 }
 struct Voter *temp = voterHead;
 printf("\n--- Voter List ---\n");
 while (temp != NULL) {
 printf("Voter ID: %d | Voted: %s\n",
 temp->id, temp->hasVoted ? "YES" : "NO");
 temp = temp->next;
 }
}
void showResults() {
 if(candidateHead == NULL) {
 printf("No candidates available to show results.\n");
 return;
 }
 struct Candidate *arr[100];
 int n = getCandidateArray(arr);
 sortCandidatesByVotes(arr, n);
 printf("\n--- Election Results ---\n");
 printf("Rank\tID\tName\t\tVotes\n");
 for(int i = 0; i < n; i++) {
 printf("%d\t%d\t%-15s\t%d\n", i + 1, arr[i]->id, arr[i]->name, arr[i]->votes);
 }
 int maxVotes = arr[0]->votes;
 printf("\nWinner(s):\n");
 for(int i = 0; i < n; i++) {
 if(arr[i]->votes == maxVotes) {
 printf("%s (ID %d) with %d votes\n", arr[i]->name, arr[i]->id, arr[i]->votes);
 } else {
 break;
 }
 }
}
void adminMenu() {
 int choice;
 while (1) {
 printf("\n------ ADMIN MENU ------\n");
 printf("1. Add Candidate\n");
 printf("2. Remove Candidate\n");
 printf("3. View Candidates\n");
 printf("4. Set Number of Voters\n");
 printf("5. View Voters\n");
 printf("6. Save Results\n");
 printf("7. Display Results\n");
 printf("8. Exit\n");
 printf("Enter choice: ");
 scanf("%d", &choice);
 if (choice == 1) addCandidate();
 else if (choice == 2) removeCandidate();
 else if (choice == 3) viewCandidates();
 else if (choice == 4) generateVoters();
 else if (choice == 5) viewVoters();
 else if (choice == 6) saveResults();
 else if (choice == 7) showResults();
 else if (choice == 8) break;
 else printf("Invalid choice!\n");
 }
}
// ---------------- VOTER MODE -------------------
void castVote(struct Voter *v) {
 if(candidateHead == NULL) {
 printf("No candidates available to vote for.\n");
 return;
 }
 printf("\n--- Cast Your Vote ---\n");
 viewCandidates();
 printf("%d. NOTA\n", candidateCount + 1);
 int choice;
 printf("Enter your choice: ");
 scanf("%d", &choice);
 struct Candidate *temp = candidateHead;
 if (choice == candidateCount + 1) {
 printf("You selected NOTA.\n");
 }
 else if (choice >= 1 && choice <= candidateCount) {
 while (temp != NULL) {
 if (temp->id == choice) {
 temp->votes++;
 printf("Vote cast successfully!\n");
 break;
 }
 temp = temp->next;
 }
 }
 else {
 printf("Invalid choice!\n");
 return;
 }
 v->hasVoted = 1;
 saveCandidates();
 saveVoters();
}
void voterMenu() {
 if(voterHead == NULL) {
 printf("No voters available.Admin must generate voters first\n");
 return;
 }
 if(candidateHead == NULL) {
 printf("No candidates available. Admin must add candidates first\n");
 return;
 }
 int id;
 printf("Enter your voter ID: ");
 scanf("%d", &id);
 struct Voter *temp = voterHead;
 while (temp != NULL) {
 if (temp->id == id) {
 if (temp->hasVoted) {
 printf("You have already voted!\n");
 return;
 }
 castVote(temp);
 return;
 }
 temp = temp->next;
 }
 printf("Invalid Voter ID!\n");
}
// ---------------- MAIN -------------------
int main() {
 loadCandidates();
 loadVoters();
 int choice;
 while (1) {
 printf("\n======= VOTING SYSTEM =======\n");
 printf("1. Admin Login\n");
 printf("2. Voter Login\n");
 printf("3. Exit\n");
 printf("Enter choice: ");
 scanf("%d", &choice);
 if (choice == 1) {
 if(adminLogin())
 adminMenu();
 }
 else if (choice == 3) break;
 else printf("Invalid choice!\n");
 }
 return 0;
}