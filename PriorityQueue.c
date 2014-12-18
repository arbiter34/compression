/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** PriorityQueue.c
** Defines PriorityQueue functionality using PQNode struct from header
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "PriorityQueue.h"
#include "BinaryTree.h"

void PQPush(PQNode **head, PQNode *n) {
	if (*head == NULL) {
		*head = (PQNode *)malloc(sizeof(malloc)*1);
		*head = n;
		(*head)->next = NULL;
		return;
	}
	if (n->value < (*head)->value) {
		n->next = *head;
		*head = n;		
		return;
	}
	if ((*head)->next == NULL) {
		(*head)->next = n;
		return;
	}
	if (n->value < (*head)->next->value) {
		n->next = (*head)->next;
		(*head)->next = n;
		return;
	}
	PQPush(&((*head)->next), n);

}

PQNode* PQPop(PQNode **head) {
	PQNode *pop = (PQNode*)malloc(sizeof(Node)*1);
	pop = *head;
	*head = (*head)->next;
	return pop;
}

PQNode* PQPeek(PQNode **head) {
	PQNode *peek = (PQNode*)malloc(sizeof(Node)*1);
	peek = *head;
	return peek;
}

unsigned int PQSize(PQNode *head) {
	if (head == NULL) {
		return 0;
	}
	return 1 + PQSize(head->next);
}

void PQPrint(PQNode *head) {
	if (head == NULL) {
		return;
	}
	printf("Value: %i  Char: %i\n", head->value, head->node->letter);
	PQPrint(head->next);
}