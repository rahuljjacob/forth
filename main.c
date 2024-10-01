#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100
#define DICTIONARY_SIZE 100

typedef struct node {
  float data;
  struct node *next;
} NODE;

typedef struct stack {
  NODE *top;
} STACK;

typedef struct {
  char name[MAX_WORD_LENGTH];
  int type;
  union {
    void (*function)(STACK *);
    char definition[100];
  } executable;
} Word;

STACK *stack;
Word dictionary[DICTIONARY_SIZE];
int dictSize = 0;
int wordMode = 0;
int stringMode = 0;
int wordName = 0;

char *wordToken;

int isWord(char *token);
int isNumber(char *token);

// Stack functions
NODE *createnode(int data);
int isEmpty(STACK *l);
STACK *init();
void push(STACK *l, int data);
int pop(STACK *l);
int peek(STACK *l);
void printstack(STACK *l);

// forth words
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

void printString(char *str);
void execWord(char *token);
void addPredefinedWord(char *name, void (*function)(STACK *));
void initDictionary();

int main() {
  char input[256];
  char *token;

  stack = init();
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

void addPredefinedWord(char *name, void (*function)(STACK *)) {
  if (dictSize < DICTIONARY_SIZE) {
    strcpy(dictionary[dictSize].name, name);
    dictionary[dictSize].executable.function = function;
    dictionary[dictSize].type = 0;
    dictSize++;
  } else {
    printf("Dictionary Full");
  }
}

void initDictionary() {
  addPredefinedWord("+", add);
  addPredefinedWord("-", subtract);
  addPredefinedWord(".", dot);
  addPredefinedWord("dup", dup);
  addPredefinedWord("drop", drop);
  addPredefinedWord("over", over);
  addPredefinedWord("rot", rot);
  addPredefinedWord("cr", cr);
  addPredefinedWord("emit", emit);
  addPredefinedWord("quit", quit);
}

void execWord(char *token) {
  if (strcmp(token, ":") == 0) {
    wordMode = 1;
    return;
  }
  if (wordMode == 1) {
    if (wordName == 0) {
      strcpy(dictionary[dictSize].name, token);
      dictionary[dictSize].type = 1;
      wordName = 1;
    } else {
      if (strcmp(token, ";") == 0) {
        wordMode = 0;
        wordName = 0;
        printf("%s", dictionary[dictSize].executable.definition);
        dictSize++;
        return;
      }
      strcat(dictionary[dictSize].executable.definition, " ");
      strcat(dictionary[dictSize].executable.definition, token);
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
  for (int i = 0; i < dictSize; i++) {
    if (strcmp(token, dictionary[i].name) == 0) {
      if (dictionary[i].type == 0) {
        dictionary[i].executable.function(stack);
        return;
      } else {
        wordToken = strtok(dictionary[i].executable.definition, " \t\n");
        while (wordToken != NULL) {
          execWord(wordToken);
          wordToken = strtok(NULL, " \t\n");
        }
      }
    }
  }
  if (isNumber(token)) {
    push(stack, atoi(token));
    return;
  }
  printf("?");
}

int isWord(char *token) {}

void printString(char *str) {}

void add(STACK *l) { push(stack, (pop(stack) + pop(stack))); }

void subtract(STACK *l) { push(stack, (pop(stack) - pop(stack))); }

void dot(STACK *l) { printf("%d", pop(stack)); }

void dup(STACK *l) { push(stack, peek(stack)); }

void drop(STACK *l) { pop(stack); };

void cr(STACK *l) { printf("\n"); }

void emit(STACK *l) { printf("%d", pop(stack)); }

void quit(STACK *l) { exit(0); }

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
