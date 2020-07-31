/* avlTree.c */

#include "avlTree.h"

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;

#define LEFT     0
#define RIGHT    1
#define NEITHER -1
#define Balanced(n) ((n)->longer < 0)

static inline s32
stringCompare(u8 *str1, u8 *str2) __attribute__((always_inline));

static inline s32
stringCompare(u8 *str1, u8 *str2)
{
	s32 c1, c2;
	
	while(1){
		c1=*str1;
		str1+=1;
		c2=*str2;
		str2+=1;
		c1-=c2;
		if( (c1!=0) || (c2==0) ){
			return c1;
		}
	}
}

#ifdef AVLTREE_TREE_DEBUG
STATIC_BUILD
void
avlTree_debugPrintf(s32 mainAPIReturnValue)
{
	s32 x = mainAPIReturnValue;
	switch(x){
		case avlTree_errorNullParam1:
		printf("avlTree Error: First parameter provided is NULL(0).\n");
		break;
		case avlTree_errorNullParam2:
		printf("avlTree Error: Second parameter provided is NULL(0).\n");
		break;
		case avlTree_errorNullParam3:
		printf("avlTree Error: Third parameter provided is NULL(0).\n");
		break;
		case avlTree_errorMallocFailed:
		printf("avlTree Error: Malloc was called and returned NULL(0).\n");
		break;
		case avlTree_OK:
		printf("avlTree OK: Everything worked as intended.\n");
		break;
		case avlTree_nothingFound:
		printf("avlTree Status: Search for node terminated with nothing.\n");
		break;
		case avlTree_updatedValOfExistingKey:
		printf("avlTree Status: Existing key found and value updated.\n");
		break;
		default:
		printf("avlTree Default: This value is not enumerated."
		       " Debug has no information for you\n");
	}
}
#endif

static StringToValNode *
avlTree_find_internal(StringToValNode *tree, u8 *key)
{
	s32 res;
	while ( (res=CMP(key, tree->key))!=0 ) {
		s8 next_step = (res>0);
		tree = tree->next[next_step];
		if(tree==0){
			break;
		}
	}
	return tree;
}

STATIC_BUILD
s32
avlTree_find(StringToValNode *tree, u8 *key, StringToValNode **result)
{
	StringToValNode *internalResult;
	if(tree==0){
		return avlTree_errorNullParam1;
	}
	if(key==0){
		return avlTree_errorNullParam2;
	}
	if(result==0){
		return avlTree_errorNullParam3;
	}
	
	internalResult = avlTree_find_internal(tree, key);
	
	if(internalResult==0){
		return avlTree_nothingFound;
	}

	*result = internalResult;
	return avlTree_OK;
}

STATIC_BUILD
s32
avlTree_findIntKey(StringToValNode *tree, s64 key, StringToValNode **result)
{
	u8 keyBuffer[16];
	(void)avlTree_s64toString(key, keyBuffer);
	return avlTree_find(tree, keyBuffer, result);
}

static u32
getNodeLen(u32 keyLen)
{
	// return nodeLen in bytes. Assume null termination, add 1
	return (keyLen+1+sizeof(AvlValue)+18+7)/8*8; // round up to 8 bytes
}

static inline StringToValNode*
makeNode(u8 *key, u32 keyLen, AvlValue value)
{
	u32 i=0;
	u32 node_len = getNodeLen(keyLen);
	StringToValNode *tree = AVLTREE_MALLOC(node_len);
	
	if (tree) {
		tree->next[0] = tree->next[1] = 0;
		tree->value = value;
		tree->longer = NEITHER;
		tree->taken = NEITHER;
		do{tree->key[i]=key[i];i++;}while(i<keyLen);
		tree->key[keyLen] = 0; // null terminate
	}
	return tree;
}

static StringToValNode*
avlTree_rotate_2(StringToValNode* *path_top, s64 dir)
{
	StringToValNode* B, *C, *D, *E;
	s64 otherDir = 1^dir;
	B = *path_top;
	D = B->next[dir];
	C = D->next[otherDir];
	E = D->next[dir];

	*path_top = D;
	D->next[otherDir] = B;
	B->next[dir] = C;
	B->longer = NEITHER;
	D->longer = NEITHER;
	return E;
}

static StringToValNode*
avlTree_rotate_3(StringToValNode* *path_top, s64 dir, s64 third)
{
	StringToValNode* B, *F, *D, *C, *E;
	s64 otherDir = 1^dir;
	B = *path_top;
	F = B->next[dir];
	D = F->next[otherDir];
	/* note: C and E can be NULL */
	C = D->next[otherDir];
	E = D->next[dir];
	*path_top = D;
	D->next[otherDir] = B;
	D->next[dir] = F;
	B->next[dir] = C;
	F->next[otherDir] = E;
	D->longer = NEITHER;

	/* assume both trees are balanced */
	B->longer = F->longer = NEITHER;

	if (third == NEITHER){
		return 0;
	}
	else if (third == dir) {
		/* E holds the insertion so B is unbalanced */ 
		B->longer = otherDir;
		return E;
	} else {
		/* C holds the insertion so F is unbalanced */
		F->longer = dir;
		return C;
	}
}


