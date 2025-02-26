#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100
#define DICTIONARY_SIZE 100
#define MAX_WORD_TOKENS 100
#define MAX_VAR_CONSTANTS 101

typedef struct node {
    float data;
    struct node *next;
} NODE;

typedef struct stack {
    NODE *top;
} STACK;

typedef struct variable {
    char name[10];
    int location;
    int value;
} Variable;

typedef struct constant {
    char name[10];
    int value;
} Constant;

typedef struct {
    char name[MAX_WORD_LENGTH];
    int type;
    int compileWord;
    union {
        void (*function)(STACK *);
        char definition[15][MAX_WORD_TOKENS];
    } executable;
} Word;

STACK *stack;
STACK *ControlStack;
STACK *loopUpperBound;
STACK *loopLowerBound;
STACK *loopJumpIndex;
Word dictionary[DICTIONARY_SIZE];
Variable variables[MAX_VAR_CONSTANTS];
Constant constants[MAX_VAR_CONSTANTS];
int dictSize = 0;
int variableDictSize = 0;
int constDictSize = 0;
int variableMode = 0;
int constMode = 0;
int wordMode = 0;
int stringMode = 0;
int wordName = 0;
int wordIndexDuringDefinition = 0;

// Forth words
void add(STACK *l);
void subtract(STACK *l);
void dot(STACK *l);
void dup(STACK *l);
void drop(STACK *l);
void over(STACK *l);
void rot(STACK *l);
void cr(STACK *l);
void emit(STACK *l);
void quit(STACK *l);
void mod(STACK *l);
void bang(STACK *l);
void ampersand(STACK *l);
void forth_equal(STACK *l);
void forth_lesser_than(STACK *l);
void forth_greater_than(STACK *l);
void forth_lesser_than(STACK *l);
void forth_greater_than_or_equal_to(STACK *l);
void forth_lesser_than_or_equal_to(STACK *l);
void forth_not_equal(STACK *l);

// Main Functions
void addPredefinedWord(char *name, void (*function)(STACK *), int compiled);
void initDictionary();
void execWord(char *token);
void execDefinedWords(char function_defintion[100][100]);
int isNumber(char *token);

// Stack functions
NODE *createnode(int data);
int isEmpty(STACK *l);
STACK *init();
void push(STACK *l, int data);
int pop(STACK *l);
int peek(STACK *l);
void printstack(STACK *l);
int stackDepth(STACK *l);

int main() {
    char input[256];
    char *token;

    stack = init();
    ControlStack = init();
    loopUpperBound = init();
    loopLowerBound = init();
    loopJumpIndex = init();
    initDictionary();

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        token = strtok(input, " \t\n");
        while (token != NULL) {
            execWord(token);
            token = strtok(NULL, " \t\n");
        }
    }
}

void addPredefinedWord(char *name, void (*function)(STACK *), int compiled) {
    if (dictSize < DICTIONARY_SIZE) {
        strcpy(dictionary[dictSize].name, name);
        dictionary[dictSize].executable.function = function;
        dictionary[dictSize].type = 0;
        dictionary[dictSize].compileWord = compiled;
        dictSize++;
    } else {
        printf("Dictionary Full");
    }
}

void initDictionary() {
    addPredefinedWord("+", add, 0);
    addPredefinedWord("-", subtract, 0);
    addPredefinedWord(".", dot, 0);
    addPredefinedWord("dup", dup, 0);
    addPredefinedWord("drop", drop, 0);
    addPredefinedWord("over", over, 0);
    addPredefinedWord("rot", rot, 0);
    addPredefinedWord("cr", cr, 0);
    addPredefinedWord("emit", emit, 0);
    addPredefinedWord("quit", quit, 0);
    addPredefinedWord("mod", mod, 0);
    addPredefinedWord("=", forth_equal, 0);
    addPredefinedWord(">", forth_greater_than, 0);
    addPredefinedWord("<", forth_lesser_than, 0);
    addPredefinedWord(">=", forth_greater_than_or_equal_to, 0);
    addPredefinedWord("<=", forth_lesser_than_or_equal_to, 0);
    addPredefinedWord("!", bang, 0);
    addPredefinedWord("@", ampersand, 0);
}

