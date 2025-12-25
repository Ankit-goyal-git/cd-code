#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_SYMBOLS 15

int main() {
    char expression[MAX_SYMBOLS];
    char symbol_table[MAX_SYMBOLS];
    void *addresses[MAX_SYMBOLS];
    int i = 0, x = 0;
    char c;

    printf("Expression terminated by $: ");
    while (i < MAX_SYMBOLS - 1 && (c = getchar()) != '$') {
        expression[i++] = c;
    }
    expression[i] = '\0';

    printf("Given Expression: %s\n", expression);
    printf("\nSymbol Table\n");
    printf("Symbol\tAddress\t\tType\n");

    for (int j = 0; j < i; j++) {
        c = expression[j];
        if (isalpha(c)) {  
            addresses[x] = malloc(sizeof(char));
            if (!addresses[x]) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }
            symbol_table[x] = c;
            printf("%c\t%p\tidentifier\n", c, addresses[x]);
            x++;
        }
        else if (isdigit(c)) {  
            addresses[x] = malloc(sizeof(char));
            if (!addresses[x]) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }
            symbol_table[x] = c;
            printf("%c\t%p\tconstant\n", c, addresses[x]);
            x++;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '=') {  
            symbol_table[x] = c;
            printf("%c\t-\toperator\n", c);  
            x++;
        }
        
    }

    
    for (int k = 0; k < x; k++) {
        if (isalpha(symbol_table[k]) || isdigit(symbol_table[k]))
            free(addresses[k]);
    }

    return 0;
}