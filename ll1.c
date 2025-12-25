#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX 20
#define MAX_SYMBOLS 50





char productions[MAX][MAX];
int n;

char firstSet[26][MAX];
char followSet[26][MAX];
int firstCalculated[26];
int followCalculated[26];

int isNonTerminal(char c) { return (c >= 'A' && c <= 'Z'); }

void addToSet(char set[], char value) {
    if (strchr(set, value) == NULL) {
        int len = strlen(set);
        set[len] = value;
        set[len + 1] = '\0';
    }
}

void unionSet(char dest[], char src[]) {
    for (int i = 0; src[i] != '\0'; i++)
        addToSet(dest, src[i]);
}

void findFirst(char c);
void findFollow(char c);

void findFirstForString(char result[], char *str) {
    result[0] = '\0';

    for (int i = 0; str[i] != '\0'; i++) {
        char symbol = str[i];

        if (!isNonTerminal(symbol)) {
            addToSet(result, symbol);
            return;
        }

        findFirst(symbol);
        char *fst = firstSet[symbol - 'A'];
        int eps = 0;

        for (int k = 0; fst[k] != '\0'; k++) {
            if (fst[k] == '#') eps = 1;
            else addToSet(result, fst[k]);
        }

        if (!eps) return;
    }

    addToSet(result, '#');
}

void findFirst(char c) {
    if (!isNonTerminal(c)) return;
    if (firstCalculated[c-'A']) return;

    firstCalculated[c-'A'] = 1;

    for (int i = 0; i < n; i++) {
        if (productions[i][0] != c) continue;

        char *rhs = productions[i] + 3;

        if (strcmp(rhs, "#") == 0) {
            addToSet(firstSet[c-'A'], '#');
            continue;
        }

        char temp[MAX]="";
        findFirstForString(temp, rhs);
        unionSet(firstSet[c-'A'], temp);
    }
}

void findFollow(char c) {
    if (!isNonTerminal(c)) return;
    if (followCalculated[c-'A']) return;

    followCalculated[c-'A'] = 1;

    if (productions[0][0] == c)
        addToSet(followSet[c-'A'], '$');

    for (int i = 0; i < n; i++) {
        char lhs = productions[i][0];
        char *rhs = productions[i] + 3;
        int len = strlen(rhs);

        for (int j = 0; j < len; j++) {
            if (rhs[j] == c) {

                if (j + 1 < len) {
                    char temp[MAX]="";
                    findFirstForString(temp, rhs + j + 1);

                    for (int k = 0; temp[k]; k++)
                        if (temp[k] != '#')
                            addToSet(followSet[c-'A'], temp[k]);

                    if (strchr(temp, '#')) {
                        findFollow(lhs);
                        unionSet(followSet[c-'A'], followSet[lhs-'A']);
                    }
                }
                else {
                    findFollow(lhs);
                    unionSet(followSet[c-'A'], followSet[lhs-'A']);
                }
            }
        }
    }
}





int table[26][128];  

// void buildLL1Table() {
    
//     for (int i = 0; i < 26; i++)
//         for (int j = 0; j < 128; j++)
//             table[i][j] = -1;

    
//     for (int p = 0; p < n; p++) {
//         char A = productions[p][0];
//         char *rhs = productions[p] + 3;

//         char firstRHS[MAX] = "";
//         findFirstForString(firstRHS, rhs);

        
//         for (int k = 0; firstRHS[k]; k++) {
//             char t = firstRHS[k];
//             if (t != '#')
//                 table[A-'A'][t] = p;
//         }

        
//         if (strchr(firstRHS, '#')) {
//             char *f = followSet[A-'A'];
//             for (int k = 0; f[k]; k++) {
//                 char t = f[k];
//                 table[A-'A'][t] = p;
//             }
//         }
//     }
// }

