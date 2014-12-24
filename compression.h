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
//Max desired RAM footprint, actual size should be targeted value +.8
	#define MAX_RAM 32
	#define true 1
	#define false 0
	
	const int BLOCK_SIZE = 1;
	
	 
	
	typedef struct EncodeLetter EncodeLetter;
	struct EncodeLetter {
		unsigned int bits;
		unsigned int value;
	};
	
	typedef struct FileNames FileNames;
	struct FileNames {
		char *fileIn;
		char *fileOut;
	};
	
	FileNames *getFileNames();
	
	void printMenu();
	
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
	
	unsigned int getFileSize(FILE *fp);

#endif