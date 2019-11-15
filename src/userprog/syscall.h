#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "pagedir.h" 
#include "../threads/vaddr.h"

#include "userprog/process.h"
#define pid_t int

#include "../filesys/filesys.h"
#include "../filesys/file.h"
#include "../filesys/inode.h"

void syscall_init (void);

/* project 1 */
bool is_valid_access(void *user_addr);

void halt (void);
void exit (int status);
int write (int fd, const void *buffer, unsigned size);
int fibonacci (int n);

pid_t exec (char *cmd_line);
int wait(pid_t pid);
int read (int fd, void *buffer, unsigned size);
int sum_of_four_int(int a, int b, int c, int d);

/* project 2*/
bool create(const char *file, unsigned initial_size);
bool remove (const char *file);
int  open (const char *file);
int filesize (int fd);
//int read (int fd, void *buffer, unsigned size);
//int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

bool is_valid_fd (int fd);

#endif /* userprog/syscall.h */