/***************************************************
 * INSERTION                                       *
 ***************************************************/
static inline void
avlTree_rebalance_path(StringToValNode* path)
{
	/* Each StringToValNode* in path is currently balanced.
	 * Until we find target, mark each StringToValNode* as longer
	 * in the s32 of target because we know we have
	 * inserted target there
	 */
	if(path!=0){
		while ( path->taken > NEITHER ) {
			s8 next_step = (path->taken);
			path->longer = next_step;
			path = path->next[next_step];
		}
	}
}

static inline void
avlTree_rebalance_insert(StringToValNode **path_top)
{
	StringToValNode *path = *path_top;
	s64 first, second, third;
	if (Balanced(path)) {
		;
	}
	else if (path->longer != (first = (path->taken)) ) {
		/* took the shorter path */
		path->longer = NEITHER;
		path = path->next[first];
	} else if (first == (second = (path->next[first]->taken))) {
		/* just a two-point rotate */
		path = avlTree_rotate_2(path_top, first);
	} else {
		/* fine details of the 3 point rotate depend on the third step.
		 * However there may not be a third step, if the third point of the
		 * rotation is the newly inserted point.  In that case we record
		 * the third step as NEITHER
		 */
		path = path->next[first]->next[second];
		if (path->taken==NEITHER) {third = NEITHER;}
		else {third = (path->taken);}
		path = avlTree_rotate_3(path_top, first, third);
	}
	avlTree_rebalance_path(path);
}

static s32
avlTree_insert_internal(
	StringToValNode **treep,
	u8 *key,
	u32 keyLen,
	AvlValue value)
{
	StringToValNode *tree = *treep;
	StringToValNode **path_top = treep;
	s32 res;

	while ( tree && ((res=CMP(key, tree->key))!=0) ) {
		s8 next_step = tree->taken = (res>0);
		if (!Balanced(tree)) {
			path_top = treep;
		}
		treep = &tree->next[next_step];
		tree = *treep;
	}
	
	if (tree){ // tree already exists
		tree->value = value; // update value
		return avlTree_updatedValOfExistingKey;
	}
	
	tree = makeNode(key, keyLen, value);
	if (tree==0) {
		return avlTree_errorMallocFailed;
	}
	
	
	*treep = tree;
	avlTree_rebalance_insert(path_top);
	return avlTree_OK;
}

STATIC_BUILD
s32
avlTree_insert(
	StringToValNode **treep,
	u8 *key,
	u32 keyLen,
	AvlValue value)
{
	if(treep==0){
		return avlTree_errorNullParam1;
	}
	if(key==0){
		return avlTree_errorNullParam2;
	}
	return avlTree_insert_internal(treep, key, keyLen, value);
}

STATIC_BUILD
s32
avlTree_insertIntKey(
	StringToValNode **treep,
	s64 key,
	AvlValue value)
{
	u8  keyBuffer[16];
	u32 keyLen = avlTree_s64toString(key, keyBuffer);
	return avlTree_insert(treep, keyBuffer, keyLen, value);
}

/******************************************************
 * DELETION                                           *
 *****************************************************/

static inline void 
avlTree_swap_del(StringToValNode **targetp, StringToValNode **treep, s64 dir)
{
	StringToValNode *targetn = *targetp;
	StringToValNode *tree = *treep;

	*targetp = tree;
	*treep = tree->next[1^dir];
	tree->next[LEFT] = targetn->next[LEFT];
	tree->next[RIGHT]= targetn->next[RIGHT];
	tree->longer = targetn->longer;

	AVLTREE_FREE(targetn);
}

static inline StringToValNode**
avlTree_rebalance_del(StringToValNode **treep, StringToValNode **targetp)
{
	/* each StringToValNode* from treep down towards target, but
	 * excluding the last, will have a subtree grow
	 * and need rebalancing
	 */
	StringToValNode *targetn = *targetp;
	StringToValNode *tree;
	s64 dir, otherDir;

	while (1) {
		tree = *treep;
		dir = (tree->taken);
		otherDir = 1^dir;

		if (tree->next[dir]==NULL){
			break;
		}

		if (Balanced(tree)){
			tree->longer = otherDir;
		}
		else if (tree->longer == dir){
			tree->longer = NEITHER;
		}
		else {
			s32 second = tree->next[otherDir]->longer;
			if (second == dir) {
				avlTree_rotate_3(treep, otherDir, 
					     tree->next[otherDir]->next[dir]->longer);
			}
			else if (second == NEITHER) {
				avlTree_rotate_2(treep, otherDir);
				tree->longer = otherDir;
				(*treep)->longer = dir;
			} else {
				avlTree_rotate_2(treep, otherDir);
			}
			if (tree == targetn){
				targetp = &(*treep)->next[dir];
			}
		}

		treep = &tree->next[dir];
	}
	return targetp;
}


