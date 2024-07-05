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

NODE* createnode(int data);
int isEmpty(STACK* l);
STACK* init();
void push(STACK* l, int data);
int pop(STACK* l);
int peek(STACK* l);
void printstack(STACK* l);

int main(){
	stack = init();
	char str[100];
	fgets(str, 100, stdin);
	str[strcspn(str, "\n")] = 0;
	while(1){
		tokenize(str);
		printf("\n");
		fgets(str, 100, stdin);
		str[strcspn(str, "\n")] = 0;
	}
}

int isDefinedWord(char* token){
	return 0;
}

int isPredefinedWord(char* token){
	if (!strcmp(token, "+")){
		push(stack, (pop(stack) + pop(stack)));
		return 1;
	}
	else if(!strcmp(token, "-")){
		push(stack, (pop(stack) - pop(stack)));
		return 1;
	}
	return 0;
}

int isNumber(char* token){
	for(int i = 0; i < strlen(token); i++){
		if(!isdigit(token[i]))
			return 0;
	}
	return 1;
}

void parse(char* token){
	if(isDefinedWord(token)){
	}
	else if (isPredefinedWord(token)){
	}
	else if(isNumber(token)){
		push(stack, atoi(token));
	}
	else 
		printf(">> %s ?", token);
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