void execWord(char *token) {
    // if (token == NULL | strcmp(token, " ") == 0)
    //   return;
    if (strcmp(token, ":") == 0) {
        wordMode = 1;
        return;
    }
    if (wordMode == 1) {
        if (wordName == 0) {
            strcpy(dictionary[dictSize].name, token);
            dictionary[dictSize].type = 1;
            memset(dictionary[dictSize].executable.definition, '\0', 100);
            wordName = 1;
        } else {
            if (strcmp(token, ";") == 0) {
                // printf("%s", dictionary[dictSize].executable.definition);
                wordMode = 0;
                wordIndexDuringDefinition = 0;
                wordName = 0;
                dictSize++;
                return;
            }
            strcpy(dictionary[dictSize]
                       .executable.definition[wordIndexDuringDefinition],
                   token);
            wordIndexDuringDefinition++;
        }
        return;
    }
    if (strcmp(token, ".\"") == 0 && stringMode != 1) {
        stringMode = 1;
        return;
    }
    if (stringMode) {
        if (strcmp(token, "\"") == 0) {
            stringMode = 0;
            return;
        }
        printf("%s ", token);
        return;
    }
    if (strcmp(token, "variable") == 0) {
        variableMode = 1;
        return;
    }
    if (variableMode == 1) {
        strcpy(variables[variableDictSize].name, token);
        variables[variableDictSize].location = variableDictSize;
        variableMode = 0;
        variableDictSize++;
        return;
    }
    if (strcmp(token, "constant") == 0) {
        constMode = 1;
        return;
    }
    if (constMode == 1) {
        strcpy(constants[constDictSize].name, token);
        constants[constDictSize].value = pop(stack);
        constDictSize++;
        constMode = 0;
        return;
    }
    for (int i = 0; i < dictSize; i++) {
        if (strcmp(token, dictionary[i].name) == 0) {
            if (dictionary[i].type == 0) {
                dictionary[i].executable.function(stack);
                return;
            } else {
                execDefinedWords(dictionary[i].executable.definition);
                return;
            }
        }
    }
    for (int i = 0; i < variableDictSize; i++) {
        if (strcmp(token, variables[i].name) == 0) {
            push(stack, variables[i].location);
            return;
        }
    }
    for (int i = 0; i < constDictSize; i++) {
        if (strcmp(token, constants[i].name) == 0) {
            push(stack, constants[i].value);
            return;
        }
    }
    if (isNumber(token)) {
        push(stack, atoi(token));
        return;
    }
    printf("?");
}

void execDefinedWords(char function_defintion[100][100]) {
    int index = 0;
    char *token = function_defintion[index];
    int breakContinueFlag = 0;
    int passFlag = 0;
    int curLoop;
    while (strcmp(token, "\0") != 0) {
        token = function_defintion[index];
        index++;
        if (stringMode) {
            if (strcmp(token, "\"") == 0) {
                stringMode = 0;
                continue;
            }
            printf("%s ", token);
            continue;
        }
        if (strcmp(token, "\0") == 0)
            return;
        if (strcmp(token, "do") == 0) {
            // implement loop stack
            int lowerBound = pop(stack);
            int upperBound = pop(stack);
            if (upperBound < lowerBound) {
                printf("Upper bound of loop must be greater");
                exit(0);
            }
            push(loopLowerBound, lowerBound);
            push(loopUpperBound, upperBound);
            push(loopJumpIndex, index);
            continue;
        }
        if (strcmp(token, "loop") == 0) {
            int loopCheck = pop(loopLowerBound);
            if (loopCheck + 1 == peek(loopUpperBound)) {
                pop(loopUpperBound);
                pop(loopJumpIndex);
            } else {
                push(loopLowerBound, loopCheck + 1);
                index = peek(loopJumpIndex);
            }
            continue;
        }
        if (strcmp(token, "if") == 0) {
            if (passFlag == 1) {
                push(ControlStack, -1);
            } else {
                if (pop(stack) != 0) {
                    passFlag = 0;
                    push(ControlStack, 1);
                } else {
                    passFlag = 1;
                    push(ControlStack, 0);
                }
            }
            continue;
        }
        if (strcmp(token, "else") == 0) {
            if (peek(ControlStack) == -1) {
            } else {
                if (peek(ControlStack) == 1) {
                    passFlag = 1;
                } else {
                    passFlag = 0;
                }
            }
            continue;
        }
        if (strcmp(token, "then") == 0) {
            pop(ControlStack);
            passFlag = 0;
            continue;
        }
        if (passFlag == 0) {
            if (strcmp(token, ".\"") == 0 && stringMode != 1) {
                stringMode = 1;
                continue;
            }
            if (stringMode) {
                if (strcmp(token, "\"") == 0) {
                    stringMode = 0;
                    continue;
                }
                printf("%s ", token);
                continue;
            }
            for (int i = 0; i < dictSize; i++) {
                if (strcmp(token, dictionary[i].name) == 0) {
                    if (dictionary[i].type == 0) {
                        dictionary[i].executable.function(stack);
                        breakContinueFlag = 1;
                    } else {
                        execDefinedWords(dictionary[i].executable.definition);
                        breakContinueFlag = 1;
                    }
                }
            }
            if (isNumber(token)) {
                push(stack, atoi(token));
                continue;
            }

            if (breakContinueFlag == 1) {
                breakContinueFlag = 0;
                continue;
            }
            printf("? %s \n", token);
        }
    }
}