static s32
avlTree_delete_internal(StringToValNode **treep, u8 *key, AvlValue *value)
{
	/* delete the target from the tree, returning 1 on success or 0 if
	 * it wasn't found
	 */
	StringToValNode *tree;
	StringToValNode **path_top;
	StringToValNode **targetp;
	s64 dir, otherDir;
	s32 res;
	
	tree = *treep;
	path_top = treep;
	targetp = NULL;

	do {
		res = CMP(key, tree->key);
		dir = tree->taken = (res>0);
		otherDir = 1^dir;
		if (res==0){
			targetp = treep;
		}
		if (tree->next[dir] == NULL){
			break;
		}
		if (Balanced(tree)
		    || (tree->longer == (otherDir) && Balanced(tree->next[otherDir]))
			) {path_top = treep;}
		treep = &tree->next[dir];
		tree = *treep;
	} while (tree);
	
	if (!targetp){
		return avlTree_nothingFound;
	}
	
	if(value){
		*value = (*targetp)->value;
	}

	/* adjust balance, but don't lose 'targetp' */
	targetp = avlTree_rebalance_del(path_top, targetp);

	/* We have re-balanced everything, it remains only to 
	 * swap the end of the path (*treep) with the deleted item
	 * (*targetp)
	 */
	avlTree_swap_del(targetp, treep, dir);

	return avlTree_OK;
}

STATIC_BUILD
s32
avlTree_delete(StringToValNode **treep, u8 *key, AvlValue *value)
{	
	if(treep==0){
		return avlTree_errorNullParam1;
	}
	if(key==0){
		return avlTree_errorNullParam2;
	}
	return avlTree_delete_internal(treep, key, value);
}

STATIC_BUILD
s32
avlTree_deleteIntKey(
	StringToValNode **treep,
	s64 key,
	AvlValue *value)
{
	u8 keyBuffer[16];
	(void)avlTree_s64toString(key, keyBuffer);
	return avlTree_delete(treep, keyBuffer, value);
}

/*******************************************************************************
 * Helper Functions
*******************************************************************************/

static u32
avlTreeDepth_r(StringToValNode *tree, u32 depth)
{
	u32 leftDepth=0, rightDepth=0;
	if(tree->next[0]){
		leftDepth = avlTreeDepth_r(tree->next[0], depth);
	}
	if(tree->next[1]){
		rightDepth = avlTreeDepth_r(tree->next[1], depth);
	}
	
	if(leftDepth>rightDepth){
		rightDepth=leftDepth;
	}
	return rightDepth+1;
}

STATIC_BUILD
u32
avlTree_maxDepth(StringToValNode *tree)
{
	if(tree==0){
		return 0;
	}
	return avlTreeDepth_r(tree, 0);
}

static u32
avlTreeCount_r(StringToValNode *tree, u32 count)
{
	if(tree->next[0]){
		count = avlTreeCount_r(tree->next[0], count);
	}
	if(tree->next[1]){
		count = avlTreeCount_r(tree->next[1], count);
	}

	return count+1;
}

STATIC_BUILD
u32
avlTree_count(StringToValNode *tree)
{
	if(tree==0){
		return 0;
	}
	return avlTreeCount_r(tree, 0);
}

static void
avlTree_freeAll_r(StringToValNode **treep)
{
	StringToValNode *tree = *treep;
	if (tree==0) {
		return;
	}
	avlTree_freeAll_r(&tree->next[LEFT]);
	avlTree_freeAll_r(&tree->next[RIGHT]);
	AVLTREE_FREE(tree);
}

STATIC_BUILD
void
avlTree_freeAll(StringToValNode **treep)
{
	StringToValNode *tree;
	if (treep==0) {
		return;
	}
	tree = *treep;
	*treep = 0;
	avlTree_freeAll_r(&tree);
}

#define SIGN_MASK   0x8000000000000000
#define UNSIGN_MASK 0x7FFFFFFFFFFFFFFF

STATIC_BUILD
u32
avlTree_s64toString(s64 signedInput, u8 *output)
{
	u8  tempOutput[16];
	u8  *outputp = output;
	s64 count = 0;
	u64 tmp;
	u64 input = signedInput;
	u8  header;
	u8  isPositive = ((input&SIGN_MASK)==0);
	// null terminate
	tempOutput[count] = '\0';
	// take off sign bit
	input = input & UNSIGN_MASK;
	// range 1 - 64
	header = 65 - __builtin_clzl(input); 
	// set high bit on positive number's first byte (Big Endian)
	header = header | (isPositive<<7);
	
	do{
		tmp = input % 128;
		input = input / 128;
		count++;
		tempOutput[count] = (tmp+1);
	}while(input);
	
	count++;
	tempOutput[count] = header;
	
	do{
		*outputp = tempOutput[count];
		outputp++;
		count--;
	}while (count);
	
	*outputp = tempOutput[0];
	return (outputp - output);
}

STATIC_BUILD
s64 
avlTree_stringTos64(u8 *string)
{
	u64 val = 0;
	u64 isNegative = (((*string)&0x80)==0);
	
	// skip header
	++string;
	
	while ( (*string >= 1) && (*string <= 128) ) {
		val = (val * 128) + ((*string) - 1);
		++string;
	}
	
	// add back in sign bit
	val = val | (isNegative<<63);
	
	return (s64)val;
}
