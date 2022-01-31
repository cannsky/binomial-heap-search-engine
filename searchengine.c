#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

/*
Program Definition: A program for searching the keyword the user entered from the 42 content files.

The program uses the frequency for finding the best articles/websites etc. for user.

The program uses a binomial heap structure with header and binomial node.
*/

//Node of Binomial Heap
typedef struct BinomialNode {
	int key;
	char articleIndex[100];
	int degree;
	struct BinomialNode *parent;
	struct BinomialNode *sibling;
	struct BinomialNode *child;
} BinomialNode;

//Structure of Binomial Heap
typedef struct BinomialHeap {
	struct BinomialNode *header;
} BinomialHeap;

//Function for adding a new node to the heap
BinomialNode* addNewNode(int key, char *articleIndex){
	//Allocate memory for the new node
	BinomialNode *newNode = malloc(sizeof(BinomialNode));
	//Set key to key
	newNode->key = key;
	//Set degree 0 since it's at max depth level
	newNode->degree = 0;
	//Set article index
	strcpy(newNode->articleIndex, articleIndex);
	//Set parent, sibling and child to NULL
	newNode->parent = newNode->sibling = newNode->child = NULL;
	//Return the new node
	return newNode;
}

char multiDirectory[100][100];

int changed = 0;

void printFile(char *text){
	FILE *file;
	file = fopen(text, "r");
	//If the file is not opened return false
	if(!file) return;
	char inputString[255], word[50], *token;
	int id, frequency;
	//Read input file line by line
	while(fgets(inputString, 255, file) != NULL){
		printf("%s", inputString);
	}
}

/*

Connect trees method binds two binomial trees together.
This method gets the two ordered trees to connect them.
FirstTreeChild is the child root for connecting the secondTree to the root node.
But if the first three child is occupied the binomial heap algorithm will look for the siblings of the firstTreeChild.

*/

BinomialNode *connectTrees(BinomialNode* firstTreeChild, BinomialNode* secondTree){
	if(firstTreeChild == NULL){
		firstTreeChild = secondTree;
		changed = 1;
		return firstTreeChild;
	}
	firstTreeChild->sibling = connectTrees(firstTreeChild->sibling, secondTree);
	return firstTreeChild;
}

BinomialNode *swapTempTree;

int swapIndex = 0;

/*
Increase degree method works with the connectTrees and mergeTrees methods.
This method basically updates the degree of the nodes in the merged trees.
Since only the first tree (max heap tree) gets extra degree, degree of the tree increased by 1
If there are null trees the algorithm will return and the recursive function will be over.
*/

BinomialNode *increaseDegree(BinomialNode* firstTree, BinomialNode* secondTree){
	changed = 1;
	if(firstTree == NULL || secondTree == NULL) return firstTree;
	firstTree->degree = secondTree->degree + 1;
	if(firstTree->child != NULL) firstTree->child = increaseDegree(firstTree->child, secondTree->child);
	if(firstTree->sibling != NULL) firstTree->sibling = increaseDegree(firstTree->sibling, secondTree->sibling);
	return firstTree;
}

int tempValue = 0;

/*
Degree fix function fixes the degree after the max node removed.
When the max node removed, the childs will loose their degrees. But since
it is not stable, the childs must be looked single by single to find their degree.
This function looks to the every element of the tree and updates the degree's accourding to them
using same technique as tree traversal.
*/
void degreeFix(int setDegree, BinomialNode* tempTree){
	if(tempTree == NULL) return;
	tempTree->degree = setDegree;
	if(tempTree->sibling != NULL) degreeFix(setDegree, tempTree->sibling);
	if(tempTree->child != NULL) degreeFix(setDegree-1, tempTree->child);
}

/*
Decrease degree function is the first step of the degreefix function,
in this function max nodes' childs degree's will be lowered by 1.
After that recursive degreefix function will be called.
*/

BinomialNode *decreaseDegree(int maxNodeDegree, BinomialNode* tempTree){
	if(tempTree->sibling != NULL) tempTree->sibling = decreaseDegree(maxNodeDegree, tempTree->sibling);
	tempTree->degree = maxNodeDegree - 1 - tempValue;
	tempValue++;
	degreeFix(tempTree->degree - 1, tempTree->child);
	return tempTree;
}

