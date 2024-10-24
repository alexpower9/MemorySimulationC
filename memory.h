/*
** Memory header
*/

#include <stddef.h>
#include <stdio.h>
// Memory API calls
#define MALLOC  1
#define FREE    2
#define REALLOC 3

// Algorithms
#define BEST_FIT  1
#define WORST_FIT 2
#define FIRST_FIT 3

#define NUM_VARS 25

// Doubly-linked list node
typedef struct memNode {
  int isFree;
  int start;
  int size;
  struct memNode *prev;
  struct memNode *next;
} node_t;

// Function to create a node
node_t *makeNode(int isfree, int start, int size, node_t *prev, node_t *next);

// Function to print the memory list
void printList(node_t *p);

// Function to print the variable list
void printVars(node_t *p[]);

// Function to split a node allocate at beginning of free block
void split(node_t *p, int size);

// Function to coalesce nodes
void coalesce(node_t *p);

// Function to find free space to allocat
node_t *findFree(node_t *h, int size, int algo);

void getNextLine(FILE *file, int *func, int *var, int *size);

void initVarList(node_t *p[]);

void printResults(node_t *h, node_t *p[], int outOfSpace);

void freeLinkedList(node_t *h);
