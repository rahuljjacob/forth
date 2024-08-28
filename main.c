#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct node{
	float data;
	struct node* next;	
}NODE;

typedef struct stack{
	NODE* top;
}STACK;

STACK* stack;

void parse(char* token);
void tokenize(char* str);

int isDefinedWord(char* token);
int isPredefinedWord(char* token);
int isNumber(char* token);

NODE* createnode(int data);
int isEmpty(STACK* l);
STACK* init();
void push(STACK* l, int data);
int pop(STACK* l);
int peek(STACK* l);
void printstack(STACK* l);

void add(STACK *l);
void subtract(STACK *l);
int dot(STACK *l);
void dup(STACK *l);
void drop(STACK *l);
void over(STACK *l);
void rot(STACK *l);

int main(){
	stack = init();
	char str[100];
	fgets(str, 100, stdin);
	str[strcspn(str, "\n")] = 0;
	while(1){
		tokenize(str);
		fgets(str, 100, stdin);
		str[strcspn(str, "\n")] = 0;
	}
}

int isDefinedWord(char* token){
	return 0;
}

int isPredefinedWord(char* token){
	if (!strcmp(token, "+")){
		add(stack);
		return 1;
	}
	else if(!strcmp(token, "-")){
		subtract(stack);
		return 1;
	}
	else if(!strcmp(token, ".")){
		printf("%d \n", dot(stack));
		return 1;
	}
	else if(!strcmp(token, "dup")){
		dup(stack);
		return 1;
	}
	else if(!strcmp(token, "drop")){
		drop(stack);
		return 1;
	}
	else if(!strcmp(token, "over")){
		over(stack);
	}
	else if(!strcmp(token, "rot")){
		rot(stack);
	}
	else if(!strcmp(token, "emit")){

	}
	else if(!strcmp(token, ".\"")){

	}
	else if(!strcmp(token, "=")){

	}
	else if(!strcmp(token, "variable")){

	}
	return 0;
}

void add(STACK *l){
	push(stack, (pop(stack) + pop(stack)));
}

void subtract(STACK *l){
	push(stack, (pop(stack) - pop(stack)));
}

int dot(STACK *l){
	return pop(stack);
}

void dup(STACK *l){
	push(stack, peek(stack));
}

void drop(STACK *l){
	pop(stack);
};

void over(STACK *l){
	int over1 = pop(stack);
	int over2 = pop(stack);
	push(stack, over2);
	push(stack, over1);
	push(stack, over2);
};

void rot(STACK *l){
	int rot1 = pop(stack);
	int rot2 = pop(stack);
	int topElement = pop(stack);
	push(stack, rot2);
	push(stack, rot1);
	push(stack, topElement);
};

int isNumber(char* token){
	for(int i = 0; i < strlen(token); i++){
		if(!isdigit(token[i]))
			return 0;
	}
	return 1;
}

void parse(char* token){
	if(isDefinedWord(token));
	else if (isPredefinedWord(token));
	else if(isNumber(token)){
		push(stack, atoi(token));
	}
	else 
		printf(">> %s? \n", token);
}

void tokenize(char* str){
	char* token = strtok(str, " ");
	while (token != NULL) {
		parse(token);
		token = strtok(NULL, " ");
	}

}

NODE* createnode(int data){
	NODE* newnode = (NODE*)malloc(sizeof(NODE));
	newnode -> next = NULL;
	newnode -> data = data;
	return newnode;
}

int isEmpty(STACK* l){
	if (l -> top == NULL)
		return 1;
	else 
		return 0;
}

STACK* init(){
	STACK* stack = (STACK*)malloc(sizeof(STACK));
	stack -> top = NULL;
	return stack;
}

void push(STACK* l, int data){
	NODE* newhead = createnode(data);
	newhead -> next = l -> top;
	l -> top = newhead;
}

int pop(STACK* l){
	if(isEmpty(l)){
		printf("stack underflow\n");
		return -1;
	}
	NODE* oldhead = l -> top;
	int popped = oldhead -> data;
	l -> top = l -> top -> next;
	free(oldhead);
	return popped;
}

void printstack(STACK* l){
	NODE* printnode = l -> top;
	while (printnode -> next != NULL){
		printf("%f", printnode -> data);
		printf("\n");
		printnode = printnode -> next;
	}
	printf("%f \n", printnode -> data);
}

int peek(STACK* l){
	if (isEmpty(l)){
		printf("Empty Stack");
		return 0;
	}
	return l -> top -> data;
}
