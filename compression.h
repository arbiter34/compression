/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** compression.h
** Header file for compression.c.  Provides structs and function prototypes
** for export
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

	typedef struct EncodeLetter EncodeLetter;
	struct EncodeLetter {
		unsigned int bits;
		unsigned int value;
	};
	
	PQNode* buildPriorityQueue(unsigned int *char_frequency);
	
	Node* buildBinaryTree(PQNode* pq);
	
	void printUsage();

	void buildCharMap(Node *n, EncodeLetter *encodeArray[], size_t size, unsigned int value, unsigned int bits);
	
	void writeTreeToFile(Node *n, FILE *out);
	
	Node* readTreeFromFile(FILE *in);
	
	void writeBitToFile(int count, char byte, FILE *out);
	
	unsigned char readBitFromFile(int count, FILE *in);
	
	unsigned char decompressChar(Node *head, FILE *in);

	void compressFile(char* filein, char* fileout);

	void decompressFile(char* filein, char* fileout);

#endif