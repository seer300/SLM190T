#ifndef __DUMP_H__
#define __DUMP_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

void DumpRegister_from_Normal(void);
void DumpRegister_from_Fault(uint32_t *stack_sp);
void Dump_Handler_in_Fault(uint32_t *stack_sp);
// defined in others file
void Dump_Memory_to_File(void);
void dump_sync(void);


#ifdef __cplusplus
 }
#endif

#endif  /* end of __DUMP_H__ */
