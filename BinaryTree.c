/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** BinaryTree.c
** Defines BinaryTree functionality using Node struct from header
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "BinaryTree.h"


void BTPrint(Node *head) {
	if (head == NULL) {
		return;
	}
	BTPrint(head->left);
	if (head->terminal) {
		printf("'%i': %i\n", head->letter, head->value);
	}
	BTPrint(head->right);
}

unsigned int BTFrequency(Node *head) {
	if (head == NULL) {
		return 0;
	}
	return head->value + BTFrequency(head->left) + BTFrequency(head->right);
}