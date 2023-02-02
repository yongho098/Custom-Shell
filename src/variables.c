#include <string.h>
#include <stdlib.h>

#include "builtins.h"
#include "io_helpers.h"

typedef struct node {
	char *variable;
	char *name;
	struct node *next;
} Node;


Node *assign_variable(char *var, Node *next){
	// parses var, makes a node and sets name and variable to it
	char var_array[strlen(var)+1];
	strcpy(var_array, var);
	char *split = "=";
	char token[64];
	char variable[64];
	
	strcpy(token, strtok(var_array, split));
	strcpy(variable, strstr(var, split));
	char* chopped_assign = variable+1;

	// free is done at termination to prevent memory leaks
	Node *new_node = malloc(sizeof(Node));
	new_node->name=malloc(strlen(token)+1);
	new_node->variable=malloc(strlen(chopped_assign)+1);
	strcpy(new_node->name, token);
	strcpy(new_node->variable, chopped_assign);
	new_node->next = next;
	return new_node;
	}

// initialize first node
Node *create_head(){
	Node *front = NULL;
	return front;
}

// performed at exit
void freeList(struct node* head){
	struct node* current = head;
	struct node* next;

	while (current != NULL)
	{
		next = current->next;
		free(current->variable);
		free(current->name);
		free(current);
		current = next;
	}
	head=NULL;
}

// sees if var is in the linked list
int checker(char *var, struct node* head){
	int value=1;
	while (head != NULL){
		value = strcmp(var, head->name);
		if (value==0){
			return value;
			}
		head=head->next;
		}
	return value;
}

// if var is in the linked list then sets the name as variable
void set_value(char *var, struct node* head){
	while (head != NULL){
		int value;
		value = strcmp(var, head->name);
		if (value==0){
			strcpy(var, head->variable);
		}
	head=head->next;
	}
}
