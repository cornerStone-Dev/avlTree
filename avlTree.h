/* avlTree.h */

#ifndef AVL_TREE_HEADER
#define AVL_TREE_HEADER
#include <stdint.h>
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

typedef struct StringTo64Node_s StringTo64Node;

typedef struct StringTo64Node_s {
	StringTo64Node *next[2];
	s64             val;
	s8              longer;
	s8              taken;
	u8              len;
	u8              key[5];
} StringTo64Node;

u32
avlTreeCount(StringTo64Node* tree, u32 count);

u32
avlTreeDepth(StringTo64Node* tree, u32 depth);

StringTo64Node*
StringTos64Tree_find(StringTo64Node* tree, u8 *target);

s32
StringTos64Tree_insert(StringTo64Node* *treep, u8 *target, u32 len, s64 val);



#endif
