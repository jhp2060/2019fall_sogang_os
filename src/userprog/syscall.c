#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "devices/intq.h"

//#include "lib/user/syscall.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *esp = f->esp;
	ASSERT(is_valid_access(esp));
	int syscall_num = *(int*)esp;
	esp += sizeof(int*);
	
	switch (syscall_num) {
		case SYS_HALT:
			halt();
			break;
		case SYS_EXIT:
			ASSERT(is_valid_access(esp));
			int status = *(int*)esp;
			esp += sizeof(int*);
			exit(status);
			break;
		case SYS_EXEC:
			// char* cmd_line
			ASSERT(is_valid_access(esp));
			char* cmd_line = *(char**)esp;
			esp += sizeof(char*);
			
			ASSERT(is_valid_access(cmd_line));
			f->eax = (uint32_t)exec(cmd_line);
			break;
		case SYS_WAIT:
			// pid_t pid
			ASSERT(is_valid_access(esp));
			pid_t pid = *(pid_t*)esp;
			esp += sizeof(pid_t*);
			f->eax = wait(pid);
			break;
		case SYS_READ:
			// int fd, void *buffer, unsigned size
			ASSERT(is_valid_access(esp));
			int r_fd = *(int*)esp;
			esp += sizeof(int*);

			ASSERT(is_valid_access(esp));
			void* r_buffer = *(void**)esp;
			esp += sizeof(void**);
			
			ASSERT(is_valid_access(esp));
			unsigned r_size = *(unsigned*)esp;
			esp += sizeof(unsigned*);

			ASSERT(is_valid_access(r_buffer));			
			f->eax = read(r_fd, r_buffer, r_size);			
			break;
		case SYS_WRITE:
			ASSERT(is_valid_access(esp));
			int fd = *(int*)esp;
			esp += sizeof(int*);

			ASSERT(is_valid_access(esp));
			void *buffer = *(void**)esp;
			esp += sizeof(void**);

			ASSERT(is_valid_access(esp));
			unsigned size = *(unsigned*)esp;
			esp += sizeof(unsigned*);
			
			ASSERT(is_valid_access(buffer));
			f->eax = write(fd, (const void*) buffer, size);
			break;
		case SYS_FIBONACCI:
			ASSERT(is_valid_access(esp));
			int n = *(int*)esp;
			esp += sizeof(int*);

			f->eax = fibonacci(n);
			break;
	}
	//thread_exit();
}

bool is_valid_access(void *user_addr){
	struct thread *t = thread_current(); 
	return (pagedir_get_page(t->pagedir, user_addr) &&
		   	is_user_vaddr(user_addr) && !is_kernel_vaddr(user_addr));
}

void halt(void){
	shutdown_power_off();
}

void exit(int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_exit();
}

int write(int fd, const void *buffer, unsigned size){
	int written = 0;
	if (fd == 1){
		putbuf(buffer, size);
		written = size;
	}
	return written;
}

int fibonacci(int n){
	if (n < 2)
		return n;
	int i, a=0, b=1;
	for (i=1; i<n; i++){
		b = b+a;
		a = b-a;
	}
	return b;
}

/* ----------------------------  */

pid_t exec(char* cmd_line){
	return (pid_t)(process_execute(cmd_line));
}

int wait(pid_t pid){
	return process_wait(pid);
}


int read (int fd, void *buffer, unsigned size){
	if (fd == STDIN_FILENO){
		unsigned len;
		for (len = 0; len <size; len++) {
			if(((char*)buffer)[len] != '\0') continue;
			break;
		}
		return len;
	}
	return -1;
}

