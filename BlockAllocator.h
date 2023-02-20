#ifndef T_BLOCK_ALLOCATOR_H
#define T_BLOCK_ALLOCATOR_H

#include <cstdint>
#include "BlockAllocator_cfg.h"

/*
----------------------------------------------------------------------
 This file describes block allocator memory.
 
 Maximum blocks, for use 0xFFFF - 1 (65534)
 U will have (BlockAllocator_BLOCK_COUNT * uint16_t) additional memory
 for library maintenance.
 U may use only one allocator for your tasks
 

													!!!	IMPORTANT	!!!

BLOCK ALLOCATOR DOES NOT TRACK BLOCK BOUNDARIES WHEN WRITING DATA BY POINTER
IF YOU EXCEED THE BLOCK SIZE DURING RECORDING, SOME OF THE DATA MAY BE LOST

 How to use:
 
 1. U need to define count of blocks and block size in BlockAllocator_cfg.h, for example:
 #define BlockAllocator_BLOCK_COUNT		8 
 #define BlockAllocator_BLOCK_SIZE		8 
 
 2. U need initialize memory pool in main() before to use allocator, for example:
 BlockAllocator_initialize();

 Note: u can not use this function, if define macro AUTO_INITIALIZE like 1 in BlockAllocator_cfg.h.
 In this case BlockAllocator_initialize start on first block allocation. You must remember
 that the initialization time is O(n). By default, auto-initialization is disabled. 

 3. For allocate block:
 "your_type"* "var_name" = ("your_type"*)BlockAllocator_newBlock();
 
 4. For free block:
 bool res = BlockAllocator_deleteBlock("var_name");
 
 4. For releace all blocks
 BlockAllocator_releaseAll();

 5. If u use RTOS please define macros TBlockAllocator_ENTER_CRITICAL() and
TBlockAllocator_EXIT_CRITICAL() in BlockAllocator_cfg.h according to your RTOS.
Example defines for FreeRTOS:
#include "FreeRTOS.h"
#include "task.h"
#define TBlockAllocator_ENTER_CRITICAL()	taskENTER_CRITICAL()
#define TBlockAllocator_EXIT_CRITICAL()	taskEXIT_CRITICAL()

6. During debuging u may use debug information and error hooks 
(search struct TBlockAllocator_DebugInfo in this file). For this
define macro __BLOCK_ALLOCATOR_DEBUG__ in your solution or in BlockAllocator_cfg.h. For example:
#define __BLOCK_ALLOCATOR_DEBUG__

7. BlockAllocator has tests for functions (run from BlockAllocator_tests()):
	BlockAllocator_newBlock()			=>		BlockAllocator_newBlock_test()
	BlockAllocator_deleteBlock()	=>		BlockAllocator_deleteBlock_test()
----------------------------------------------------------------------
 */


//=====================================================================
//												Library defines
//=====================================================================
//	By default, auto-initialization is disabled. 
#ifndef AUTO_INITIALIZE
#define	AUTO_INITIALIZE	0
#endif // !AUTO_INITIALIZE


//	if u use RTOS define fhis in BlocAllocator_cfg.h
#ifndef BlockAllocator_ENTER_CRITICAL
#define	BlockAllocator_ENTER_CRITICAL()
#endif // !TBlockAllocator_ENTER_CRITICAL


//	if u use RTOS define fhis in BlocAllocator_cfg.h
#ifndef BlockAllocator_EXIT_CRITICAL
#define	BlockAllocator_EXIT_CRITICAL()
#endif // !TBlockAllocator_EXIT_CRITICAL


//---------------------------------------------------------------------
//	Debug information struct and hooks
#ifdef __BLOCK_ALLOCATOR_DEBUG__
struct TBlockAllocator_DebugInfo
{
	//	Increment if a request new block
	uint32_t countOfNew;
	//	Increment if a request to delete block
	uint32_t countOfDelete;
	//	The size of the free space in the heap
	uint16_t countFreeBlocks;
	//	Increment if a request new block when all bkocks is busy
	uint32_t countOfNewErrors;
	//	Increment if a request to delete block when this block was deleted earlier
	uint32_t countOfDeleteErrors;
	//	Increment if a request to delete invalid pointer
	uint32_t countOfDeleteInvalidPointer;
};


//	macro  callback for BlockAllocator_DebugInfo.countOfNewErrors
//	call if new block when all bkocks is busy
#ifndef RequestNewBlock_ERROR_HOOK
#define RequestNewBlock_ERROR_HOOK()
#endif // RequestNewBlock_ERROR_HOOK


//	macro  callback for BlockAllocator_DebugInfo.countOfDeleteErrors
//	call if delete block when this block was deleted earlier
#ifndef RequestDeleteBlock_ERROR_HOOK
#define RequestDeleteBlock_ERROR_HOOK()
#endif // RequestDeleteBlock_ERROR_HOOK


//	macro  callback for BlockAllocator_DebugInfo.countOfDeleteInvalidPointer
//	call if a request to delete invalid pointer
#ifndef RequestDeleteInvalidBlockPointer_ERROR_HOOK
#define RequestDeleteInvalidBlockPointer_ERROR_HOOK()
#endif // RequestNewBlock_ERROR_HOOK

#endif // __BLOCK_ALLOCATOR_DEBUG__


//=====================================================================
//												Block Allocator API
//=====================================================================

//	initialize memory pool and debug information if u use __BLOCK_ALLOCATOR_DEBUG__ macro
void BlockAllocator_initialize();

//---------------------------------------------------------------------
//	get new block from memory pool
//	retun value:
//								pointer to new reserved block
//								NULL if all blocks is busy
//	u can use RequestNewBlock_ERROR_HOOK for more information and search errors	
const void* BlockAllocator_newBlock();

//---------------------------------------------------------------------
//	free block
//	parametr:
//								pointer to block
//	retun value:
//								TRUE if block was free sucsaseful
//								FALSE if block was not free sucsaseful
//	u can use RequestDeleteBlock_ERROR_HOOK and RequestDeleteUnvalidBlockPointer_ERROR_HOOK
//	for more information and search errors
bool BlockAllocator_deleteBlock(const void* pointer);

//---------------------------------------------------------------------
//	release all blocks
void BlockAllocator_releaseAll();

//---------------------------------------------------------------------
//	BlockAllocator tests
//	tests list:
//	1. test to allocate all memory pool
//	2. test to allocate block, when all memory pool is busy
//	3. test to free pointer not from memory pool
//	4. test to re-deleting a block
//	5. test to correct free all blocks
void BlockAllocator_tests();

#endif // !T_BLOCK_ALLOCATOR_H


