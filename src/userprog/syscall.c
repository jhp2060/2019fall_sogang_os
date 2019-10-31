#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"

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
	int syscall_num = *(int*)esp;
	esp += sizeof(int*);
	
	switch (syscall_num) {
		case SYS_HALT:
			sys_halt();
			break;
		case SYS_EXIT:
			ASSERT(is_valid_access(esp));
			int status = *(int*)esp;
			esp += sizeof(int*);

			sys_exit(status);
			break;
		case SYS_EXEC:
			break;
		case SYS_WAIT:
			break;
		case SYS_READ:
			break;
		case SYS_WRITE:
			ASSERT(is_valid_access(esp));
			
			int fd = *(int*)esp;
			esp += sizeof(int*);

			ASSERT(is_valid_access(f->esp));
			const void *buffer = *(void**)esp;
			esp += sizeof(void**);

			ASSERT(is_valid_access(esp));
			unsigned size = *(unsigned*)esp;
			esp += sizeof(unsigned*);
			
			ASSERT(is_valid_access(buffer));
			f->eax = sys_write(fd, buffer, size);
			break;
	}
	//thread_exit();
}

bool is_valid_access(void *user_addr){
	struct thread *t = thread_current(); 
	if (pagedir_get_page(t->pagedir, user_addr) && is_user_vaddr(user_addr) && !is_kernel_vaddr(user_addr))
		return 1;
	else return 0;
}

void sys_halt(void){
	shutdown_power_off();
}

void sys_exit(int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_exit();
}

int sys_write(int fd, const void *buffer, unsigned size){
	int written = 0;
	if (fd == 1){
		int i;
		intr_set_level(INTR_OFF);
		for (i=0;i<size;i++){
			input_putc(*(uint8_t*)(buffer+i));
			written++;
		}
		intr_set_level(INTR_ON);
	}
	return written;
}
