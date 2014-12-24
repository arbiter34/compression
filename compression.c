/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** compression.c
** main file for compression CLI program
**
** Author: Travis Alpers - arbiter34@gmail.com
** -------------------------------------------------------------------------*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "BinaryTree.h"
#include "PriorityQueue.h"
#include "compression.h"

//bitstream file i/o buffers
unsigned char input;

//bit index for bitstream file i/o
int read_bit_count = 0;
int write_bit_count = 0;

//length of tree in bits in file
int tree_bit_length;

//input byte buffer
unsigned char *buffer, *output;


int main(int argc, char *argv[]) {
	//Verify correct CLI usage
	if (argc != 1) {
		printf("Incorrect usage\n");
		return -1;
	}	

	while (true) {

		printMenu();
		int option = 0;
		if (scanf("%i", &option) < 0) {
			printf("\n\nInvalid Entry. Try again.\n\n");
			continue;
		}
		
		FileNames *fn = NULL;
		switch(option){
			case 1:
				fn = getFileNames();
				compressFile(fn->fileIn, fn->fileOut);
				break;
			case 2:
				fn = getFileNames();
				
				decompressFile(fn->fileIn, fn->fileOut);
				break;
			default:
				printf("Exiting...\n\n", argv[1]);
				exit(0);
		}
	}
	
	
	
	return 0;
}
 
FileNames *getFileNames() {
	FileNames *fn = (FileNames *)malloc(sizeof(FileNames)*1);
	char buffer[255];
	int character_count = 0;
	while (true) {
		printf("\n\nEnter the input file path and name: ");
		if (scanf("%s", &buffer) < 0) {
			printf("\n\nInvalid entry\\n\n");
			continue;
		}
		character_count = strlen(buffer);
		fn->fileIn = (char*)malloc(sizeof(char)*character_count);
		strncpy(fn->fileIn, buffer, sizeof(char)*character_count);
		
		printf("\n\nEnter the output file path and name: ");
		if (scanf("%s", &buffer) < 0) {
			printf("\n\nInvalid entry\\n\n");
			continue;
		}
		
		character_count = strlen(buffer);
		fn->fileOut = (char*)malloc(sizeof(char)*character_count);
		strncpy(fn->fileOut, buffer, sizeof(char)*character_count);
		
		break;
	}
	return fn;
}

void printMenu() {
	printf("\n\n1. Compress File");
	printf("\n\n2. Decompress File");
	printf("\n\n3. Exit\n\n");
	printf("Make a selection: ");
}


/*
 *	compressFile	
 *
 *	Function: Builds a Huffman tree from an input file using the Huffman coding
 *	technique.  Tree is then written to output file followed by encoded input 
 *	file contents.
 *
 *	Arguments: Input file char* and Output file char*
 *	Returns: None
 */
void compressFile(char* filein, char* fileout) {
	//allocate arrays
	unsigned int *char_frequency = (unsigned int *)malloc(sizeof(int)*256);
	
	free(buffer);
	free(output);
	
	buffer = (unsigned char *)malloc(sizeof(char)*BLOCK_SIZE);
	output = (unsigned char *)malloc(sizeof(char)*BLOCK_SIZE);
	
	//Attempt open file stream
	FILE *fp = fopen(filein, "r");

	//Check if file pointer is valid
	if (fp == NULL) {
		printf("ShitError: Couldn't open %s\n", filein);
		return;
	} 
	
	//init char_frequency array
	for (int i = 0; i < 256; i++) {
		char_frequency[i] = 0;
	}
	unsigned int file_size = getFileSize(fp);
	printf("File size: %i\n", file_size);
	
	//Read in file
	while (fread(buffer, sizeof(char), BLOCK_SIZE, fp) > 0) {
		for (int i = 0; i < BLOCK_SIZE; i++) {
			char_frequency[buffer[i]]++;
		}
	}
	
	
	//built frequency array, close file to avoid resource collisions
	fclose(fp);
	
	//build priorityqueue from frequency_array
	PQNode *pq = buildPriorityQueue(char_frequency);
	
	//build huffman binary tree from priorityqueue
	Node *bt = buildBinaryTree(pq);	
	
	//alloc encoded character map
	EncodeLetter *encodedCharMap[256];

	//encodedCharMap returned by reference
	buildCharMap(bt, encodedCharMap, 256*sizeof(EncodeLetter), 0, 0);

	
	//Attempt open file stream
	fp = fopen(filein, "r");
	FILE *out = fopen(fileout, "w");

	//Check if file pointer is valid
	if (fp == NULL) {
		printf("Error: Couldn't open %s\n", filein);
		return;
	}	

	//Check if file pointer is valid
	if (out == NULL) {
		printf("Error: Couldn't open %s\n", fileout);
		return;
	}	
	for (int i = 0; i < BLOCK_SIZE; i++) {
		output[i] = 0;
	}
	write_bit_count = 0;
	
	fwrite(&file_size, sizeof(file_size), 1, out);
	
	//write btree to file	
	writeTreeToFile(bt, out);
	
	int count = 0;
	//Read in file byte by byte and output bit by bit via charmap
	while (fread(buffer, sizeof(unsigned char), BLOCK_SIZE, fp) > 0) {
		for (int y = 0; y < BLOCK_SIZE; y++) {	
			for (int i = (encodedCharMap[buffer[y]]->bits)-1; i >= 0; i--) {
			
				//reveal each bit per encodedChar
				char bit = ((1<<i) & encodedCharMap[buffer[y]]->value) == 0 ? 0 : 1;
				
				writeBitToFile(1, bit, out);
			}
			count++;
			if (count == file_size) {
				break;
			}
		}
	}
	
	//check for partial byte in bit write buffer
	if (write_bit_count % 8 != 0) {
		//grab write buffer
		char part = output[count%BLOCK_SIZE]; 
		
		//write it out to file with correct shift
		writeBitToFile(8, part, out);
	}
	


	fclose(fp);
	fclose(out);
	
	printf("\nThe file %s was compressed to %s successfully!\n", filein, fileout);
}


