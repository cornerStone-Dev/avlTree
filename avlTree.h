/* avlTree.h */

#ifndef AVL_TREE_HEADER
#define AVL_TREE_HEADER
#include <stdint.h>

/*******************************************************************************
 * INTERFACE FOR ALTERATIONS
*******************************************************************************/

#ifndef AVL_CUSTOM_TYPE
typedef uint64_t AvlValue;
#endif

#ifndef AVL_CUSTOM_CMP
#define CMP(x,y)  (stringCompare((x),(y)))
#endif

#ifdef STATIC_BUILD
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
	AvlValue        val;
	int8_t          longer;
	int8_t          taken;
	uint8_t         key[6];
} StringToValNode;

/*******************************************************************************
 * Function API
*******************************************************************************/

STATIC_BUILD
uint32_t
avlTree_count(StringToValNode* tree);

STATIC_BUILD
uint32_t
avlTree_maxDepth(StringToValNode* tree);

STATIC_BUILD
void
avlTree_freeAll(StringToValNode** treep);

STATIC_BUILD
StringToValNode*
avlTree_find(StringToValNode *tree, uint8_t *key);

STATIC_BUILD
int32_t
avlTree_insert(
	StringToValNode **treep,
	uint8_t *key,
	uint32_t keyLen,
	AvlValue val);

STATIC_BUILD
int32_t
avlTree_delete(StringToValNode **treep, uint8_t *key, AvlValue *val);

STATIC_BUILD
uint32_t
avlTree_s64toString(int64_t input, uint8_t *output);

STATIC_BUILD
int64_t 
avlTree_stringTos64(uint8_t *string);

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
