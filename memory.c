#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  
  if(argc < 2) {
    printf("No file provided here\n");
    return 1;
  }
  FILE *file = fopen(argv[1], "r"); 
  
  //Get the memorySize and the algo
  int memorySize, algo;
  fscanf(file, "%d", &memorySize);
  fscanf(file, "%d", &algo);
  node_t *head = makeNode(1, 0, memorySize, NULL, NULL);
  
  //initialize var list and all to null 
  node_t *vars[25];
  initVarList(vars);
 
   //probably fair to assume that func will not be -1 for the first round
  int func, var, size;

  int outOfSpaceFlag = 0; //if we are out of space, set this to 1 and exit
  
  //This code will handle all lines of input until the end
  while(1) {
    getNextLine(file, &func, &var, &size); //get next line of input

    if(func == -1 || outOfSpaceFlag) break; //probably will need to add some other logic here later
    
    switch(func) {
      case MALLOC: {
        node_t *spaceNode = findFree(head, size, algo); //gives us the node we will need to malloc into
        if(spaceNode == NULL) {
          outOfSpaceFlag = 1;
          break;
        } 

        if(size == spaceNode->size) {
          spaceNode->isFree = 0;
        } else {
          split(spaceNode, size);
        } 
        vars[var] = spaceNode;
        break;
      }

      case FREE: {
        //Get pointer from list, set to free, set var list index to NULL and check for coalesce
        node_t *freeNode = vars[var];

        if(freeNode != NULL) {
          freeNode->isFree = 1;
          vars[var] = NULL;
          coalesce(freeNode);
        }

        break;
      }
      case REALLOC: {
        //free first, coalesce, and then try to malloc maybe?
        node_t *freeNode = vars[var];
        if(freeNode != NULL) {
          freeNode->isFree = 1;
          vars[var] = NULL;
          coalesce(freeNode);
        }
        
        //now same code from malloc 
        node_t *spaceNode = findFree(head, size, algo);
        if(spaceNode == NULL) {
          outOfSpaceFlag = 1;
          break;
        }

        if(size == spaceNode->size) {
          spaceNode->isFree = 0; //mark as allocated 
        } else {
          split(spaceNode, size);
        }

        vars[var] = spaceNode;
        break;
      }
    }  
  }
  fclose(file); 
  printResults(head, vars, outOfSpaceFlag);
  freeLinkedList(head);
  return 0;
}

node_t *makeNode(int isfree, int start, int size, node_t *prev, node_t *next) {

  node_t *newNode = (node_t *)malloc(sizeof(node_t));

  if(newNode == NULL) {
    printf("mem alloc failed\n");
    return NULL;
  }

  newNode->isFree = isfree;
  newNode->start = start;
  newNode->size = size;
  newNode->prev = prev;
  newNode->next = next;

  return newNode;
}
//untested
void printList(node_t *p) {
  node_t *temp = p;
  
  printf("List:\n");
  while(temp != NULL) {
    printf("(%d,%d,%d)\n", temp->isFree, temp->start, temp->size);
    temp = temp->next;
  }
}

//untested
void printVars(node_t *p[]) {
  //use index for var, then print start and size
  printf("Vars:\n");
  for(int i = 0; i < 25; i++) {
    if(p[i] != NULL) {
      printf("(%d,%d,%d)\n", i, p[i]->start, p[i]->size);
    }
  }
}

void split(node_t *p, int size) {
  //this will handle insertion
  //if this is being run, we know space is available 
  node_t *freeNode = makeNode(1, p->start + size, p->size - size, p, p->next); //insert allocated space on left

  if(p->next != NULL) {
    p->next->prev = freeNode; //adjust pointer to point to free block
  }


  p->next = freeNode; //free space comes after allocated (insert at beginning)
  p->size = size; //make allocated node size of requested 
  p->isFree = 0; //set to allocated
  
}

//will need to call this after any free() and realloc(), will not always occur
void coalesce(node_t *p) {

  //check left side 
  if(p->prev != NULL && p->prev->isFree) {
    p->prev->size += p->size; //join the size of current and left node
    p->prev->next = p->next; //skip p
      
    if(p->next != NULL) {
        p->next->prev = p->prev; //make combined node point back to other node in list
    }
      
    node_t *temp = p; //make p the previous node, then we can free temp
    p = p->prev; //move back p
    free(temp);
  }

  //now right side 
  if(p->next != NULL && p->next->isFree) {
    p->size += p->next->size; //same as for left side
    node_t *temp = p->next;  //temp is block we are going to free
    p->next = p->next->next; //skip the one we are about to delete
      
    if(p->next != NULL) { //if some node exists after deleting
        p->next->prev = p;
    }
      
    free(temp); 
  }
    
}

//Need to complete this later on
//return point to the node that is going to be split
node_t *findFree(node_t *h, int size, int algo) {
  node_t *temp = h; //temporary head for traversal
  node_t *fitNode = NULL; //this will be the pointer we return once we get the node that is going to be split
  switch(algo) {
    //O(n)
    case BEST_FIT:
      //best fit code
      //traverse list, keep track of all spaces available and take smallest one
        while(temp != NULL) {
        if(temp->isFree && temp->size >= size) { //if space is free and fits it could work
          if(fitNode == NULL || temp->size < fitNode->size) { //if still null or we find smaller space
            fitNode = temp;
          }
        }
        temp = temp->next;
      }
      break;
    case WORST_FIT:
      //worst fit code
      //Basically going to be the same as best fit just greater than in if statement
      while(temp != NULL) {
        if(temp->isFree && temp->size >= size) { //same logic as best fit
          if(fitNode == NULL || temp->size > fitNode->size) {
            fitNode = temp;
          }
        }
        temp = temp->next;
      }
      break;
    case FIRST_FIT:
      //first fit code
      //also basically the same as before but just take first one that works
      while(temp != NULL) {
        if(temp->isFree && temp->size >= size) {
          fitNode = temp;
          break;
        }
        temp = temp->next;
      }
      break; 
    }

  return fitNode; //if null we know we cannot possibly do it
}

//func can be malloc, free or realloc, var could be 0-24 and size is optional
void getNextLine(FILE *file, int *func, int *var, int *size) {
  int status = fscanf(file, "%d", func);

  if(*func == -1) {
      return; //end of file, stop the read
  } else if(*func == FREE) {
      fscanf(file, "%d", var);
      return;
  } else {
     fscanf(file, "%d", var);
     fscanf(file, "%d", size); 
  }
}

//Just use this to fill the array to null at the start
//then we can just set p[i] to be the pointer of the var
//we are dealing with
void initVarList(node_t *p[]) {
  for(int i = 0; i < 25; i++) {
    p[i] = NULL;
  }
}

//This will be triggered when the end is reached or we run out of space
void printResults(node_t *h, node_t *p[], int outOfSpace) {
  //if outOfSpace == 1, then we print out of space 
  if(outOfSpace) {
    printf("ERROR: Out of Space\n");
    printf("\n");
  }
  
  //print nodes
  printList(h);
  
  printf("\n");

  //print vars 
  printVars(p);
}

void freeLinkedList(node_t *h) {
  node_t *temp;
  
  while(h != NULL) {
    temp = h;
    h = h->next;
    free(temp);
  }
}
