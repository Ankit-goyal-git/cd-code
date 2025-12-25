#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 20

char productions[MAX][MAX];
int n;

char firstSet[26][MAX];
char followSet[26][MAX];
int firstCalculated[26];
int followCalculated[26];

int isNonTerminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

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

/* FIRST of a string (used internally) */
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
            if (fst[k] == '#')
                eps = 1;
            else
                addToSet(result, fst[k]);
        }

        if (!eps)
            return;
    }

    addToSet(result, '#');
}

/* FIRST set */
void findFirst(char c) {
    if (!isNonTerminal(c)) return;
    if (firstCalculated[c - 'A']) return;

    firstCalculated[c - 'A'] = 1;

    for (int i = 0; i < n; i++) {
        if (productions[i][0] != c) continue;

        char *rhs = productions[i] + 3;

        if (strcmp(rhs, "#") == 0) {
            addToSet(firstSet[c - 'A'], '#');
            continue;
        }

        char temp[MAX] = "";
        findFirstForString(temp, rhs);
        unionSet(firstSet[c - 'A'], temp);
    }
}

/* FOLLOW set */
void findFollow(char c) {
    if (!isNonTerminal(c)) return;
    if (followCalculated[c - 'A']) return;

    followCalculated[c - 'A'] = 1;

    if (productions[0][0] == c)
        addToSet(followSet[c - 'A'], '$');

    for (int i = 0; i < n; i++) {
        char lhs = productions[i][0];
        char *rhs = productions[i] + 3;
        int len = strlen(rhs);

        for (int j = 0; j < len; j++) {
            if (rhs[j] == c) {

                if (j + 1 < len) {
                    char temp[MAX] = "";
                    findFirstForString(temp, rhs + j + 1);

                    for (int k = 0; temp[k]; k++)
                        if (temp[k] != '#')
                            addToSet(followSet[c - 'A'], temp[k]);

                    if (strchr(temp, '#')) {
                        findFollow(lhs);
                        unionSet(followSet[c - 'A'],
                                 followSet[lhs - 'A']);
                    }
                } else {
                    findFollow(lhs);
                    unionSet(followSet[c - 'A'],
                             followSet[lhs - 'A']);
                }
            }
        }
    }
}

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &n);

    printf("Enter productions (e.g., S->AB, A->a, B->#):\n");
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
        printf("FIRST(%c) = { %s }\n", A, firstSet[A - 'A']);
    }

    printf("\nFOLLOW sets:\n");
    for (int i = 0; i < n; i++) {
        char A = productions[i][0];
        printf("FOLLOW(%c) = { %s }\n", A, followSet[A - 'A']);
    }

    return 0;
}
