#include "util.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void die(const char* message) {
	if (errno) {
		perror(message);
	} else {
		fprintf(stderr, "%s\n", message);
	}
	exit(EXIT_FAILURE);
}
