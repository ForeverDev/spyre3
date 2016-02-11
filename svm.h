#ifndef __SVM_H
#define __SVM_H

#define IP 0
#define SP 1
#define BP 2

#define	FLAG_CMP	0x01
		
#define SIZE_STACK	1024

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef signed char			s8;
typedef signed short		s16;
typedef signed int			s32;
typedef signed long long	s64;

typedef	float				f32;
typedef double				f64;

typedef struct spy_state {
	
	f64	mem[65536];
	f64	reg[12];
	u8	flags;

} spy_state;

spy_state*		spy_newstate();
void			spy_run(spy_state*, const u8*);
void			spy_debug(spy_state*);

#endif
