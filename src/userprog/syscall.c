#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "devices/intq.h"
#include "lib/string.h"
#include "threads/synch.h"

//#include "lib/user/syscall.h"

struct semaphore mutex;

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	sema_init(&mutex, 1);
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
			//printf("status : %d\n", status);
			esp += sizeof(int*);
			exit(status);
			break;
		case SYS_EXEC:
			// char* cmd_line
			ASSERT(is_valid_access(esp));
			char* cmd_line = *(char**)esp;
			esp += sizeof(char*);
			
			ASSERT(is_valid_access(cmd_line));
			f->eax = exec(cmd_line);
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
		case SYS_SUM_OF_FOUR_INT:
			ASSERT(is_valid_access(esp));
			int a = *(int*)esp;
			esp += sizeof(int*);

			ASSERT(is_valid_access(esp));
			int b = *(int*)esp;
			esp += sizeof(int*);
			
			ASSERT(is_valid_access(esp));
			int c = *(int*)esp;
			esp += sizeof(int*);

			ASSERT(is_valid_access(esp));
			int d = *(int*)esp;
			esp += sizeof(int*);

			f->eax = sum_of_four_int(a,b,c,d);
			break;
		/* project 2 */
		case SYS_CREATE:
			// const char *file, unsigned initial_size
			is_valid_access(esp);
			char* create_file = *(char**)esp;
			esp += sizeof(char*);

			is_valid_access(esp);
			unsigned initial_size = *(int*)esp;

			is_valid_access(create_file);
			f->eax = create((const char*)create_file, initial_size);
			break;
		case SYS_REMOVE:
			// const char *file
			is_valid_access(esp);
			char* remove_file = *(char**)esp;

			is_valid_access(remove_file);
			f->eax = remove((const char*)remove_file);
			break;
		case SYS_OPEN:
			// const char* file
			is_valid_access(esp);
			char* open_file = *(char**)esp;
			
			is_valid_access(open_file);
			f->eax = open((const char*)open_file);
			break;
		case SYS_FILESIZE:
			is_valid_access(esp);
			int filesize_fd = *(int*)esp;
			esp += sizeof(int*);

			f->eax = filesize(filesize_fd);
			break;
		case SYS_SEEK:
			// int fd, unsigned position
			is_valid_access(esp);
			int seek_fd = *(int*)esp;
			esp += sizeof(int*);

			is_valid_access(esp);
			unsigned position = *(unsigned*)esp;
			seek(seek_fd, position);
			break;
		case SYS_TELL:
			// int fd
			is_valid_access(esp);
			int tell_fd = *(int*)esp;
			
			f->eax = tell(tell_fd);
			break;
		case SYS_CLOSE:
			is_valid_access(esp);
			int close_fd = *(int*)esp;
			close(close_fd);
			break;
		default:
			break;
	}
	//thread_exit();
}

bool is_valid_access(void *user_addr){
	struct thread *t = thread_current();
	if (is_user_vaddr(user_addr) && !is_kernel_vaddr(user_addr)
			&& pagedir_get_page(t->pagedir, user_addr))
		return true;
	else exit(-1);
}

void halt(void){
	shutdown_power_off();
}

void exit(int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	//printf("current exit_status : %d\n", thread_current()->exit_status);
	
	int i;
	for (i=2; i<MAX_OPEN_FILES; i++)
		if (is_valid_fd(i))
			close(i);

  thread_exit();
}

int write(int fd, const void *buffer, unsigned size){
	int written = 0;
	if (fd == 1){
		putbuf(buffer, size);
		written = size;
	}
	else if (fd == 0);
	else {
		if (!is_valid_fd(fd))	exit(-1);
		sema_down(&mutex);
		written = file_write(thread_current()->fd[fd], buffer, size);
		sema_up(&mutex);
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
	int read = -1;
	if (fd == STDIN_FILENO){
		unsigned i;
		for (i = 0; i < size; i++) {
			((char*)buffer)[i] = input_getc();
		}
		read = size;
	}
	else if (fd == 1);
	else {
		if (!is_valid_fd(fd))	exit(-1);
		sema_down(&mutex);
		read = file_read(thread_current()->fd[fd], buffer, size);
		sema_up(&mutex);
	}

	return read;
}

int sum_of_four_int(int a, int b, int c, int d){
	return (a+b+c+d);
}


/* project 2 */
bool create(const char *file, unsigned initial_size){
	return filesys_create(file, (off_t)initial_size);
}

bool remove (const char *file){
	return filesys_remove(file);
}

int open (const char *file){
	struct file *f = filesys_open(file);

	// the file could not be opened
	if (f == NULL) return -1;
	
	// STDIN = 0, STDOUT = 1
	int i = 2;
	for (; i < MAX_OPEN_FILES; i++) {
		if (thread_current()->fd[i] != NULL) continue;
		if (strcmp(thread_current()->name, file) == 0)
			file_deny_write(f);
		thread_current()->fd[i] = f;
		return i;
	}
	
	// overflow of maximum number of files the current thread can open
	return -1;
}

int filesize (int fd){
	if (!is_valid_fd(fd)) exit(-1);
	return file_length(thread_current()->fd[fd]);	
}

void seek (int fd, unsigned position){
	if (!is_valid_fd(fd)) exit(-1);
	file_seek(thread_current()->fd[fd], (off_t)position);	
}

unsigned tell (int fd){
	if (!is_valid_fd(fd)) exit(-1);
	return (unsigned) file_tell(thread_current()->fd[fd]);	
}

void close (int fd){
	if(!is_valid_fd(fd)) exit(-1);
	file_close(thread_current()->fd[fd]);
	thread_current()->fd[fd] = NULL;
}

bool is_valid_fd (int fd) {
	if (fd < 0 || fd >= MAX_OPEN_FILES)
		return false;
	if (thread_current()->fd[fd] == NULL)
		return false;
	return true;
}
