#pragma once

#define Align(n, m)       ((n) == 0 ? 0 : ((((n) - 1) / (m) + 1) * (m)))

void *Alloc(i32 size);
void Free(void *ptr);
