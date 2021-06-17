#include <stdbool.h>
#include <string.h>

#define MAX_TABLE_SIZE 128
#define N_SLOTS 3

//MAX_TABLE_SIZE * 3 is used to allow for linear probing
//essentially the table has MAX_TABLE_SIZE of N_SLOTS slot buckets
//first a string gets hashed to one of the buckets
//then % N_SLOTS on the hash to identify the slots in the bucket
//this should keep the number of collisions at a minimum
static int table[MAX_TABLE_SIZE * N_SLOTS] = {-1};

//based on the polynomial rolling hash function
static unsigned long compute_hash(char *string) {
	const unsigned long p = 31;
	const unsigned long m = 1e9 + 9;
	unsigned long hash_value = 0;
	unsigned long p_pow = 1;
	int character_value = 0;
	for(int i = 0; i < strlen(string); i++) {
		//following convention is used for character values
		//just use ascii code - 46, this guarantees that all digits and letters will have a value >0
		//this is quick and dirty but prob good enough for the small hash table
		character_value = string[i] - 46;
		hash_value = (hash_value + (character_value * p_pow)) % m;
		p_pow = (p_pow * p) % m;
	}
	return hash_value;
}

int get_table_index(unsigned long hash_value) {
	int bucket_number = hash_value % MAX_TABLE_SIZE;
	int slot_number = hash_value % N_SLOTS;
	int table_index = bucket_number * N_SLOTS + slot_number;
	return table_index;
}

void add_entry_symbol_table(char *string, int address) {
	table[get_table_index(compute_hash(string))] = address;
}

bool contains_entry_symbol_table(char *string) {
	if(table[get_table_index(compute_hash(string))] != -1) {
		return true;
	}
	else {
		return false;
	}
}

int get_address_entry_symbol_table(char *string) {
	return table[get_table_index(compute_hash(string))];
}