/*
Update swap temp tree, creates a new temp tree for changing the first and second trees if
the second tree is greater than the first one. The function uses sibling child traversals to 
copy the first tree and then this tree will be used in the swapping trees section.
*/
BinomialNode *updateSwapTempTree(BinomialNode* swapTempTreeLocal, BinomialNode* firstTree){
	if(swapIndex++ == 0){
		swapTempTreeLocal = malloc(sizeof(BinomialNode));
		swapTempTreeLocal->key = firstTree->key;
		strcpy(swapTempTreeLocal->articleIndex, firstTree->articleIndex);
		swapTempTreeLocal->child = updateSwapTempTree(swapTempTreeLocal->child, firstTree->child);
	}
	else if(firstTree != NULL){
		swapTempTreeLocal = malloc(sizeof(BinomialNode));
		swapTempTreeLocal->key = firstTree->key;
		strcpy(swapTempTreeLocal->articleIndex, firstTree->articleIndex);
		swapTempTreeLocal->degree = firstTree->degree;
		if(firstTree->sibling != NULL) swapTempTreeLocal->sibling = updateSwapTempTree(swapTempTreeLocal->sibling, firstTree->sibling);
		if(firstTree->child != NULL) swapTempTreeLocal->child = updateSwapTempTree(swapTempTreeLocal->child, firstTree->child);
	}
	return swapTempTreeLocal;
};

/*
swap first tree gets the information of the second tree and only updates the keys and article index of it.
the degree and the other connections of the nodes will be the same.
*/
BinomialNode *swapFirstTree(BinomialNode *firstTree, BinomialNode *secondTree){
	if(swapIndex++ == 0){
		firstTree->key = secondTree->key;
		strcpy(firstTree->articleIndex, secondTree->articleIndex);
		firstTree->child = swapFirstTree(firstTree->child, secondTree->child);
	}
	else if(firstTree != NULL && secondTree != NULL){
		firstTree->key = secondTree->key;
		strcpy(firstTree->articleIndex, secondTree->articleIndex);
		if(secondTree->sibling != NULL) firstTree->sibling = swapFirstTree(firstTree->sibling, secondTree->sibling);
		if(secondTree->child != NULL) firstTree->child = swapFirstTree(firstTree->child, secondTree->child);
	}
	return firstTree;
}

/*
The same function as the first tree's swap function. It only changes the second tree.
*/
BinomialNode *swapSecondTree(BinomialNode* swapTempTreeLocal, BinomialNode *secondTree){
	if(swapIndex++ == 0){
		secondTree->key = swapTempTreeLocal->key;
		strcpy(secondTree->articleIndex, swapTempTreeLocal->articleIndex);
		swapSecondTree(swapTempTreeLocal->child, secondTree->child);
	}
	else if(swapTempTreeLocal != NULL && secondTree != NULL){
		secondTree->key = swapTempTreeLocal->key;
		strcpy(secondTree->articleIndex, swapTempTreeLocal->articleIndex);
		if(swapTempTreeLocal->sibling != NULL) secondTree->sibling = swapSecondTree(swapTempTreeLocal->sibling, secondTree->sibling);
		if(swapTempTreeLocal->child != NULL) secondTree->child = swapSecondTree(swapTempTreeLocal->child, secondTree->child);
	}
	swapTempTreeLocal = NULL;
	return secondTree;
}

/*
merge trees will be called if there are avaliable trees to merge,
since the binomial heap's structure can only have one degree tree at the same time,
merge tree uses swap tree, increase degree, connect tree methods to merge the trees
after the trees merged the first tree ( max tree ) will be returned to be added to the heap.
*/
BinomialNode *mergeTrees(BinomialNode* firstTree, BinomialNode* secondTree){
	if(firstTree == NULL || secondTree == NULL) return firstTree;
	if(firstTree->degree != secondTree->degree) return firstTree;
	if(secondTree->key > firstTree->key){
		swapIndex = 0;
		swapTempTree = updateSwapTempTree(swapTempTree, firstTree);
		swapIndex = 0;
		firstTree = swapFirstTree(firstTree, secondTree);
		swapIndex = 0;
		secondTree = swapSecondTree(swapTempTree, secondTree);
	}
	if(secondTree->sibling != NULL) firstTree->sibling = secondTree->sibling;
	else if(secondTree->sibling == NULL) firstTree->sibling = NULL;
	secondTree->sibling = NULL;
	increaseDegree(firstTree, secondTree);
	firstTree->child = connectTrees(firstTree->child, secondTree);
	return firstTree;
}

/*
Fix heap looks for the broken nodes after an initilization or removing the max node.
fix heap calls the merge trees function for merging the trees.
*/
BinomialNode *fixHeap(BinomialNode* binomialHeapRoot){
	binomialHeapRoot = mergeTrees(binomialHeapRoot, binomialHeapRoot->sibling);
	if(binomialHeapRoot->sibling == NULL) return binomialHeapRoot;
	if(binomialHeapRoot->sibling->sibling != NULL) binomialHeapRoot->sibling = fixHeap(binomialHeapRoot->sibling);
	return binomialHeapRoot;
}

