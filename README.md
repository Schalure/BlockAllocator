# BlockAllocator
  This file describes block allocator memory.
 
 Maximum blocks, for use 0xFFFF - 1 (65534)
 U will have (BlockAllocator_BLOCK_COUNT * uint16_t) additional memory
 for library maintenance.
 U may use only one allocator for your tasks
 

 =====================================================================
													!!!	IMPORTANT	!!!
BLOCK ALLOCATOR DOES NOT TRACK BLOCK BOUNDARIES WHEN WRITING DATA BY POINTER
IF YOU EXCEED THE BLOCK SIZE DURING RECORDING, SOME OF THE DATA MAY BE LOST
 =====================================================================

 How to use:
 
 1. U need to define count of blocks and block size in BlockAllocator_cfg.h, for example:
 #define BlockAllocator_BLOCK_COUNT		8 
 #define BlockAllocator_BLOCK_SIZE		8 
 
 2. U need initialize memory pool in main() before to use allocator, for example:
 BlockAllocator_initialize();

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

