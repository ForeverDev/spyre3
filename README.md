
Opcode modes

MODE ID	| OPERANDS
--------|-----
0	| 
1	| (reg)
2	| (reg), (f64)
3	| (reg), (reg)
4	| (reg), [(reg) + (u64)]
5	| [(reg) + (u64)], (f64)
6	| [(reg) + (u64)], (reg)
7	| (u64)
8	| [(reg) + (u64)], [(reg) + (u64)]
Opcodes

OPCODE	| NAME	| VALID MODES 
--------|-------|------------
00	| NULL	| 0
01	| EXIT	| 0
02	| RET	| 0
20	| MOV	| 2 3 4 5 6 8
21	| ADD	| 2 3 4 5 6 8
22	| SUB	| 2 3 4 5 6 8
23	| MUL	| 2 3 4 5 6 8
24	| DIV	| 2 3 4 5 6 8
25	| NEG	| 1 
26	| OR	| 2 3 4 5 6 8
27	| AND 	| 2 3 4 5 6 8
28	| XOR	| 2 3 4 5 6 8
29	| NOT	| 1 
2A	| SHL	| 2 3 4 5 6 8
2B	| SHR	| 2 3 4 5 6 8
2C	| LT	| 2 3 4 5 6 8
2D	| LE	| 2 3 4 5 6 8
2E	| GT	| 2 3 4 5 6 8
2F	| GE	| 2 3 4 5 6 8
30	| CMP	| 2 3 4 5 6 8
31	| LAND	| 2 3 4 5 6 8
32	| LOR	| 2 3 4 5 6 8
33	| LNOT	| 2 3 4 5 6 8
34	| LEA	| 4 8
40	| PUSH	| 1 7
41	| POP 	| 0 1
60	| CALL	| 7 
61	| CCALL	| 7
62	| JMP	| 7
63	| JIF	| 7
64	| JIT	| 7
f0	| DLOG	| 1

File <.spyb> headers

BYTES	| DESCRIBES
--------|----------
[0, 4)	| starting location of ROM section
[4, 8)	| starting location of code section

Registers

CODE	| NAME	| PURPOSE
--------|-------|--------
00	| RIP	| instruction pointer
01	| RSP	| stack pointer
02	| RBP	| base pointer
03	| RAX	| general purpose #1, holds function return value
04	| RBX	| general purpose #2
05	| RCX	| general purpose #3
06	| RDX	| general purpose #4
07	| REX	| general purpose #5, holds function argument #1
08	| RFX	| general purpose #6, holds function argument #2
09	| RGX	| general purpose #7, holds function argument #3
0A	| RHX	| general purpose #8, holds function argument #4
0B	| RIX	| general purpose #9, holds function argument #5
0C	| RJX	| general purpose #10, holds function argument #6

Memory map

SECTION	| START			| FINISH	
--------|-----------------------|-------
ROM	| 0			| SIZE\_ROM - 1
STACK	| SIZE\_ROM		| SIZE\_ROM + SIZE\_STACK - 1
HEAP	| SIZE\_ROM + SIZE\_STACK	| SIZE\_MEMORY - 1


Calling convention

```
Arguments should be passed to a function in the following
registers respectively:
	RFX
	RGX
	RHX
	RIX
	RJX

Registers that should be the same after a function call:
	RBP
	RSP
	RBX
	RCX
	RDX

Return value is returned in:
	RAX

CALLING C FUNCTIONS:
	The calling convention for C functions is the same except for
	one thing.  The number of arguments to the C function is expected
	on the top of the stack. The number of arguments is passed to the C
	function as an argument.  SpyreVM automatically takes care
	of the nargs that was pushed onto the stack, you don't need to
	explicitly take care of it from the C function 
```
	



