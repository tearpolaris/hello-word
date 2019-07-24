#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

struct node {
	char* message;
	struct node *next;
};

typedef struct node node;
node* head;

void init_stack(void);
void push(char* str);
void list_node(void);
char* pop(void);
void create_data(int num_node);
void sort_list_node(void);

int main() {
	char test[30], des[30];
	int ret;
	//printf("%s\n", test);
	//ret = strcmp(test, (char*)NULL);
	init_stack();
	create_data(3);
	list_node();
	_getch();
	return 0;
}

void init_stack(void) {
	node *list_node, *tmp_node;
	for (list_node = head; list_node != NULL;) {
		tmp_node = list_node->next;
		free(list_node->message);
		free(list_node);
		list_node = tmp_node;
	}
	head = (node*)NULL;
}

void push(char* str) {
	node* new_node = (node*)malloc(1*sizeof(node));
	new_node->message = (char*)malloc(60*sizeof(char));
	if (new_node == NULL) {
		printf("Not enough memory to allocate\n");
		return;//Exit function
	}
	else {
		new_node->next = head;
		strcpy(new_node->message, str);
		head = new_node;
	}
}

char* pop(void) {
	node* tmp_free;
	char* ret_str = (char*)malloc(strlen(head->message)+1);
	if (head == NULL) {
		printf("There is no node exist\n");
		return (char*)NULL;
	}
	else {
		strcpy(ret_str, head->message);
		tmp_free = head;
		head = head->next;
		free(tmp_free->message);
		free(tmp_free); 
	}
	return ret_str;
}

void create_data(int num_node) {
	char tmp_str[256];
	node* new_node;
	for (int i=0; i < num_node; i++) {
		printf("Please input string %d: ", i+1);
		scanf("%[^\n]%*c", tmp_str);
		//if (strcmp(tmp_str, (char*)NULL) > 0) {
			push(tmp_str);
		//}
	}
}

void list_node(void) {
	node* ls_node;
	int i;

	ls_node = head;
	i = 1;
	while (ls_node != NULL) {
		printf("Node %d is: %s\n", i, ls_node->message);
		ls_node = ls_node->next;
		i++;
	}
}

void sort_list_node(void) {
	node *node_max, *node_index, *previous_node, *tmp_node;

	node_max = (node*)malloc(1*sizeof(node));
	node_index = (node*)malloc(1*sizeof(node));
	previous_node = (node*)malloc(1*sizeof(node));
	tmp_node = (node*)malloc(1*sizeof(node));

	if ((node_max == NULL) || (node_index == NULL)) {
		printf("Not enough memory to allocate\n");
		return;
	}
	node_max = node_index = previous_node = head;

	while (node_index != NULL) {
		if (strcmp(node_index->message, node_index->next->message) > 0) {
			previous_node->next = node_index->next;
			tmp_node = node_index->next->next;
			//node_index->next = node_index;
			node_index->next->next = node_index;
			node_index->next = tmp_node;
		}
		node_index = node_index->next;
	}
}