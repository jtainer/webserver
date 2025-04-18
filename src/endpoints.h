#ifndef ENDPOINTS_H
#define ENDPOINTS_H

#include "hash_table.h"

Table LoadEndpoints();

void UnloadEndpoints(Table* table);

const char* ResolveEndpoint(Table* table, const char* str);

#endif