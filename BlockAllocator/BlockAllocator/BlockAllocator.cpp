#include "BlockAllocator.h"


//---------------------------------------------------------------------
//	Defines
#define	BLOCK_IS_BUSY	0xFFFF



//---------------------------------------------------------------------
//	Vars
#if !(defined(BlockAllocator_BLOCK_SIZE) && defined(BlockAllocator_BLOCK_COUNT))
#error you must define BlockAllocator_BLOCK_SIZE and BlockAllocator_BLOCK_COUNT in BlockAllocator_cfg.h
#else
//	Tabel whith next free block in block chain
static uint16_t nextBlockIndexTabel_[BlockAllocator_BLOCK_COUNT];
//	Memory pool
static uint8_t memoryPool_[BlockAllocator_BLOCK_COUNT][BlockAllocator_BLOCK_SIZE];
//	Index for new blok (initialize in void BlockAllocator_initialize())
static uint16_t currentFreeIndex_;
//	Last free index (initialize in void BlockAllocator_initialize())
static uint16_t lastFreeIndex_;
//	
static bool isInitialized = false;
#endif

//	block size must bo more then uint64_t pointer
#define	MINIMUM_BLOCK_SIZE	(8)
#if	(BlockAllocator_BLOCK_SIZE < MINIMUM_BLOCK_SIZE)
#error BlockAllocator_BLOCK_SIZE must be more than 8
#endif


#ifdef __BLOCK_ALLOCATOR_DEBUG__
TBlockAllocator_DebugInfo BlockAllocator_DebugInfo;
#endif // __DEBAG__






//---------------------------------------------------------------------
void BlockAllocator_initialize()
{
	if (isInitialized)
		return;

	//	fill tabel info about blocks
	uint16_t i = BlockAllocator_BLOCK_COUNT;
	while (i)
	{
		nextBlockIndexTabel_[i - 1] = i;
		--i;
	}
	currentFreeIndex_ = 0;
	lastFreeIndex_ = BlockAllocator_BLOCK_COUNT - 1;
	isInitialized = true;

#ifdef __BLOCK_ALLOCATOR_DEBUG__
	//	prepare debug info
	BlockAllocator_DebugInfo.countFreeBlocks = BlockAllocator_BLOCK_COUNT;
	BlockAllocator_DebugInfo.countOfDelete = 0;
	BlockAllocator_DebugInfo.countOfNew = 0;
	BlockAllocator_DebugInfo.countOfDeleteErrors = 0;
	BlockAllocator_DebugInfo.countOfNewErrors = 0;
	BlockAllocator_DebugInfo.countOfDeleteInvalidPointer = 0;
#endif // __BLOCK_ALLOCATOR_DEBUG__

}

//---------------------------------------------------------------------
const void* BlockAllocator_newBlock()
{
	void* ptr = NULL;
	uint16_t tempCurrentIndex;

	BlockAllocator_ENTER_CRITICAL();

	//	is memoryArray has free blocks
	if (nextBlockIndexTabel_[lastFreeIndex_] != BLOCK_IS_BUSY)
	{
		//	if free more then one block		=>	index = currentFreeIndex_
		//	if free only one block				=>	index = lastFreeIndex_
		uint16_t* index = (nextBlockIndexTabel_[currentFreeIndex_] != BlockAllocator_BLOCK_COUNT)
			? &currentFreeIndex_ : &lastFreeIndex_;

		//	get block pointer
		ptr = &memoryPool_[*index];
		//	save current index to temp
		tempCurrentIndex = *index;
		//	update currentFreeIndex_ to next free index
		currentFreeIndex_ = nextBlockIndexTabel_[*index];
		//	set flag busy in Index Tabel
		nextBlockIndexTabel_[tempCurrentIndex] = BLOCK_IS_BUSY;
#ifndef __BLOCK_ALLOCATOR_DEBUG__
	}
#else
		++BlockAllocator_DebugInfo.countOfNew;
		--BlockAllocator_DebugInfo.countFreeBlocks;
}
	else
	{
		++BlockAllocator_DebugInfo.countOfNewErrors;
		RequestNewBlock_ERROR_HOOK();
	}
#endif // !__BLOCK_ALLOCATOR_DEBUG__

	BlockAllocator_EXIT_CRITICAL();

	return ptr;
}

