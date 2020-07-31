/* avlTree.h */

#ifndef AVLTREE_HEADER
#define AVLTREE_HEADER
#include <stdint.h>

/*******************************************************************************
 * INTERFACE FOR ALTERATIONS
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
 * Types
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
	avlTree_errorNullParam1   = -1,
	avlTree_errorNullParam2   = -2,
	avlTree_errorNullParam3   = -3,
	avlTree_errorMallocFailed = -4,
	// worked as expected
	avlTree_OK = 0,
	// not an error, but did not work as expected
	avlTree_nothingFound = 1,
	avlTree_updatedValOfExistingKey = 2,
};

/*******************************************************************************
 * Main Function API
*******************************************************************************/

STATIC_BUILD
int32_t
avlTree_find(StringToValNode *tree, uint8_t *key, StringToValNode **result);

STATIC_BUILD
int32_t
avlTree_findIntKey(StringToValNode *tree, int64_t key, StringToValNode **result);

STATIC_BUILD
int32_t
avlTree_insert(
	StringToValNode **treep,
	uint8_t *key,
	uint32_t keyLen,
	AvlValue value);

STATIC_BUILD
int32_t
avlTree_insertIntKey(
	StringToValNode **treep,
	int64_t key,
	AvlValue value);

STATIC_BUILD
int32_t
avlTree_delete(
	StringToValNode **treep,
	uint8_t *key,
	AvlValue *value);

STATIC_BUILD
int32_t
avlTree_deleteIntKey(
	StringToValNode **treep,
	int64_t key,
	AvlValue *value);

/*******************************************************************************
 * Helper/Utility Function API
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
 * Inline traversal MACRO API
 * This can be used to implement function programming style function or
 * iterators. Such as map, filter, reduce, etc.
*******************************************************************************/

#define INORDER_TRAVERSAL(tree, function, param) \
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
		if(function(treep, param)==0){ \
			break; \
		} \
		treep = treep->next[1]; \
	} \
}while(0)

#define PREORDER_TRAVERSAL(tree, function, param) \
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
			if(function(treep, param)==0){ \
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

#define POSTORDER_TRAVERSAL(tree, function, param) \
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
			if(function(treep, param)==0){ \
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
