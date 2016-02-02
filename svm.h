#ifndef __SVM_H
#define __SVM_H

#define IP 0
#define SP 1
#define FP 2

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
	u32	reg[12];

} spy_state;

spy_state*	spy_newstate();
void		spy_run(spy_state*, u8*);

#endif
