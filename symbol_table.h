#ifndef HASH_H
#define HASH_H

#include <stdbool.h>

//contains hash map of all variables/labels found in the .asm file
//its the symbol table
//the maximum amount of entries it can hold is 128

void add_entry_symbol_table(char *string, int address);
bool contains_entry_symbol_table(char *string);
int get_address_entry_symbol_table(char *string);


#endif