BinomialNode *maxNode = NULL;
BinomialNode *lastNode = NULL;

/*
Find maximum node looks for the every root of the every binomial tree in the binomial heap structure,
max node will be saved to be used in removing the max node.
*/
BinomialNode* findMaximumNode(BinomialNode* binomialHeapRoot){
	if(binomialHeapRoot == NULL) return binomialHeapRoot;
	if(maxNode == NULL) maxNode = binomialHeapRoot;
	else if(binomialHeapRoot->key > maxNode->key){
		maxNode = binomialHeapRoot;
	}
	if(binomialHeapRoot->sibling != NULL) {
		if(maxNode != binomialHeapRoot) lastNode = binomialHeapRoot;
		binomialHeapRoot->sibling = findMaximumNode(binomialHeapRoot->sibling);
	}
	return binomialHeapRoot;
}

/*
find maximum function calls find maximum node to find the max node.
*/
BinomialHeap* findMaximum(BinomialHeap* binomialHeap, BinomialNode* binomialHeapRoot){
	if(binomialHeapRoot == NULL) return binomialHeap;
	binomialHeap->header = binomialHeapRoot;
	binomialHeapRoot = findMaximumNode(binomialHeapRoot);
	return binomialHeap;
}

/*
When the node have a degree higher than the root, the binomial heap algorithm uses
the find place for node function to find the place of the root with looking to its degree.
*/
BinomialNode* findPlaceForNode(BinomialNode* heapNode, BinomialNode* tempTree){
	if(tempTree == NULL) return heapNode;
	if(heapNode == NULL){
		heapNode = tempTree;
		return heapNode;
	}
	if(heapNode->degree >= tempTree->degree){
		BinomialNode* tempNode = heapNode;
		heapNode = tempTree;
		tempTree->sibling = heapNode;
	}
	if(heapNode->degree < tempTree->degree){
		heapNode->sibling = findPlaceForNode(heapNode->sibling, tempTree);
	}
	return heapNode;
}

BinomialNode *tempNode = NULL;

/*
Update heap structure is being used when there is a root with higher degree than the root.
The update heap structure function checks the degrees and sends the node to the find place for the node.
This function is being called when the max node removed.
*/

BinomialHeap* updateHeapStructure(BinomialHeap* binomialHeap, BinomialNode* tempTree){
	if(tempTree == NULL) return binomialHeap;
	tempNode = tempTree->sibling;
	tempTree->sibling = NULL;
	if(binomialHeap->header == NULL) binomialHeap->header = tempTree;
	else {
		if(binomialHeap->header->degree >= tempTree->degree){
			BinomialNode* tempNode = binomialHeap->header;
			binomialHeap->header = tempTree;
			binomialHeap->header->sibling = tempNode;
			fixHeap(binomialHeap->header);
		}
		if(binomialHeap->header->degree < tempTree->degree){
			binomialHeap->header->sibling = findPlaceForNode(binomialHeap->header->sibling, tempTree);
			fixHeap(binomialHeap->header);
		}
	}
	return binomialHeap;
}

/*
remove node recursively calls itself to find a new place for the childs of the removed max node.
*/
BinomialHeap* removeNode(BinomialHeap* binomialHeap, BinomialNode* newTree){
	if(newTree != NULL){
		binomialHeap = updateHeapStructure(binomialHeap, newTree);
		if(tempNode != NULL){
			binomialHeap = removeNode(binomialHeap, tempNode);
		}
	}
	return binomialHeap;
}

/*
remove maximum node removes the maximum node by removing its pointer from the binomial heap structure,
after that uses remove node function and decrease degree function to update the structure and fix the
binomial heap.
*/
BinomialHeap* removeMaximumNode(BinomialHeap* binomialHeap, BinomialNode* binomialHeapRoot){
	if(lastNode != NULL){
		if(maxNode == NULL || lastNode == NULL) return binomialHeap;
		//Some node to be removed
		if(maxNode != NULL && maxNode != lastNode){
			lastNode->sibling = maxNode->sibling;
		}
	}
	else{
		//Root to be removed
		if(binomialHeap->header != lastNode) binomialHeap->header = lastNode;
	}
	tempValue = 0;
	if(maxNode != NULL && lastNode != NULL) decreaseDegree(maxNode->degree, maxNode->child);
	binomialHeap = removeNode(binomialHeap, maxNode->child);
	maxNode = NULL;
	lastNode = NULL;
	return binomialHeap;
}

