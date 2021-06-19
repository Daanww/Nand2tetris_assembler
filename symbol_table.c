#include <stdbool.h>
#include <string.h>

#define TABLE_SIZE (1024 * 1024)
#define KEY_MAX_LENGTH (128)

typedef struct Data
{
	int value;
	char key[KEY_MAX_LENGTH];
} data;

static data table[TABLE_SIZE] = {0,{0}};


//based on the polynomial rolling hash function
static unsigned long compute_hash(char *string) {
	const unsigned long p = 67;
	const unsigned long m = 1e9 + 9;
	unsigned long hash_value = 0;
	unsigned long p_pow = 1;
	int character_value = 0;
	for(int i = 0; i < strlen(string); i++) {
		//following convention is used for character values
		//just use ascii code - 0, this guarantees that all digits and letters will have a value >0
		//this is quick and dirty but prob good enough for the small hash table
		character_value = string[i] - 0;
		hash_value = (hash_value + (character_value * p_pow)) % m;
		p_pow = (p_pow * p) % m;
	}
	return hash_value;
}

int get_table_index(unsigned long hash_value) {
	int table_index = hash_value % TABLE_SIZE;
	return table_index;
}


//computes hash, gets table index from that and then checks if that key is at that index.
//uses linear probing to find extra slots if a collision occurs
//first checks two positions above the hashed index (index-1 and index-2) and then two positions below (index+1 index+2)
//if theres still no empty slot found or the string is not found then it throws an error
//returns index in the table of the key if it is found
//if empty=true, then it returns the index of the empty slot it finds where the key can be added
//returns -1 on error/no viable slot found
int find_index_key(char *string, bool empty) {
	int index = get_table_index(compute_hash(string));
	//check if index contains string
	if(strcmp(table[index].key, string) == 0 || (table[index].key[0] == 0 && empty)) {
		return index;
	}
	else if(strcmp(table[index-1].key, string) == 0 || (table[index-1].key[0] == 0 && empty)) {
		return index-1;
	}
	else if(strcmp(table[index-2].key, string) == 0 || (table[index-2].key[0] == 0 && empty)) {
		return index-2;
	}
	else if(strcmp(table[index+1].key, string) == 0 || (table[index+1].key[0] == 0 && empty)) {
		return index+1;
	}
	else if(strcmp(table[index+2].key, string) == 0 || (table[index+2].key[0] == 0 && empty)) {
		return index+2;
	}
	else {
		return -1;
	}

}

//returns 0 on succesfull addition, returns -1 on unsuccesfull addition
int add_entry_symbol_table(char *string, int address) {
	int index = find_index_key(string, true);
	if(index != -1) {
		strcpy(table[index].key, string);
		table[index].value = address;
		return 0;
	}
	else {
		return -1;
	}
}

//checks if table contains a symbol and returns the index if it finds it
//returns -1 if its not found
int contains_entry_symbol_table(char *string) {
	int index = find_index_key(string, false);
	if(index != -1) {
		return index;
	}
	else {
		return -1;
	}
}

//returns address in symbol table
int get_address_entry_symbol_table(int index) {
	return table[index].value;
}