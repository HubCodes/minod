#ifndef MINOD_PROCESSOR_H
#define MINOD_PROCESSOR_H

#include "minod.h"

typedef struct processor processor;

processor* processor_new(void);
void processor_handle(processor* self, struct cmd* cmd);

#endif // MINOD_PROCESSOR_H