/*
This function is being used for the extraction of the highest node.
Uses the other functions mentioned above.
*/
BinomialHeap *extractTop(BinomialHeap** binomialHeap, BinomialNode** binomialHeapRoot){
	findMaximum(*binomialHeap, *binomialHeapRoot);
	printf("\n%d - keywords found for the search.", maxNode->key);
	printf("\n\nArticle name: %s -> \n", maxNode->articleIndex);
	printFile(maxNode->articleIndex);
	removeMaximumNode(*binomialHeap, *binomialHeapRoot);
	return *binomialHeap;
}

//Function for initilazing the heap
BinomialNode* initHeap(BinomialHeap** binomialHeap, BinomialNode** binomialHeapRoot, int key, char* articleIndex){
	//If there is no binomial heap create a new one with a new tree
	if(*binomialHeapRoot == NULL) return addNewNode(key, articleIndex);
	//If degree is equal to zero
	if((*binomialHeapRoot)->degree == 0){
		BinomialNode *tempNode = (*binomialHeapRoot)->sibling;
		//Add the new node next to the current node
		(*binomialHeapRoot)->sibling = addNewNode(key, articleIndex);
		(*binomialHeapRoot)->sibling->sibling = tempNode;
	}
	else if((*binomialHeapRoot)->degree > 0){
		//Set current node as the temp node
		BinomialNode *tempNode = (*binomialHeapRoot);
		//Set root as a new node
		(*binomialHeapRoot) = addNewNode(key, articleIndex);
		//Set sibling as tempNode
		(*binomialHeapRoot)->sibling = tempNode;
	}
	//Fix the heap
	*binomialHeapRoot = fixHeap(*binomialHeapRoot);
	while(changed == 1){
		changed = 0;
		*binomialHeapRoot = fixHeap(*binomialHeapRoot);
	}
	//Return the new binomial heap root
	return (*binomialHeapRoot);
}

//for debugging.
void printHeap(BinomialNode *root){
	if(root == NULL) return;
	printf("%d \n", root->key);
	printf("%d \n", root->degree);
	printf("looking for child.\n");
	printHeap(root->child);
	printf("looking for sibling.\n");
	printHeap(root->sibling);
}

FILE *file;
char keyword[100] = "\0";

/*
main function looks for the directory of the program
after looking to the directory collects every document with content_ naming
the documents will be opened by the program and the frequencies of the keywords will be checked
by the keyword that the user entered. If there are matches, the documents' data will be saved to
the binomial heap and the heap will be fixed by the binomial heap fix algorithm.
After the initalization of the binomial heap the program extracts the top five nodes from the
binomial heap and prints them to the user.
*/
int main(int argc, char *argv[]) {
	//Create binomialHeapRoot
	BinomialNode *binomialHeapRoot = NULL;
	//Create binomialHeapRoot
	BinomialHeap *binomialHeap = NULL;
	binomialHeap = malloc(sizeof(binomialHeap));
	binomialHeap->header = NULL;
	printf("Enter a keyword to search: ");
	scanf("%s", keyword);
	DIR *d;
	struct dirent *directory;
	d = opendir(".");
	int k = 0;
	int count = 0;
	int relevantDocuments = 0;
	int totalCount = 0;
	int id = 0;
	if(d){
		while((directory=readdir(d)) != NULL){
			char inputString[256], word[50], *token, *token2, *token3, directory_name[100];
			int id, frequency;
			strcpy(directory_name, directory->d_name);
			token = strtok(directory_name, "_");
			if(strcmp(token, "content") == 0){
				file = fopen(directory->d_name, "r");
				//If the file is not opened return false
				if(!file) return 0;
				//Read input file line by line
				//strcpy(multiDirectory[id], directory->d_name);
				while(fgets(inputString, 256, file) != NULL){
					//Get the id from line string
					token = strtok(inputString, " ");
					//Get word
					while(token != NULL){
						token = strtok(NULL, " ");
						if(token != NULL){
							for(k = 0; k < strlen(token); k++){
								if(isalpha(token[k]) == 0){
									token[k] = '\0';
								}
							}
							if(strcmp(keyword, token) == 0){
								count++;
								totalCount++;
							}
						}
					}
				}
				if(count > 0){
					relevantDocuments++;
					binomialHeapRoot = initHeap(&binomialHeap, &binomialHeapRoot, count, directory->d_name);
					count = 0;
				}
			}
		}
	}
	printf("\nNumber of relevant documents found with your search is %d\n\n", relevantDocuments);
	printf("Number of keywords found with your search is %d\n", totalCount);
	int x = 0;
	while(x < 5){
		if(binomialHeap != NULL && binomialHeapRoot != NULL){
			binomialHeap = extractTop(&binomialHeap, &binomialHeapRoot);
			binomialHeapRoot = binomialHeap->header;
			x++;
		}
	}
}
