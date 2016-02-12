#ifndef __SVM_H
#define __SVM_H

#define IP 0
#define SP 1
#define BP 2

#define	FLAG_CMP	0x01

#define SIZE_ROM	0x00800		
#define SIZE_STACK	0x00400
#define SIZE_MEMORY	0x10000

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;

typedef	float				f32;
typedef double				f64;

struct spy_state;
struct spy_cfunc;

typedef struct spy_cfunc {
	
	s8 identifier[128];
	void (*fptr)(struct spy_state*, u8);

} spy_cfunc;

typedef struct spy_state {
	
	f64			mem[SIZE_MEMORY];
	f64			reg[12];
	u8			flags;
	u8			nfuncs;
	spy_cfunc 	cfuncs[128];

} spy_state;

spy_state*		spy_newstate();
void			spy_readAndRun(spy_state*, const s8*);
static void		spy_run(spy_state*, const u8*);
static void		spy_debug(spy_state*);

#endif
