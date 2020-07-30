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

typedef struct StringToValNode_s StringToValNode;

typedef struct StringToValNode_s {
	StringToValNode *next[2];
	AvlValue        val;
	int8_t          longer;
	int8_t          taken;
	uint8_t         key[6];
} StringToValNode;

uint32_t
avlTreeCount(StringToValNode *tree, uint32_t count);

uint32_t
avlTreeDepth(StringToValNode *tree, uint32_t depth);

StringToValNode*
avlTree_find(StringToValNode *tree, uint8_t *key);

int32_t
avlTree_insert(StringToValNode **treep, uint8_t *key, uint32_t keyLen, AvlValue val);

int32_t
avlTree_delete(StringToValNode **treep, uint8_t *key);

#define INORDER_TRAVERSAL(tree, function, param) \
do{ \
	if(tree) { \
		StringToValNode *stack[48]; \
		StringToValNode *treep = tree; \
		u32 index = 0; \
		u32 x = 0; \
		\
		do{ \
			stack[x]=0; \
			x++; \
		}while(x<48); \
		\
		while(1) \
		{ \
			while(1) \
			{ \
				if(treep==0){ \
					break; \
				} \
				\
				stack[index]= treep; \
				index++; \
				treep = treep->next[0]; \
			} \
			\
			if(index==0){ \
				break; \
			} \
			\
			index--; \
			treep = stack[index]; \
			if(function(treep, param)==0){ \
				break; \
			} \
			treep = treep->next[1]; \
		} \
	} \
}while(0) \

#endif
