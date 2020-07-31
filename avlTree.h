/* avlTree.h */

#ifndef AVLTREE_HEADER
#define AVLTREE_HEADER
#include <stdint.h>

/*******************************************************************************
 * Section Interface for Alterations
*******************************************************************************/
//#define AVLTREE_TREE_DEBUG

#ifdef AVLTREE_TREE_DEBUG
#include <stdio.h>
#endif

#ifndef AVLTREE_CUSTOM_ALLOC
#include <stdlib.h>
#define AVLTREE_FREE   free
#define AVLTREE_MALLOC malloc
#endif

#ifndef AVLTREE_CUSTOM_TYPE
typedef uint64_t AvlValue;
#endif

#ifndef AVLTREE_CUSTOM_CMP
#define CMP(x,y)  (stringCompare((x),(y)))
#endif

#ifdef AVLTREE_STATIC_BUILD
#define STATIC_BUILD static
#else
#define STATIC_BUILD
#endif

/*******************************************************************************
 * Section Types
*******************************************************************************/

typedef struct StringToValNode_s StringToValNode;

typedef struct StringToValNode_s {
	StringToValNode *next[2];
	AvlValue         value;
	int8_t           longer;
	int8_t           taken;
	uint8_t          key[6];
} StringToValNode;

// Main Function API error enumeration
enum {
	// errors
	avlTree_errorNullParam1         = -1,
	avlTree_errorNullParam2         = -2,
	avlTree_errorNullParam3         = -3,
	avlTree_errorTreeIsEmpty        = -4,
	avlTree_errorMallocFailed       = -5,
	// worked as expected
	avlTree_OK                      =  0,
	// not an error, but did not work as expected
	avlTree_nothingFound            =  1,
	avlTree_updatedValOfExistingKey =  2,
};

/*******************************************************************************
 * Section Main Function API
 * Return values are of the enumeration in Types
*******************************************************************************/

STATIC_BUILD
int32_t
avlTree_find(
	StringToValNode *tree,     // pointer to tree
	uint8_t *key,              // pointer to c-string key(null required)
	StringToValNode **result); // address for search result to be written

// convenience function for using integer keys
STATIC_BUILD
int32_t
avlTree_findIntKey(
	StringToValNode *tree,     // pointer to tree
	int64_t key,               // signed integer key
	StringToValNode **result); // address for search result to be written

STATIC_BUILD
int32_t
avlTree_insert(
	StringToValNode **treep, // pointer memory holding address of tree
	uint8_t *key,            // pointer to string key(null termination optional)
	uint32_t keyLen,         // length of key in bytes(not including null)
	AvlValue value);         // value to be stored

// convenience function for using integer keys
STATIC_BUILD
int32_t
avlTree_insertIntKey(
	StringToValNode **treep, // pointer memory holding address of tree
	int64_t key,             // signed integer key
	AvlValue value);         // value to be stored


STATIC_BUILD
int32_t
avlTree_delete(
	StringToValNode **treep, // pointer memory holding address of tree
	uint8_t *key,            // pointer to c-string key(null required)
	AvlValue *value);        // OPTIONAL: pointer to memory for value to written

// convenience function for using integer keys
STATIC_BUILD
int32_t
avlTree_deleteIntKey(
	StringToValNode **treep, // pointer memory holding address of tree
	int64_t key,             // signed integer key
	AvlValue *value);        // OPTIONAL: pointer to memory for value to written

/*******************************************************************************
 * Section Helper/Utility Function API
*******************************************************************************/

#ifdef AVLTREE_TREE_DEBUG
STATIC_BUILD
void
avlTree_debugPrintf(int32_t mainAPIReturnValue);
#endif

STATIC_BUILD
uint32_t
avlTree_count(StringToValNode *tree);

STATIC_BUILD
uint32_t
avlTree_maxDepth(StringToValNode *tree);

STATIC_BUILD
void
avlTree_freeAll(StringToValNode** treep);

STATIC_BUILD
uint32_t
avlTree_s64toString(int64_t input, uint8_t *output);

STATIC_BUILD
int64_t 
avlTree_stringTos64(uint8_t *string);