// Forth Predefined Words
void add(STACK *l) { push(stack, (pop(stack) + pop(stack))); }

void subtract(STACK *l) { push(stack, (pop(stack) - pop(stack))); }

void dot(STACK *l) { printf("%d", pop(stack)); }

void dup(STACK *l) { push(stack, peek(stack)); }

void drop(STACK *l) { pop(stack); };

void cr(STACK *l) { printf("\n"); }

void emit(STACK *l) { printf("%c", pop(stack)); }

void quit(STACK *l) { exit(0); }

void mod(STACK *l) {
    int divisor = pop(stack);
    push(stack, pop(stack) % divisor);
}

void over(STACK *l) {
    int over1 = pop(stack);
    int over2 = pop(stack);
    push(stack, over2);
    push(stack, over1);
    push(stack, over2);
};

void rot(STACK *l) {
    int rot1 = pop(stack);
    int rot2 = pop(stack);
    int topElement = pop(stack);
    push(stack, rot2);
    push(stack, rot1);
    push(stack, topElement);
};

void bang(STACK *l) {
    int location = pop(stack);
    variables[location].value = pop(stack);
}

void ampersand(STACK *l) {
    int location = pop(stack);
    if (location < 0)
        push(stack, 0);
    push(stack, variables[location].value);
}

void forth_equal(STACK *l) {
    if (pop(stack) == pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

void forth_not_equal(STACK *l) {
    if (pop(stack) != pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

void forth_lesser_than(STACK *l) {
    if (pop(stack) > pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

void forth_greater_than(STACK *l) {
    if (pop(stack) < pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

void forth_greater_than_or_equal_to(STACK *l) {
    if (pop(stack) <= pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

void forth_lesser_than_or_equal_to(STACK *l) {
    if (pop(stack) >= pop(stack))
        push(stack, 1);
    else
        push(stack, 0);
}

// Stack Operations and misc

int isNumber(char *token) {
    for (int i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i]))
            return 0;
    }
    return 1;
}

NODE *createnode(int data) {
    NODE *newnode = (NODE *)malloc(sizeof(NODE));
    newnode->next = NULL;
    newnode->data = data;
    return newnode;
}

int isEmpty(STACK *l) {
    if (l->top == NULL)
        return 1;
    else
        return 0;
}

STACK *init() {
    STACK *stack = (STACK *)malloc(sizeof(STACK));
    stack->top = NULL;
    return stack;
}

void push(STACK *l, int data) {
    NODE *newhead = createnode(data);
    newhead->next = l->top;
    l->top = newhead;
}

int pop(STACK *l) {
    if (isEmpty(l)) {
        printf("stack underflow\n");
        return -1;
    }
    NODE *oldhead = l->top;
    int popped = oldhead->data;
    l->top = l->top->next;
    free(oldhead);
    return popped;
}

void printstack(STACK *l) {
    NODE *printnode = l->top;
    while (printnode->next != NULL) {
        printf("%f", printnode->data);
        printf("\n");
        printnode = printnode->next;
    }
    printf("%f \n", printnode->data);
}

int peek(STACK *l) {
    if (isEmpty(l)) {
        printf("Empty Stack");
        return 0;
    }
    return l->top->data;
}

int stackDepth(STACK *l) {
    int depth = 0;
    NODE *head = l->top;
    while (head != NULL) {
        depth++;
        head = head->next;
    }
    return depth;
}
