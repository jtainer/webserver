#include "endpoints.h"

Table LoadEndpoints() {
	const unsigned int table_size = 32;
	Table endpoints = CreateTable(table_size);
	InsertTableEntry(&endpoints, "/", "assets/text/home.html");
	InsertTableEntry(&endpoints, "/path/a", "assets/text/a.html");
	InsertTableEntry(&endpoints, "/path/b", "assets/text/b.html");
	InsertTableEntry(&endpoints, "/path/c", "assets/text/c.html");
	InsertTableEntry(&endpoints, "/image", "assets/images/home.png");
	InsertTableEntry(&endpoints, "/image/a", "assets/images/a.png");
	InsertTableEntry(&endpoints, "/image/b", "assets/images/b.png");
	InsertTableEntry(&endpoints, "/image/c", "assets/images/c.png");
	return endpoints;
}

void UnloadEndpoints(Table* table) {
	DestroyTable(table);
}

const char* ResolveEndpoint(Table* table, const char* str) {
	return GetTableValue(table, str);
}