/*******************************************************************************
 * Section Inline traversal MACRO API
 * This can be used to implement function programming style function or
 * iterators. Such as map, filter, reduce, etc.
 * 
 * Parameter explanation:
 * tree - pointer to tree.
 * 
 * function - function name that takes a pointer to a tree as a first parameter
 * and "parameter" of what ever type as second parameter.
 * 
 * function return value is zero to signal OK to continue or non-zero to exit
 * iteration early. You can use this to execute over a range, etc.
 * 
 * parameter - this is a paramater of your choosing, for example to store state
 * or to pass data to your function.
 * *****************************************************************************
 * 
 * Examples:
 * I want to print out all node keys in-order
 * 	uint32_t print(StringToValNode *tree, uint32_t parameter)
 * 	{
 * 		printf("%s ", tree->key);
 * 		return 0;	
 * 	}
 * 	...
 * 	INORDER_TRAVERSAL(tree, print, 0);
 * 	...
 * 
 * I want to make a copy of the entire tree efficiently 
 * 
uint32_t copy(StringToValNode *tree, StringToValNode **copyStackAddressp)
{
	// get size of node
	uint32_t size =(strlen(tree->key)+1+sizeof(AvlValue)+18);
	size = (size+7)/8*8;// round up to 8 bytes
	// get memory for new node
	StringToValNode *new = AVLTREE_MALLOC(size);
	//make a copy of node
	memcpy(new, tree, size);
	// now the tricky bit
	// check right child first
	if(new->next[1]){
		copyStackAddressp--;
		new->next[1]  = *copyStackAddressp;
	}
	if(new->next[0]){
		copyStackAddressp--;
		new->next[0]  = *copyStackAddressp;
	}
	// branchless version if you need speed
	//uint64_t val = new->next[1]!=0;
	//copyStackAddressp-=val;
	//new->next[1] = (StringToValNode*)(((uint64_t)*copyStackAddressp)&(-val));
	//uint64_t val2 = new->next[0]!=0;
	//copyStackAddressp-=val2;
	//new->next[0] = (StringToValNode*)(((uint64_t)*copyStackAddressp)&(-val2));

	// put yourself on the stack
	*copyStackAddressp = new;
	copyStackAddressp++;

	return 0;	
}
 * ...
	StringToValNode *copyStack[48];
	StringToValNode **copyStackAddressp = &copyStack[0];
 * ...
	POSTORDER_TRAVERSAL(tree, copy, copyStackAddressp);
	// result
	StringToValNode *newTree = *(copyStackAddressp-1);
 * 
*******************************************************************************/

#define INORDER_TRAVERSAL(tree, function, parameter) \
do{ \
	StringToValNode *stack[48]; \
	StringToValNode *treep = tree; \
	uint64_t index = 0; \
	while(1) \
	{ \
		while(1) \
		{ \
			if(treep==0){ \
				break; \
			} \
			stack[index]= treep; \
			index++; \
			treep = treep->next[0]; \
		} \
		if(index==0){ \
			break; \
		} \
		index--; \
		treep = stack[index]; \
		if(function(treep, parameter)){ \
			break; \
		} \
		treep = treep->next[1]; \
	} \
}while(0)

#define PREORDER_TRAVERSAL(tree, function, parameter) \
do{ \
	__label__ EXIT; \
	StringToValNode *stack[48]; \
	StringToValNode *treep = tree; \
	uint64_t index = 0; \
	while(1) \
	{ \
		while(1) \
		{ \
			if(treep==0){ \
				break; \
			} \
			if(function(treep, parameter)){ \
				goto EXIT; \
			} \
			stack[index]= treep; \
			index++; \
			treep = treep->next[0]; \
		} \
		if(index==0){ \
			break; \
		} \
		index--; \
		treep = stack[index]; \
		treep = treep->next[1]; \
	} \
	EXIT: ;\
}while(0)

#define POSTORDER_TRAVERSAL(tree, function, parameter) \
do{ \
	__label__ T4; \
	StringToValNode *stack[48]; \
	StringToValNode *treep = tree; \
	StringToValNode *last = 0; \
	uint64_t index = 0; \
	while(1) \
	{ \
		while(1) \
		{ \
			if(treep==0){ \
				break; \
			} \
			stack[index]= treep; \
			index++; \
			treep = treep->next[0]; \
		} \
		T4: \
		if(index==0){ \
			break; \
		} \
		treep = stack[index-1]; \
		if( (treep->next[1] == 0) || (treep->next[1] == last) ){ \
			index--; \
			if(function(treep, parameter)){ \
				break; \
			} \
			last = treep; \
			goto T4; \
		} else { \
			treep = treep->next[1]; \
		} \
	} \
}while(0)

#endif
