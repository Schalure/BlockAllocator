//	Block Allocator configuration file
#ifndef T_BLOCK_ALLOCATOR_CFH_H
#define T_BLOCK_ALLOCATOR_CFG_H


//---------------------------------------------------------------------
//	You must uncomment next defines and change:
//  "BC" to the number of blocks you want
//	"BS" to the block size you want
//#define	BLOCK_COUNT	(BC)
//#define	BLOCK_SIZE	(BS)


//---------------------------------------------------------------------
//	uncomment if u want to use debug information
	#define __BLOCK_ALLOCATOR_DEBUG__


//---------------------------------------------------------------------
//	uncomment if u want to use error hooks and implement hook functions:
//	void NewBlock_ERROR_HOOK()
//	void DeleteBlock_ERROR_HOOK()
//	DeleteInvalidBlockPointer_ERROR_HOOK()
#ifdef __BLOCK_ALLOCATOR_DEBUG__
//#define RequestNewBlock_ERROR_HOOK()	NewBlock_ERROR_HOOK()
//#define RequestDeleteBlock_ERROR_HOOK()	DeleteBlock_ERROR_HOOK()
//#define RequestDeleteInvalidBlockPointer_ERROR_HOOK()	DeleteInvalidBlockPointer_ERROR_HOOK()
#endif // __BLOCK_ALLOCATOR_DEBUG__


//---------------------------------------------------------------------
//	uncomment if u use FreeRTOS
//#include "FreeRTOS.h"
//#include "task.h"
//#define TBlockAllocator_ENTER_CRITICAL()	taskENTER_CRITICAL()
//#define TBlockAllocator_EXIT_CRITICAL()	taskEXIT_CRITICAL()




#endif // !T_BLOCK_ALLOCATOR_CFG_H