//---------------------------------------------------------------------
bool BlockAllocator_deleteBlock(const void* pointer)
{
	//	check valid pointer
	if (((uint64_t)pointer < (uint64_t)memoryPool_) ||
		(((uint64_t)pointer) > (uint64_t)&memoryPool_[BlockAllocator_BLOCK_SIZE - 1]))
	{
#ifdef __BLOCK_ALLOCATOR_DEBUG__
		RequestDeleteInvalidBlockPointer_ERROR_HOOK();
		++BlockAllocator_DebugInfo.countOfDeleteInvalidPointer;
#endif
		return false;
	}

	BlockAllocator_ENTER_CRITICAL();

	//	get array index form pointer
	uint16_t index = (uint16_t)(((uint64_t)pointer - (uint64_t)memoryPool_) / BlockAllocator_BLOCK_SIZE);

	//	check block: free or busy
	if (nextBlockIndexTabel_[index] != BLOCK_IS_BUSY)
	{
#ifdef __BLOCK_ALLOCATOR_DEBUG__
		RequestDeleteBlock_ERROR_HOOK();
		++BlockAllocator_DebugInfo.countOfDeleteErrors;
#endif
		return false;
	}

	//	if free zero blocks									=>	currentFreeIndex_ = index
	//	if free more then zero blocks				=>	nextBlockIndexTabel_[lastFreeIndex_] = index
	if (currentFreeIndex_ == BlockAllocator_BLOCK_COUNT)
		currentFreeIndex_ = index;
	else
		nextBlockIndexTabel_[lastFreeIndex_] = index;
	//	update last free index
	lastFreeIndex_ = index;
	//	set flag, this block is last in block chain
	nextBlockIndexTabel_[index] = BlockAllocator_BLOCK_COUNT;

#ifdef __BLOCK_ALLOCATOR_DEBUG__
	++BlockAllocator_DebugInfo.countFreeBlocks;
	++BlockAllocator_DebugInfo.countOfDelete;
#endif // __BLOCK_ALLOCATOR_DEBUG__

	BlockAllocator_EXIT_CRITICAL();

	return true;
}

//---------------------------------------------------------------------
void BlockAllocator_releaseAll()
{
	BlockAllocator_ENTER_CRITICAL();

	isInitialized = false;
	BlockAllocator_initialize();

	BlockAllocator_EXIT_CRITICAL();
}

//=====================================================================
//														Tests
//=====================================================================
// if u need, u can redefine this macro if necessary.
#ifndef BlockAllocator_ASSERT
#include <assert.h>
#define BlockAllocator_ASSERT(x) assert(x)
#endif

//	test BlockAllocator_newBlock
static void BlockAllocator_newBlock_test();
//	test BlockAllocator_deleteBlock
static void BlockAllocator_deleteBlock_test();

//---------------------------------------------------------------------
//	BlockAllocator all tests
void BlockAllocator_tests()
{
	BlockAllocator_newBlock_test();
	BlockAllocator_deleteBlock_test();
}

//---------------------------------------------------------------------
//	test BlockAllocator_newBlock
static void BlockAllocator_newBlock_test()
{
	uint8_t* block = NULL;

	BlockAllocator_releaseAll();

	//	1.	if any block will be NULL, it will be error
	for (uint16_t i = 0; i < BlockAllocator_BLOCK_COUNT; ++i)
	{
		block = (uint8_t*)BlockAllocator_newBlock();
		BlockAllocator_ASSERT((block != NULL) && "block can't allocate, but memory pool not busy!");
	}

	//	2. Now all blocks are busy, try to allocate another block
	block = (uint8_t*)BlockAllocator_newBlock();
	BlockAllocator_ASSERT((block == NULL) && "block allocate, but memory pool is busy!");
}

//---------------------------------------------------------------------
//	test BlockAllocator_deleteBlock
static void BlockAllocator_deleteBlock_test()
{
	BlockAllocator_releaseAll();

	//	fill all memory pool
	void* blockList[BlockAllocator_BLOCK_COUNT];
	for (uint16_t i = 0; i < BlockAllocator_BLOCK_COUNT; ++i)
	{
		blockList[i] = (uint8_t*)BlockAllocator_newBlock();
	}

	//	1. delete random pointer
	uint8_t* pointer = (uint8_t*)((uint64_t)memoryPool_[0] - sizeof(void*));
	BlockAllocator_ASSERT(!BlockAllocator_deleteBlock(pointer) && "allocator can free pointer not from memory pool");

	pointer = (uint8_t*)((uint64_t)memoryPool_[BlockAllocator_BLOCK_COUNT - 1] + sizeof(void*));
	BlockAllocator_ASSERT(!BlockAllocator_deleteBlock(pointer) && "allocator can free pointer not from memory pool");

	//	2. re-deleting a block
	BlockAllocator_deleteBlock(blockList[0]);
	BlockAllocator_ASSERT(!BlockAllocator_deleteBlock(blockList[0]) && "allocator can re-deleting block");

	//	3. free all blocks
	blockList[0] = (uint8_t*)BlockAllocator_newBlock();
	for (uint16_t i = 0; i < BlockAllocator_BLOCK_COUNT; ++i)
	{
		BlockAllocator_ASSERT(BlockAllocator_deleteBlock(blockList[i]) && "allocator can't delet block");
	}
}


