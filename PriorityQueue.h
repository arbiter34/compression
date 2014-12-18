/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** PriorityQueue.h
** Head for PriorityQueue functions
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_
	
	typedef struct PQNode PQNode;
	struct PQNode {
		unsigned int value;
		struct Node *node;
		struct PQNode *next;
	};
	
	void PQPush(PQNode **head, PQNode *n);
	PQNode* PQPop(PQNode **head);
	PQNode* PQPeek(PQNode **head);
	unsigned int PQSize(PQNode *head);
	void PQPrint(PQNode *head);

#endif