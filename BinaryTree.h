/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** BinaryTree.h
** Head for BinaryTree functions
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#ifndef _BINARYTREE_H_
#define _BINARYTREE_H_
	
	typedef struct Node Node;
	struct Node {
		unsigned char terminal;
		unsigned char letter;
		unsigned int value;
		struct Node *left;
		struct Node *right;
	};
	
	void BTPrint(Node *head);
	
	unsigned int BTFrequency(Node *head);
	
#endif