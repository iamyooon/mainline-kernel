/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_ASM_GENERIC_SIGINFO_H
#define _UAPI_ASM_GENERIC_SIGINFO_H

#include <linux/compiler.h>
#include <linux/types.h>

typedef union sigval {
	int sival_int;
	void __user *sival_ptr;
} sigval_t;

/*
 * This is the size (including padding) of the part of the
 * struct siginfo that is before the union.
 */
#ifndef __ARCH_SI_PREAMBLE_SIZE
#define __ARCH_SI_PREAMBLE_SIZE	(3 * sizeof(int))
#endif

#define SI_MAX_SIZE	128
#ifndef SI_PAD_SIZE
#define SI_PAD_SIZE	((SI_MAX_SIZE - __ARCH_SI_PREAMBLE_SIZE) / sizeof(int))
#endif

#ifndef __ARCH_SI_UID_T
#define __ARCH_SI_UID_T	__kernel_uid32_t
#endif

/*
 * The default "si_band" type is "long", as specified by POSIX.
 * However, some architectures want to override this to "int"
 * for historical compatibility reasons, so we allow that.
 */
#ifndef __ARCH_SI_BAND_T
#define __ARCH_SI_BAND_T long
#endif

#ifndef __ARCH_SI_CLOCK_T
#define __ARCH_SI_CLOCK_T __kernel_clock_t
#endif

#ifndef __ARCH_SI_ATTRIBUTES
#define __ARCH_SI_ATTRIBUTES
#endif

#ifndef HAVE_ARCH_SIGINFO_T

typedef struct siginfo {
	int si_signo;
	int si_errno;
	int si_code;

	union {
		int _pad[SI_PAD_SIZE];

		/* kill() */
		struct {
			__kernel_pid_t _pid;	/* sender's pid */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			__kernel_timer_t _tid;	/* timer id */
			int _overrun;		/* overrun count */
			char _pad[sizeof( __ARCH_SI_UID_T) - sizeof(int)];
			sigval_t _sigval;	/* same as below */
			int _sys_private;       /* not to be passed to user */
		} _timer;

		/* POSIX.1b signals */
		struct {
			__kernel_pid_t _pid;	/* sender's pid */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
			sigval_t _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			__kernel_pid_t _pid;	/* which child */
			__ARCH_SI_UID_T _uid;	/* sender's uid */
			int _status;		/* exit code */
			__ARCH_SI_CLOCK_T _utime;
			__ARCH_SI_CLOCK_T _stime;
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			void __user *_addr; /* faulting insn/memory ref. */
#ifdef __ARCH_SI_TRAPNO
			int _trapno;	/* TRAP # which caused the signal */
#endif
			short _addr_lsb; /* LSB of the reported address */
			union {
				/* used when si_code=SEGV_BNDERR */
				struct {
					void __user *_lower;
					void __user *_upper;
				} _addr_bnd;
				/* used when si_code=SEGV_PKUERR */
				__u32 _pkey;
			};
		} _sigfault;

		/* SIGPOLL */
		struct {
			__ARCH_SI_BAND_T _band;	/* POLL_IN, POLL_OUT, POLL_MSG */
			int _fd;
		} _sigpoll;

		/* SIGSYS */
		struct {
			void __user *_call_addr; /* calling user insn */
			int _syscall;	/* triggering system call number */
			unsigned int _arch;	/* AUDIT_ARCH_* of syscall */
		} _sigsys;
	} _sifields;
} __ARCH_SI_ATTRIBUTES siginfo_t;

/* If the arch shares siginfo, then it has SIGSYS. */
#define __ARCH_SIGSYS
#endif

/*
 * How these fields are to be accessed.
 */
#define si_pid		_sifields._kill._pid
#define si_uid		_sifields._kill._uid
#define si_tid		_sifields._timer._tid
#define si_overrun	_sifields._timer._overrun
#define si_sys_private  _sifields._timer._sys_private
#define si_status	_sifields._sigchld._status
#define si_utime	_sifields._sigchld._utime
#define si_stime	_sifields._sigchld._stime
#define si_value	_sifields._rt._sigval
#define si_int		_sifields._rt._sigval.sival_int
#define si_ptr		_sifields._rt._sigval.sival_ptr
#define si_addr		_sifields._sigfault._addr
#ifdef __ARCH_SI_TRAPNO
#define si_trapno	_sifields._sigfault._trapno
#endif
#define si_addr_lsb	_sifields._sigfault._addr_lsb
#define si_lower	_sifields._sigfault._addr_bnd._lower
#define si_upper	_sifields._sigfault._addr_bnd._upper
#define si_pkey		_sifields._sigfault._pkey
#define si_band		_sifields._sigpoll._band
#define si_fd		_sifields._sigpoll._fd
#ifdef __ARCH_SIGSYS
#define si_call_addr	_sifields._sigsys._call_addr
#define si_syscall	_sifields._sigsys._syscall
#define si_arch		_sifields._sigsys._arch
#endif

/*
 * si_code values
 * Digital reserves positive values for kernel-generated signals.
 */
#define SI_USER		0		/* sent by kill, sigsend, raise */
#define SI_KERNEL	0x80		/* sent by the kernel from somewhere */
#define SI_QUEUE	-1		/* sent by sigqueue */
#define SI_TIMER	-2		/* sent by timer expiration */
#define SI_MESGQ	-3		/* sent by real time mesq state change */
#define SI_ASYNCIO	-4		/* sent by AIO completion */
#define SI_SIGIO	-5		/* sent by queued SIGIO */
#define SI_TKILL	-6		/* sent by tkill system call */
#define SI_DETHREAD	-7		/* sent by execve() killing subsidiary threads */

