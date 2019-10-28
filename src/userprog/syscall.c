#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}

bool is_valid_access(void* user_addr){
	struct thread *t = thread_current(); 
	if (pagedir_get_page(t->pagedir, user_addr) && is_user_vaddr(user_addr) && !is_kernel_vaddr(user_addr))
		return 1;
	else return 0;
}
