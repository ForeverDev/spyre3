#ifndef __SLIB_H
#define __SLIB_H

#include "spyre.h"

void 		spyL_loadlibs(spy_state*);

static void spyL_io_printf(spy_state*, u8);
static void	spyL_io_getchar(spy_state*, u8);
static void spyL_io_getln(spy_state*, u8);
static void spyL_io_getstr(spy_state*, u8);
static void spyL_io_fopen(spy_state*, u8);
static void spyL_io_fclose(spy_state*, u8);
static void spyL_io_fprintf(spy_state*, u8);
static void spyL_io_fputstr(spy_state*, u8);
static void spyL_io_fputchar(spy_state*, u8);
static void spyL_io_fputnum(spy_state*, u8);

static void spyL_str_strlen(spy_state*, u8);
static void spyL_str_strcmp(spy_state*, u8);
static void spyL_str_strncmp(spy_state*, u8);
static void spyL_str_strcpy(spy_state*, u8);
static void spyL_str_strcat(spy_state*, u8);
static void spyL_str_sprintf(spy_state*, u8);
static void spyL_str_strtor(spy_state*, u8);

static void spyL_mem_malloc(spy_state*, u8);
static void spyL_mem_free(spy_state*, u8);
static void spyL_mem_memcpy(spy_state*, u8);
static void spyL_mem_memset(spy_state*, u8);

#endif
