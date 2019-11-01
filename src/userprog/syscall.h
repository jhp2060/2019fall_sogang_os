#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "pagedir.h" 
#include "../threads/vaddr.h"

#include "userprog/process.h"
#define pid_t int

void syscall_init (void);

bool is_valid_access(void *user_addr);

void halt (void);
void exit (int status);
int write (int fd, const void *buffer, unsigned size);
int fibonacci (int n);

/* -----------  */
pid_t exec (char *cmd_line);
int wait(pid_t pid);
int read (int fd, void *buffer, unsigned size);

#endif /* userprog/syscall.h */