// siginfo 구조체에 설정된 si_code가 0보다 크지 않다면 
// 유저영역에서 전달된 siginfo 구조체이다.
#define SI_FROMUSER(siptr)	((siptr)->si_code <= 0)
// 0보다 크다면 커널코드에서 온 siginfo 구조체다.
#define SI_FROMKERNEL(siptr)	((siptr)->si_code > 0)

/*
 * SIGILL si_codes
 */
#define ILL_ILLOPC	1	/* illegal opcode */
#define ILL_ILLOPN	2	/* illegal operand */
#define ILL_ILLADR	3	/* illegal addressing mode */
#define ILL_ILLTRP	4	/* illegal trap */
#define ILL_PRVOPC	5	/* privileged opcode */
#define ILL_PRVREG	6	/* privileged register */
#define ILL_COPROC	7	/* coprocessor error */
#define ILL_BADSTK	8	/* internal stack error */
#define NSIGILL		8

/*
 * SIGFPE si_codes
 */
#define FPE_INTDIV	1	/* integer divide by zero */
#define FPE_INTOVF	2	/* integer overflow */
#define FPE_FLTDIV	3	/* floating point divide by zero */
#define FPE_FLTOVF	4	/* floating point overflow */
#define FPE_FLTUND	5	/* floating point underflow */
#define FPE_FLTRES	6	/* floating point inexact result */
#define FPE_FLTINV	7	/* floating point invalid operation */
#define FPE_FLTSUB	8	/* subscript out of range */
#define NSIGFPE		8

/*
 * SIGSEGV si_codes
 */
#define SEGV_MAPERR	1	/* address not mapped to object */
#define SEGV_ACCERR	2	/* invalid permissions for mapped object */
#define SEGV_BNDERR	3	/* failed address bound checks */
#define SEGV_PKUERR	4	/* failed protection key checks */
#define NSIGSEGV	4

/*
 * SIGBUS si_codes
 */
#define BUS_ADRALN	1	/* invalid address alignment */
#define BUS_ADRERR	2	/* non-existent physical address */
#define BUS_OBJERR	3	/* object specific hardware error */
/* hardware memory error consumed on a machine check: action required */
#define BUS_MCEERR_AR	4
/* hardware memory error detected in process but not consumed: action optional*/
#define BUS_MCEERR_AO	5
#define NSIGBUS		5

/*
 * SIGTRAP si_codes
 */
#define TRAP_BRKPT	1	/* process breakpoint */
#define TRAP_TRACE	2	/* process trace trap */
#define TRAP_BRANCH     3	/* process taken branch trap */
#define TRAP_HWBKPT     4	/* hardware breakpoint/watchpoint */
#define NSIGTRAP	4

/*
 * SIGCHLD si_codes
 */
#define CLD_EXITED	1	/* child has exited */
#define CLD_KILLED	2	/* child was killed */
#define CLD_DUMPED	3	/* child terminated abnormally */
#define CLD_TRAPPED	4	/* traced child has trapped */
#define CLD_STOPPED	5	/* child has stopped */
#define CLD_CONTINUED	6	/* stopped child has continued */
#define NSIGCHLD	6

/*
 * SIGPOLL (or any other signal without signal specific si_codes) si_codes
 */
#define POLL_IN		1	/* data input available */
#define POLL_OUT	2	/* output buffers available */
#define POLL_MSG	3	/* input message available */
#define POLL_ERR	4	/* i/o error */
#define POLL_PRI	5	/* high priority input available */
#define POLL_HUP	6	/* device disconnected */
#define NSIGPOLL	6

/*
 * SIGSYS si_codes
 */
#define SYS_SECCOMP	1	/* seccomp triggered */
#define NSIGSYS		1

/*
 * sigevent definitions
 * 
 * It seems likely that SIGEV_THREAD will have to be handled from 
 * userspace, libpthread transmuting it to SIGEV_SIGNAL, which the
 * thread manager then catches and does the appropriate nonsense.
 * However, everything is written out here so as to not get lost.
 */
#define SIGEV_SIGNAL	0	/* notify via signal */
#define SIGEV_NONE	1	/* other notification: meaningless */
#define SIGEV_THREAD	2	/* deliver via thread creation */
#define SIGEV_THREAD_ID 4	/* deliver to thread */

/*
 * This works because the alignment is ok on all current architectures
 * but we leave open this being overridden in the future
 */
#ifndef __ARCH_SIGEV_PREAMBLE_SIZE
#define __ARCH_SIGEV_PREAMBLE_SIZE	(sizeof(int) * 2 + sizeof(sigval_t))
#endif

#define SIGEV_MAX_SIZE	64
#define SIGEV_PAD_SIZE	((SIGEV_MAX_SIZE - __ARCH_SIGEV_PREAMBLE_SIZE) \
		/ sizeof(int))

typedef struct sigevent {
	sigval_t sigev_value;
	int sigev_signo;
	int sigev_notify;
	union {
		int _pad[SIGEV_PAD_SIZE];
		 int _tid;

		struct {
			void (*_function)(sigval_t);
			void *_attribute;	/* really pthread_attr_t */
		} _sigev_thread;
	} _sigev_un;
} sigevent_t;

#define sigev_notify_function	_sigev_un._sigev_thread._function
#define sigev_notify_attributes	_sigev_un._sigev_thread._attribute
#define sigev_notify_thread_id	 _sigev_un._tid


#endif /* _UAPI_ASM_GENERIC_SIGINFO_H */