/*
 *	decompressFile	
 *
 *	Function: Builds Huffman Tree from file header in the 
 *  input file then uses it to decode the succeeding bytes
 *	and output them to the output file.
 *
 *	Arguments: Input file char* and Output file char*
 *	Returns: None
 */
void decompressFile(char* filein, char* fileout) {

	FILE *in = fopen(filein, "r");
	FILE *out = fopen(fileout, "w");
	
	//reset bit indexes
	write_bit_count = 0;
	read_bit_count = 0;
	
	free(buffer);
	free(output);
	
	buffer = (unsigned char *)malloc(sizeof(char)*BLOCK_SIZE);
	output = (unsigned char *)malloc(sizeof(char)*BLOCK_SIZE);
	
	//get file size from header
	unsigned int file_size = 0;
	fread(&file_size, sizeof(file_size), 1, in);
	
	//build tree from header
	Node *head = readTreeFromFile(in);
	
	//get offset for decoding to begin(tree length + sizeof file_size)
	unsigned long int offset = (tree_bit_length/8)+sizeof(file_size)+1;

	//seek to start of decoding
	fseek(in, offset, SEEK_SET);

	//decode until end of file
	while (!feof(in)) {
		
		//decode letter
		unsigned char c = decompressChar(head, in);
		
		//necessary check to avoid decoding the trailing zeros on the last byte
		if (ftell(out) >= file_size) {
			break;
		}
		
		//write decoded letter to out file
		fwrite(&c, sizeof(unsigned char), 1, out);
	}
		

	fclose(in);
	fclose(out);

	printf("\nThe file %s was decompressed to %s successfully!\n", filein, fileout);

}

/*
 *	buildPriorityQueue	
 *
 *	Function: Builds a priority queue given an array 
 *	which defines frequency or value.
 *
 *	Arguments: Character/Number frequency array
 *	Returns: Priority Queue head node
 */
PQNode* buildPriorityQueue(unsigned int *char_frequency) {
	PQNode *pq = NULL;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_frequency[i] > 0) {
			Node *n = (Node *)malloc(sizeof(Node)*1);
			
			n->value = char_frequency[i];
			n->letter = i;
			n->terminal = 1;
			n->left = NULL;
			n->right = NULL;
			PQNode *pn = (PQNode *)malloc(sizeof(PQNode)*1);
			pn->value = n->value;
			pn->node = n;
			pn->next = NULL;
			PQPush(&pq, pn);
		}
	}
	return pq;
}

/*
 *	buildBinaryTree	
 *
 *	Function: Builds a Huffman Tree given a priority
 *	queue.
 *
 *	Arguments: PriorityQueue node
 *	Returns: Huffman Tree head node
 */
Node* buildBinaryTree(PQNode* pq) {
	while (PQSize(pq) > 1) {
		Node *p = PQPop(&pq)->node;
		Node *q = PQPop(&pq)->node;
		Node *r = (Node *)malloc(sizeof(Node)*1);

		r->value = p->value + q->value;
		r->letter = 0;
		r->terminal = 0;
		r->left = p;
		r->right = q;
		PQNode *pn = (PQNode *)malloc(sizeof(PQNode)*1);
		pn->value = r->value;
		pn->node = r;
		pn->next = NULL;
		PQPush(&pq, pn);
	}
	return PQPeek(&pq)->node;
}

/*
 *	buildCharMap	
 *
 *	Function: Builds a map for encoding char's
 *	to bits corresponding to their Huffman Tree	
 *	path using recursion.
 *
 *	Arguments: Huffman Tree head node, uninitialized 
 *	array of EncodeLetter structs, size of array,
 *  current EncodeArray value, current EncodeArray 
 *	bits.
 *
 *	Returns: EncodeLetter array by reference
 */
void buildCharMap(Node *n, EncodeLetter *encodeArray[], size_t size, unsigned int value, unsigned int bits) {
	if (n->terminal) {
		encodeArray[n->letter] = (EncodeLetter *)malloc(sizeof(EncodeLetter)*1);
		encodeArray[n->letter]->value = value;
		encodeArray[n->letter]->bits = bits;				
	} else {
		bits++;
		buildCharMap(n->left, encodeArray, size, (value<<1)+0, bits);
		buildCharMap(n->right, encodeArray, size, (value<<1)+1, bits);
	}
}

