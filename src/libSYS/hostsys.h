
#ifndef __HOSTSYS_H__
#define __HOSTSYS_H__

/* Semaphores. */
void *sys_sema_new(void);
void sys_sema_del(void *);
int sys_sema_init(void *);
int sys_sema_destroy(void *);
int sys_sema_post(void *);
int sys_sema_wait(void *);
int sys_sema_trywait(void *);

/* Dlls. */
void *sys_dll_open(const char *path);
int sys_dll_close(void *);
void *sys_dll_sym(void *, const char *name);
char *sys_dll_error(void);

/* Wait. */
void sys_wait(int usec);
void sys_sleep(int sec);

/* Wait input. */
int sys_select_fd(int fdes);

/* Setting alarm. */
int sys_set_alarm(int timeout, void *handler);
#endif
