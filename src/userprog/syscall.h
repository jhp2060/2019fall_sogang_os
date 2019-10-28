#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "pagedir.h" 
#include "../threads/vaddr.h"

void syscall_init (void);

bool is_valid_access(void* user_addr);

#endif /* userprog/syscall.h */