void buildLL1Table() {

    // initialize table
    for (int i = 0; i < 26; i++)
        for (int j = 0; j < 128; j++)
            table[i][j] = -1;

    // build table
    for (int p = 0; p < n; p++) {
        char A = productions[p][0];
        char *rhs = productions[p] + 3;

        char firstRHS[MAX] = "";
        findFirstForString(firstRHS, rhs);

        // FIRST(alpha)
        for (int k = 0; firstRHS[k]; k++) {
            char t = firstRHS[k];
            if (t == '#') continue;

            if (table[A-'A'][t] != -1 &&
                table[A-'A'][t] != p) {

                printf("\n❌ LL(1) CONFLICT detected!\n");
                printf("Table[%c, %c] has clash between:\n",
                       A, t);
                printf("  %s\n",
                       productions[table[A-'A'][t]]);
                printf("  %s\n", productions[p]);
                exit(0);
            }

            table[A-'A'][t] = p;
        }

        // epsilon case
        if (strchr(firstRHS, '#')) {
            char *f = followSet[A-'A'];
            for (int k = 0; f[k]; k++) {
                char t = f[k];

                if (table[A-'A'][t] != -1 &&
                    table[A-'A'][t] != p) {

                    printf("\n❌ LL(1) CONFLICT detected!\n");
                    printf("Table[%c, %c] has clash between:\n",
                           A, t);
                    printf("  %s\n",
                           productions[table[A-'A'][t]]);
                    printf("  %s\n", productions[p]);
                    exit(0);
                }

                table[A-'A'][t] = p;
            }
        }
    }

    printf("\n✅ LL(1) Parsing Table constructed successfully (No conflicts).\n");
}




void parseString(char input[]) {
    char stack[MAX_SYMBOLS];
    int top = 0;

    
    stack[top++] = '$';
    stack[top++] = productions[0][0];  

    int ip = 0;  

    printf("\nParsing steps:\n");

    while (1) {

        char X = stack[top-1];
        char a = input[ip];

        printf("Stack: ");
        for (int i = 0; i < top; i++) printf("%c", stack[i]);
        printf("   Input: %s\n", input + ip);

        
        if (X == '$' && a == '$') {
            printf("\nString ACCEPTED by LL(1) parser.\n");
            return;
        }

        if (!isNonTerminal(X)) {
            if (X == a) {
                top--;  
                ip++;   
            } else {
                printf("\nERROR: Expected '%c' but found '%c'\n", X, a);
                return;
            }
        }
        
        else {
            int prodIndex = table[X-'A'][a];

            if (prodIndex == -1) {
                printf("\nERROR: No rule for [%c, %c]\n", X, a);
                return;
            }
            char *rhs = productions[prodIndex] + 3;
            printf("Apply production: %s\n\n", productions[prodIndex]);
            top--;  
            int len = strlen(rhs);
            if (!(len == 1 && rhs[0] == '#')) {
                for (int i = len - 1; i >= 0; i--)
                    stack[top++] = rhs[i];
            }
        }
    }
}

int main() {

    printf("Enter number of productions: ");
    scanf("%d", &n);

    printf("Enter productions (e.g., A->aB, A->#, S->AB):\n");
    for (int i = 0; i < n; i++)
        scanf("%s", productions[i]);

    for (int i = 0; i < 26; i++) {
        firstSet[i][0] = '\0';
        followSet[i][0] = '\0';
        firstCalculated[i] = 0;
        followCalculated[i] = 0;
    }

    for (int i = 0; i < n; i++)
        findFirst(productions[i][0]);

    for (int i = 0; i < n; i++)
        findFollow(productions[i][0]);

    
    printf("\nFIRST sets:\n");
    for (int i = 0; i < n; i++) {
        char A = productions[i][0];
        printf("FIRST(%c) = { %s }\n", A, firstSet[A-'A']);
    }

    
    printf("\nFOLLOW sets:\n");
    for (int i = 0; i < n; i++) {
        char A = productions[i][0];
        printf("FOLLOW(%c) = { %s }\n", A, followSet[A-'A']);
    }

    
    buildLL1Table();

    
    printf("\nLL(1) Parsing Table (NonTerminal x Terminal):\n");
    for (int A = 0; A < 26; A++) {
        int used = 0;
        for (int t = 0; t < 128; t++) {
            if (table[A][t] != -1) {
                if (!used) { printf("%c:\n", A+'A'); used = 1; }
                printf("   %c -> %s\n", t, productions[table[A][t]]);
            }
        }
    }

    
    char input[MAX];
    printf("\nEnter string to parse (append $ at end): ");
    scanf("%s", input);

    parseString(input);

    return 0;
}