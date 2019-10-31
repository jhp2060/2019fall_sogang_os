#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "pagedir.h" 
#include "../threads/vaddr.h"

void syscall_init (void);

bool is_valid_access(void *user_addr);

void sys_halt (void);
void sys_exit (int status);
int sys_write (int fd, const void *buffer, unsigned size);

#endif /* userprog/syscall.h */
