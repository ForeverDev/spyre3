#include <stdio.h>
#include <stdlib.h>
#include "svm.h"

int main() {

	spy_state* S = spy_newstate();
	
	const u8 code[] = {
		// mov r3, 5
		0x01, 0x00, 0x03, 0x05, 0x00, 0x00, 0x00,
		// mov r4, r3
		0x01, 0x01, 0x04, 0x03,
		
		0x00  
	};

	spy_run(S, code);
	spy_debug(S);

	return 0;

}
