#ifndef __INFO_H
#define __INFO_H

#define SIZE_ROM			0x00800		
#define SIZE_STACK			0x00400
#define SIZE_MEMORY			0x10000

#define START_ROM			0x00
#define START_STACK			(START_ROM + SIZE_ROM + SIZE_STACK)
#define START_MEMORY		(START_STACK + 1)

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

#endif
