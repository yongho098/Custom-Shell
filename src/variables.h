#ifndef __VARIABLES_H
#define __VARIABLES_H

typedef struct node
{
	char variable;
	char name;
	struct node *next;
} Node;

Node *assign_variable(const char *var, Node *next);
Node *create_head();
void freeList(struct node* head);
void displayvars(struct node* head);
void set_value(const char *var, Node *head);
int checker(char *var, struct node* head);
#endif
