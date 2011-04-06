#ifndef	_SYS_UTSNAME_H
#define	_SYS_UTSNAME_H

#define SYS_NMLN 32

struct utsname
{
  char sysname[SYS_NMLN];
  char nodename[SYS_NMLN];
  char release[SYS_NMLN];
  char version[SYS_NMLN];
  char machine[SYS_NMLN];
};

int	uname(struct utsname *);

#endif	/* !_SYS_UTSNAME_H */
