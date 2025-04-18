#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint32_t HashString(const char* str) {
	uint32_t hash = 987654321;
	uint8_t c;
	while ((c = *str++)) {
		hash = (hash * 17000069) + c;
	}
	return hash;
}

Table CreateTable(const uint32_t size) {
	Table table = { 0 };
	table.entry = calloc(size, sizeof(TableEntry));
	if (table.entry == NULL) {
		fprintf(stderr, "Failed to allocate hash table of size %u\n", size);
		return table;
	}
	table.max_entries = size;
	return table;
}

void DestroyTable(Table* table) {
	if (table->entry == NULL) {
		return;
	}
	free(table->entry);
	table->entry = NULL;
	table->max_entries = 0;
	table->num_entries = 0;
	return;
}

int IsTableValid(Table* table) {
	if (table == NULL)
		return 0;
	if (table->entry == NULL)
		return 0;
	if (table->max_entries == 0)
		return 0;
	if (table->num_entries > table->max_entries)
		return 0;
	return 1;
}

void InsertTableEntry(Table* table, const char* key, const char* value) {
	if (table->entry == NULL || table->max_entries == 0) {
		fprintf(stderr, "Attempted to insert item into unallocated hash table\n");
		return;
	}
	else if (table->num_entries >= table->max_entries) {
		fprintf(stderr, "Attempted to insert item into full hash table\n");
		return;
	}
	else if (key == NULL) {
		fprintf(stderr, "Attempted to insert NULL key into hash table\n");
		return;
	}
	uint32_t hash = HashString(key);
	uint32_t index = hash % table->max_entries;
	while (table->entry[index].key != NULL) {
		index += 1;
		index %= table->max_entries;
	}
	table->entry[index].key = key;
	table->entry[index].value = value;
	return;
}

void PrintTableEntries(Table* table) {
	if (table->entry == NULL || table->max_entries == 0) {
		fprintf(stderr, "Attempted to print unallocated hash table\n");
		return;
	}
	for (uint64_t i = 0; i < table->max_entries; i++) {
		if (table->entry[i].key != NULL) {
			const char* key = table->entry[i].key;
			const char* value = table->entry[i].value;
			uint32_t hash = HashString(key);
			printf("Hash = %08x, Key = %s, Value = %s\n", hash, key, value);
		}
	}
}

const char* GetTableValue(Table* table, const char* key) {
	if (!IsTableValid(table))
		return NULL;
	uint32_t hash = HashString(key);
	const char* value = NULL;
	for (uint32_t i = 0; i < table->max_entries; i++) {
		uint32_t index = (hash + i) % table->max_entries;
		if (table->entry[index].key == NULL) {
			break;
		}
		else if (!strcmp(table->entry[index].key, key)) {
			value = table->entry[index].value;
			break;
		}
	}
	return value;
}
