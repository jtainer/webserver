// 
// Hash table for resolving endpoints
//
// 2024, Jonathan Tainer

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>

typedef struct {
	const char* key;
	const char* value;
} TableEntry;

typedef struct {
	uint32_t max_entries;
	uint32_t num_entries;
	TableEntry* entry;

} Table;

uint32_t HashString(const uint8_t* str);

Table CreateTable(const uint32_t size);

void DestroyTable(Table* table);

int IsTableValid(Table* table);

void InsertTableEntry(Table* table, const char* key, const char* value);

void PrintTableEntries(Table* table);

const char* GetTableValue(Table* table, const char* key);

#endif