/*
 *	decompressChar	
 *
 *	Function: Decodes the next letter in a Huffman
 *  encoded file relative to the current file 
 *  pointer position.  Decoding is done recursively
 *  using given file pointer and Huffman tree.
 *
 *	Arguments: Huffman Tree head node, file pointer
 *
 *	Returns: decoded byte as unsigned char
 */
unsigned char decompressChar(Node *head, FILE *in) {
	if (head == NULL) {
		return -1;
	}
	if (head->terminal) {
		return head->letter;
	}
	unsigned char bit = readBitFromFile(1, in);
	if (bit) {
		return decompressChar(head->right, in);
	} else {
		return decompressChar(head->left, in);
	}
	
}

/*
 *	readTreeFromFile	
 *
 *	Function: Decodes the Huffman tree from the file
 *  header using recursion.  Huffman tree is assumed
 *  to have been written In-Order with non-terminal
 *	nodes defined by 0 and terminal nodes defined by 
 *  1.  Terminal nodes(1) are followed by 8 bits
 *  which are the value of the terminal char.
 *
 *	Arguments: file pointer to Huffman encoded file
 *
 *	Returns: Huffman Tree head node
 */
Node* readTreeFromFile(FILE *in) {
	Node *head = (Node*)malloc(sizeof(Node)*1);
	head->letter = -1;
	head->terminal = 0;
	head->left = NULL;
	head->right = NULL;
	
	unsigned char bit = readBitFromFile(1, in);
	tree_bit_length++;
	
	if (bit) {
		head->letter = readBitFromFile(8, in);
		head->value = 0;
		head->terminal = 1;
		tree_bit_length += 8;
		return head;
	} else {
		head->left = readTreeFromFile(in);
		head->right = readTreeFromFile(in);
		return head;
	}
}

/*
 *  readBitFromFile
 *
 *	Function: Allow bit by bit traversal of a file
 *	using bit twiddling and a global read_bit_count
 *	variable which is the bit index.
 *	
 *	Arguments: Number of bits to read, file pointer
 *
 *	Returns: unsigned char read bit value
 */
unsigned char readBitFromFile(int count, FILE *in) {
	input = 0;
	for (int i = count-1; i >= 0; i--) {
		if (read_bit_count == 8 || read_bit_count == 0) {
			fread(buffer, sizeof(char), 1, in);
			read_bit_count = 0;
		}
		input = (input<<1)+((buffer[0]&(128>>read_bit_count)) == 0 ? 0 : 1);		
		read_bit_count++;
	}
	return input;
}

/*
 *	writeTreeToFile	
 *
 *	Function: Encodes the Huffman tree to the file
 *  header using recursion.  Huffman tree is written 
 *  In-Order with non-terminal nodes defined by 0 
 *  and terminal nodes defined by 1. Any terminal
 *  node write it followed by 8 bits which is the 
 *  value of the terminal char.
 *
 *	Arguments: file pointer to Huffman encoded file
 *
 *	Returns: Huffman Tree head node
 */
void writeTreeToFile(Node *n, FILE *out) {
	if (n->terminal) {
		writeBitToFile(1, 1, out);
		writeBitToFile(8, n->letter, out);
	} else {
		writeBitToFile(1, 0, out);
		writeTreeToFile(n->left, out);
		writeTreeToFile(n->right, out);
	}
}

/*
 *  writeBitToFile
 *
 *	Function: Allow bit by bit writing to a file
 *	using bit twiddling and a global write_bit_count
 *	variable which is the bit index.
 *	
 *	Arguments: Number of bits to read, file pointer
 *
 *	Returns: unsigned char read bit value
 */
void writeBitToFile(int count, char byte, FILE *out) {
	for (int i = count-1; i >=0; i--) {
		output[write_bit_count/8] = (output[write_bit_count/8]<<1) + ((1<<i&byte) == 0 ? 0 : 1);
		write_bit_count++;
		if (write_bit_count == (8*BLOCK_SIZE)) {
			fwrite(output, sizeof(char), BLOCK_SIZE, out);
			write_bit_count = 0;
			for (int i = 0; i < BLOCK_SIZE; i++) {
				output[i] = 0;
			}
		}
	}
}

/*
 *  printUsage
 *
 *	Function: Print usage information for CLI
 *	
 *	Arguments: None
 *
 *	Returns: None
 */
void printUsage(char *argv1) {
	printf("Usage:\n\n%s [options] [source] [destination]\n\tOptions:\n\t\t-c\tCompress\n\t\t-d\tDecompress", argv1);
}

/*
 *  getFileSize
 *
 *  Function: returns file size in bytes
 *
 *  Arguments: Filestream pointer
 *
 *  Returns: int with file size in bytes
 */
unsigned int getFileSize(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	unsigned int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return size;
}