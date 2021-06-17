#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "symbol_table.h"

#define MAX_LINE_LENGTH 128

enum { //error types
	QUIT_APPLICATION = 1,
	EOF_INDICATOR = 2
};

enum { //error string data types
	A_INSTRUCTION,
	DEST,
	COMP,
	JUMP
};

//global variables
int current_line_raw = 0; //current line in the unformatted document that is being read as input, starting at line 1

void handle_asm_code_error(char* erroneous_string, int string_data_type) {

		const char *string_data_type_string = NULL;

		const char A_instruction_string[] = "A_instruction";
		const char DEST_string[] = "dest";
		const char COMP_string[] = "comp";
		const char JUMP_string[] = "jump";

		switch (string_data_type)
		{
		case A_INSTRUCTION:
			string_data_type_string = A_instruction_string;
			break;
		
		case DEST:
			string_data_type_string = DEST_string;
			break;

		case COMP:
			string_data_type_string = COMP_string;
			break;

		case JUMP:
			string_data_type_string = JUMP_string;
			break;

		default:
			break;
		}

		printf("ERROR: On line %i: \"%s\" is not a %s.\n", current_line_raw, erroneous_string, string_data_type_string);

		exit(0);
}

//checks arguments given when the program is executed
int check_arguments(int argc, char *argv[]) {
	int error = 0;
	if(argc > 2) {
		printf("ERROR: Too many arguments supplied.\n");
		error = QUIT_APPLICATION;
		return error;
	}
	else if(argc < 2) {
		printf("ERROR: A .asm file is expected as argument.\n");
		error = QUIT_APPLICATION;
		return error;
	}

	char *dot_asm_location = strstr(argv[1], ".asm");
	if(dot_asm_location == NULL) {
		printf("ERROR: The supplied argument is not a .asm file.\n");
		error = QUIT_APPLICATION;
		return error;
	}
	unsigned int string_length = strlen(argv[1]);
	int characters_before_dot_asm = (dot_asm_location - argv[1]) / sizeof(char);
	if((string_length - characters_before_dot_asm) != 4) { //comparing total length of string to amount of characters before .asm, should equal 4 because .asm is 4 chars
		printf("ERROR: The supplied argument is not a .asm file.\n");
		error = QUIT_APPLICATION;
		return error;
	}
	return error;
}

//reads a line from .asm file
int read_line(FILE *file, char *buffer)
{
	current_line_raw++; //updating to the next line

	int error = 0;
	char *EOF_check = NULL;
	EOF_check = fgets(buffer, MAX_LINE_LENGTH, file);
	if(EOF_check == NULL)
	{
		printf("End of file reached!\n");
		error = EOF_INDICATOR;
		return error;
	}
	if(strchr(buffer, '\n') == NULL)
	{
		printf("ERROR: A line in .asm file is too long! (lines can be max %i characters long)\n", (MAX_LINE_LENGTH-1));
		error = QUIT_APPLICATION;
		return error;
	}
	return error;
}

//formats the current line from .asm file for processing
void format_line(char *buffer)
{
	char new_buffer[MAX_LINE_LENGTH] = {0};
	int slash_flag = 0;
	static int long_comment_flag = 0; //long comment flag that carries over between lines, for /* */ comments
	int new_buffer_index = 0;

	//remove all whitespace and stop if it encounters //, /* or \n
	for(int i=0; i<MAX_LINE_LENGTH; i++) {

		if(long_comment_flag != 0) { //after a /*
			if(buffer[i] == '/' && long_comment_flag == 2) { //checks for / in */
				long_comment_flag = 0;
			}
			
			if(buffer[i] == '*') { //check for * in */
				long_comment_flag = 2;
			}
			continue;
		}

		if(buffer[i] == '\r' || buffer[i] == '\n') {
			break;
		}
		if((buffer[i] == '/') && slash_flag) { //for handling //
			//last added character should be / so replace that with \0
			new_buffer[new_buffer_index-1] = '\0';
			break;
		}
		if((buffer[i] == '*') && slash_flag) { //for handling /*  */
			long_comment_flag = 1;
			new_buffer[new_buffer_index-1] = '\0';
			break;
		}

		slash_flag = 0;
		if(buffer[i] == '/')
			slash_flag = 1;

		if((buffer[i] != ' ') && (buffer[i] != '\t')) {
			new_buffer[new_buffer_index] = buffer[i];
			new_buffer_index++;
		}
	}
	//copying new_buffer to buffer
	memcpy(buffer, &new_buffer, (MAX_LINE_LENGTH * sizeof(char)));
}

//checks if a A_instruction contains a variable or a hard coded number
bool is_variable(char *buffer) {
	//check if buffer contains only @ and digits
	int digit_length = 0;
	char digits[] = "1234567890";
	digit_length = strspn(&buffer[1], digits); //start scanning at buffer[1] because buffer[0] should be @
	int string_length = strlen(buffer);
	if((digit_length + 1) != string_length) {
		return true;
	}
	else {
		return false;
	}
}

//converts the decimal number contained in the A-instruction to a binary representation that can be parsed into the .hack file
void convert_dec_to_bin(char *buffer)
{
	int error = 0;

	//legacy code for checking if buffer is a valid @ instruction, might be needed if is_variable() doest work properly
	/*
	//check if buffer contains only @ and digits
	int digit_length = 0;
	char digits[] = "1234567890";
	digit_length = strspn(&buffer[1], digits); //start scanning at buffer[1] because buffer[0] should be @
	int string_length = strlen(buffer);
	if((digit_length + 1) != string_length) {
		printf("ERROR: An A-instruction someting other than digits.\n");
		handle_asm_code_error(buffer, A_INSTRUCTION);
	}
	*/

	//converting the string number in the A-instruction into an actual number

	int number = 0;
	int start_position = 0;
	for(int i = 10; i >= 0; i--) //because the line should be already formatted and the max numbers that can be accepted are 16383 you dont have to search through the entire array
	{
		if(buffer[i] == '@')
			break;
		
		if(buffer[i] != 0 && start_position == 0)
			start_position = i;

		if(start_position != 0) {
			number += ((int)buffer[i]-48) * pow(10, start_position-i);
			
		}
		
	}
	
	//checking if number > 32767
	if(number > 32767) {
		printf("ERROR: An A-instruction can hold a maximum value of 32767.\n");
		handle_asm_code_error(buffer, A_INSTRUCTION);
	}

	char new_buffer[MAX_LINE_LENGTH] = {0};
	new_buffer[0] = '0';
	int power_two_table[] = {32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
	int temp_number = 0;
	//algorithm om ervoor te zorgen dat new_buffer de 16 bit binary code bevat die het nummer in oude buffer representeerde
	//algorithm: https://www.wikihow.com/Convert-from-Decimal-to-Binary
	//de 2e methode gebruikt alleen integer substraction dus is cheap en fast
	for(int i = 1; i < 16; i++) {
		temp_number = number;
		temp_number = temp_number - power_two_table[i];
		if(temp_number >= 0) {
			number = temp_number;
			new_buffer[i] = '1';
		}
		else {
			new_buffer[i] = '0';
		}
	}
	//adding end to string
	new_buffer[16] = '\0';
	//copying new_buffer to buffer
	memcpy(buffer, &new_buffer, (MAX_LINE_LENGTH * sizeof(char)));

}

//check if a D-instruction contains a jump and returns the location of the jump character ';'
int contains_jump(char *buffer) {
	char *jump_index = strchr(buffer, ';');
	if(jump_index != NULL) {
		return (int)(((jump_index - buffer) / sizeof(char)));
	}
	else
		return 0;
}

//check if a D-instruction contains a dest and returns the location of the dest character '='
int contains_dest(char *buffer) {
	char *dest_index = strchr(buffer, '=');
	if(dest_index != NULL) {
		return (int)(((dest_index - buffer) / sizeof(char)));
	}
	else
		return 0;
}

//returns a pointer to string of 3 characters that are the jump part of the binary D-instruction
//returns "000" on error
char* get_jump_binary(char *buffer, int jump_character_location) {
	
	static char jump_binary[4] = {'\0'};

	if(buffer[jump_character_location+1] == 'J') {
		switch(buffer[jump_character_location+2]) {
			case 'G':
				if(buffer[jump_character_location+3] == 'T') {
					//JGT
					jump_binary[0] = '0';
					jump_binary[1] = '0';
					jump_binary[2] = '1';
					break;
				}
				else if(buffer[jump_character_location+3] == 'E') {
					//JGE
					jump_binary[0] = '0';
					jump_binary[1] = '1';
					jump_binary[2] = '1';
					break;
				}
				else {
					//handle error
					printf("ERROR: Unkown Jump condition.\n");
					handle_asm_code_error(buffer, JUMP);
					return NULL;
				}

			case 'E':
				//JEQ
				jump_binary[0] = '0';
				jump_binary[1] = '1';
				jump_binary[2] = '0';
				break;

			case 'L':
				if(buffer[jump_character_location+3] == 'T') {
					//JLT
					jump_binary[0] = '1';
					jump_binary[1] = '0';
					jump_binary[2] = '0';
					break;
				}
				else if(buffer[jump_character_location+3] == 'E') {
					//JLE
					jump_binary[0] = '1';
					jump_binary[1] = '1';
					jump_binary[2] = '0';
					break;
				}
				else {
					//handle error
					printf("ERROR: Unkown Jump condition.\n");
					handle_asm_code_error(buffer, JUMP);
				}

			case 'N':
				//JNE
				jump_binary[0] = '1';
				jump_binary[1] = '0';
				jump_binary[2] = '1';
				break;

			case 'M':
				//JMP
				jump_binary[0] = '1';
				jump_binary[1] = '1';
				jump_binary[2] = '1';
				break;

			default :
				//null
				jump_binary[0] = '0';
				jump_binary[1] = '0';
				jump_binary[2] = '0';
				printf("ERROR: Unkown Jump condition.\n");
				handle_asm_code_error(buffer, JUMP);

				break;
		}

	}
	else {
		printf("ERROR: Cant find 'J' in jump part of D-instruction.\n");
		handle_asm_code_error(buffer, JUMP);
	}
	return jump_binary;
}

//returns a pointer to string of 3 characters that are the dest part of the binary D-instruction
//returns "000" on error
char* get_dest_binary(char *buffer, int dest_character_location) {
	
	static char dest_binary[4] = {'0', '0', '0','\0'};

	//resetting the string every function call
	dest_binary[0] = '0';
	dest_binary[1] = '0';
	dest_binary[2] = '0';
	//assuming that the instruction starts with the destination, for example: DM=D+1
	//thus searching through the first n characters of the string defined by dest_character_location
	//and then checking if that character is 'A', 'D' or 'M' and setting that bit if it is
	for(int i = 0; i < dest_character_location; i++) {
		if(buffer[i] == 'A')
			dest_binary[0] = '1';
		else if(buffer[i] == 'D')
			dest_binary[1] = '1';
		else if(buffer[i] == 'M')
			dest_binary[2] = '1';
		else {
			handle_asm_code_error(buffer, DEST);
		}
	}
	return dest_binary;
}

//returns a pointer to string of 7 characters (a and c1 to c6) that are the comp part of the binary D-instruction
//returns NULL on error
char* get_comp_binary(char *buffer, int jump_character_location, int dest_character_location) {

	static char comp_binary[8] = {'0', '0', '0', '0', '0', '0', '0', '\0'};
	
	//resetting comp_binary to allow multiple function calls
	comp_binary[0] = '0';
	comp_binary[1] = '0';
	comp_binary[2] = '0';
	comp_binary[3] = '0';
	comp_binary[4] = '0';
	comp_binary[5] = '0';
	comp_binary[6] = '0';


	int start_of_comp = 0;
	int end_of_comp = 0;

	//if the instruction contains a dest, skip the beginning characters
	if(dest_character_location != 0) {
		start_of_comp = dest_character_location + 1;
	}

	//if the instruction contains a jump, skip the ending characters
	if(jump_character_location != 0) {
		end_of_comp = jump_character_location - 1;
	}
	else {
		end_of_comp = strlen(buffer) - 1; //set end of comp to the index of the final character in the string
	}

	//A comp can be 1, 2 or 3 characters long. Ive chosen to split the matching process of the input buffer and the reference comp characters in 3 parts according to string length
	//e.g. first check if the comp is 1, 2 or 3 characters long then start trying to match the input to the reference comps
	int comp_length = end_of_comp - start_of_comp + 1;

	if(comp_length == 1) {
		//handle comp (0, 1, D, A, M)
		switch (buffer[start_of_comp])
		{

		case '0': //comp 0 is code 0101010
			comp_binary[0] = '0';
			comp_binary[1] = '1';
			comp_binary[2] = '0';
			comp_binary[3] = '1';
			comp_binary[4] = '0';
			comp_binary[5] = '1';
			comp_binary[6] = '0';			
			break;
		
		case '1': //comp 1 is code 0111111
			comp_binary[0] = '0';
			comp_binary[1] = '1';
			comp_binary[2] = '1';
			comp_binary[3] = '1';
			comp_binary[4] = '1';
			comp_binary[5] = '1';
			comp_binary[6] = '1';			
			break;

		case 'D': //comp D is code 0001100
			comp_binary[0] = '0';
			comp_binary[1] = '0';
			comp_binary[2] = '0';
			comp_binary[3] = '1';
			comp_binary[4] = '1';
			comp_binary[5] = '0';
			comp_binary[6] = '0';			
			break;

		case 'A': //comp A is code 0110000
			comp_binary[0] = '0';
			comp_binary[1] = '1';
			comp_binary[2] = '1';
			comp_binary[3] = '0';
			comp_binary[4] = '0';
			comp_binary[5] = '0';
			comp_binary[6] = '0';			
			break;

		case 'M': //comp M is code 1110000
			comp_binary[0] = '1';
			comp_binary[1] = '1';
			comp_binary[2] = '1';
			comp_binary[3] = '0';
			comp_binary[4] = '0';
			comp_binary[5] = '0';
			comp_binary[6] = '0';			
			break;	
		
		default:
			//handle error
			handle_asm_code_error(buffer, COMP);
			break;
		}
	}
	else if(comp_length == 2) {
		//handle comp (-1, -D, -A, -M, !D, !A, !M)
		if(buffer[start_of_comp] == '-') {
			switch (buffer[start_of_comp + 1])
			{
			case '1': //comp -1 is code 0111010
				comp_binary[0] = '0';
				comp_binary[1] = '1';
				comp_binary[2] = '1';
				comp_binary[3] = '1';
				comp_binary[4] = '0';
				comp_binary[5] = '1';
				comp_binary[6] = '0';
				break;
			
			case 'D': //comp -D is code 0001111
				comp_binary[0] = '0';
				comp_binary[1] = '0';
				comp_binary[2] = '0';
				comp_binary[3] = '1';
				comp_binary[4] = '1';
				comp_binary[5] = '1';
				comp_binary[6] = '1';
				break;

			case 'A': //comp -A is code 0110011
				comp_binary[0] = '0';
				comp_binary[1] = '1';
				comp_binary[2] = '1';
				comp_binary[3] = '0';
				comp_binary[4] = '0';
				comp_binary[5] = '1';
				comp_binary[6] = '1';
				break;

			case 'M': //comp -M is code 1110011
				comp_binary[0] = '1';
				comp_binary[1] = '1';
				comp_binary[2] = '1';
				comp_binary[3] = '0';
				comp_binary[4] = '0';
				comp_binary[5] = '1';
				comp_binary[6] = '1';
				break;


			default:
				//handle error
				handle_asm_code_error(buffer, COMP);
				break;
			}
		}
		else if(buffer[start_of_comp] == '!') {
			switch (buffer[start_of_comp + 1])
			{
			
			case 'D': //comp !D is code 0001101
				comp_binary[0] = '0';
				comp_binary[1] = '0';
				comp_binary[2] = '0';
				comp_binary[3] = '1';
				comp_binary[4] = '1';
				comp_binary[5] = '0';
				comp_binary[6] = '1';
				break;

			case 'A': //comp !A is code 0110001
				comp_binary[0] = '0';
				comp_binary[1] = '1';
				comp_binary[2] = '1';
				comp_binary[3] = '0';
				comp_binary[4] = '0';
				comp_binary[5] = '0';
				comp_binary[6] = '1';
				break;

			case 'M': //comp !M is code 1110001
				comp_binary[0] = '1';
				comp_binary[1] = '1';
				comp_binary[2] = '1';
				comp_binary[3] = '0';
				comp_binary[4] = '0';
				comp_binary[5] = '0';
				comp_binary[6] = '1';
				break;

			default:
				//handle error
				handle_asm_code_error(buffer, COMP);
				break;
			}
		}
		else {
			//handle error
			handle_asm_code_error(buffer, COMP);
		}
	}
	else if(comp_length == 3) {
		//handle comp (D+1, A+1, D-1, A-1, D+A, D-A, A-D, D&A, D|A, M+1, M-1, D+M, D-M, M-D, D&M, D|M)
		//sorted for branching: D+1, D+A, D+M, D-1, D-A, D-M, D&A, D&M, D|A, D|M, A+1, A-1, A-D, M+1, M-1, M-D
		//first select on first character then second character then final character

		if(buffer[start_of_comp] == 'D') {
			if(buffer[start_of_comp + 1] == '+') {
				if(buffer[start_of_comp + 2] == '1') {
					//comp D+1 is code 0011111
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '1';
					comp_binary[3] = '1';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
				}
				else if(buffer[start_of_comp + 2] == 'A') {
					//comp D+A is code 0000010
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '0';
				}
				else if(buffer[start_of_comp + 2] == 'M') {
					//comp D+M is code 1000010
					comp_binary[0] = '1';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '0';
				}
				else {
				//handle error
				handle_asm_code_error(buffer, COMP);	
				}
			}
			else if(buffer[start_of_comp + 1] == '-') {
				if(buffer[start_of_comp + 2] == '1') {
					//comp D-1 is code 0001110
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '1';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '0';
				}
				else if(buffer[start_of_comp + 2] == 'A') {
					//comp D-A is code 0010011
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
				}
				else if(buffer[start_of_comp + 2] == 'M') {
					//comp D-M is code 1010011
					comp_binary[0] = '1';
					comp_binary[1] = '0';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
				}
				else {
				//handle error
				handle_asm_code_error(buffer, COMP);	
				}
			}
			else if(buffer[start_of_comp + 1] == '&') {
				if(buffer[start_of_comp + 2] == 'A') {
					//comp D&A is code 0000000
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '0';
					comp_binary[6] = '0';
				}
				else if(buffer[start_of_comp + 2] == 'M') {
					//comp D&M is code 1000000
					comp_binary[0] = '1';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '0';
					comp_binary[6] = '0';
				}
				else {
				//handle error
				handle_asm_code_error(buffer, COMP);	
				}
			}
			else if(buffer[start_of_comp + 1] == '|') {
				if(buffer[start_of_comp + 2] == 'A') {
					//comp D|A is code 0010101
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '0';
					comp_binary[6] = '1';
				}
				else if(buffer[start_of_comp + 2] == 'M') {
					//comp D|M is code 1010101
					comp_binary[0] = '1';
					comp_binary[1] = '0';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '0';
					comp_binary[6] = '1';
				}
				else {
				//handle error
				handle_asm_code_error(buffer, COMP);	
				}
			}
			else {
				//handle error
				handle_asm_code_error(buffer, COMP);	
			}

		}
		else if(buffer[start_of_comp] == 'A') {
			if(buffer[start_of_comp + 1] == '+') {
					//comp A+1 is code 0110111
					comp_binary[0] = '0';
					comp_binary[1] = '1';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
			}
			else if(buffer[start_of_comp + 1] == '-') {
				if(buffer[start_of_comp + 2] == '1') {
					//comp A-1 is code 0110010
					comp_binary[0] = '0';
					comp_binary[1] = '1';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '0';
				}
				else if(buffer[start_of_comp + 2] == 'D') {
					//comp A-D is code 0000111
					comp_binary[0] = '0';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
				}
				else {
					//handle error
					handle_asm_code_error(buffer, COMP);
				}
			}
			else {
				//handle error
				handle_asm_code_error(buffer, COMP);
			}
		}
		else if(buffer[start_of_comp] == 'M') {
			if(buffer[start_of_comp + 1] == '+') {
					//comp M+1 is code 1110111
					comp_binary[0] = '1';
					comp_binary[1] = '1';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
			}
			else if(buffer[start_of_comp + 1] == '-') {
				if(buffer[start_of_comp + 2] == '1') {
					//comp M-1 is code 1110010
					comp_binary[0] = '1';
					comp_binary[1] = '1';
					comp_binary[2] = '1';
					comp_binary[3] = '0';
					comp_binary[4] = '0';
					comp_binary[5] = '1';
					comp_binary[6] = '0';
				}
				else if(buffer[start_of_comp + 2] == 'D') {
					//comp M-D is code 1000111
					comp_binary[0] = '1';
					comp_binary[1] = '0';
					comp_binary[2] = '0';
					comp_binary[3] = '0';
					comp_binary[4] = '1';
					comp_binary[5] = '1';
					comp_binary[6] = '1';
				}
				else {
					//handle error
					handle_asm_code_error(buffer, COMP);
				}
			}
			else {
				//handle error
				handle_asm_code_error(buffer, COMP);
			}
		}
		else {
			//handle error
			handle_asm_code_error(buffer, COMP);
		}

	}
	else {
		//handle errors
		handle_asm_code_error(buffer, COMP);
	}

	return comp_binary;
}

void initialize_symbol_table() {
	add_entry_symbol_table("SP", 0);
	add_entry_symbol_table("LCL", 1);
	add_entry_symbol_table("ARG", 2);
	add_entry_symbol_table("THIS", 3);
	add_entry_symbol_table("THAT", 4);
	add_entry_symbol_table("R0", 0);
	add_entry_symbol_table("R1", 1);
	add_entry_symbol_table("R2", 2);
	add_entry_symbol_table("R3", 3);
	add_entry_symbol_table("R4", 4);
	add_entry_symbol_table("R5", 5);
	add_entry_symbol_table("R6", 6);
	add_entry_symbol_table("R7", 7);
	add_entry_symbol_table("R8", 8);
	add_entry_symbol_table("R9", 9);
	add_entry_symbol_table("R10", 10);
	add_entry_symbol_table("R11", 11);
	add_entry_symbol_table("R12", 12);
	add_entry_symbol_table("R13", 13);
	add_entry_symbol_table("R14", 14);
	add_entry_symbol_table("R15", 15);
	add_entry_symbol_table("SCREEN", 16384);
	add_entry_symbol_table("KBD", 24576);
}


int main( int argc, char *argv[]) {

	/*

	//checking arguments
	if(check_arguments(argc, argv) == QUIT_APPLICATION) { return 0;}

	//opening .asm file
	FILE *asm_file = fopen(argv[1], "r");
	if(asm_file == NULL) {
		printf("ERROR: file \"%s\" is not found.\n", argv[1]);
		return 0;
	}

	//opening .hack file
	char hack_file_name[64];
	strncpy(hack_file_name, argv[1], (strlen(argv[1]) - 4));
	strcat(hack_file_name, ".hack");
	FILE *hack_file = fopen(hack_file_name, "w");

	/**/

	//--------- following section is different for debugging

	//opening .asm file
	FILE *asm_file = fopen("Add.asm", "r");
	if(asm_file == NULL) {
		printf("ERROR: file \"%s\" is not found.\n", "Add.asm");
		return 0;
	}

	//opening .hack file
	char hack_file_name[64];
	strncpy(hack_file_name, "Add.asm", (strlen("Add.asm") - 4));
	strcat(hack_file_name, ".hack");
	FILE *hack_file = fopen(hack_file_name, "w");

	//-----------

	//*/

	//FIRST PASS-------------------------------------
	printf("FIRST PASS---------------\n");
	int ROM_address = 0;

	char first_pass_buffer[MAX_LINE_LENGTH] = {0};
	while(read_line(asm_file, first_pass_buffer) == 0) {
		format_line(first_pass_buffer);
		if(strlen(first_pass_buffer) == 0) //skipping line if its length is zero after formatting
			continue;

		//find label and add it to symbol table
		//assumes labels are identified as: (Xxx) after formatting
		if(first_pass_buffer[0] == '(') {
			char label_name[128] = {0};
			int string_length = strlen(first_pass_buffer);
			strncpy(label_name, &first_pass_buffer[1], (string_length - 2));
			label_name[string_length - 1] = '\0';
			add_entry_symbol_table(label_name, ROM_address);
			printf("Added \"%s\" with value %i to symbol table!\n", label_name, ROM_address);
			continue;
		}
		
		ROM_address++;
	}

	rewind(asm_file);

	//SECOND PASS------------------------------------
	printf("SECOND PASS----------------\n");

	//parsing lines from asm file, converting them, then parsing into hack file
	char asm_buffer[MAX_LINE_LENGTH] = {0};

	//current lowest ram address that is still free for allocation
	int lowest_RAM_address = 16;

	while(read_line(asm_file, asm_buffer) == 0) {
		//format line, removing comments and whitespace
		format_line(asm_buffer);
		if(strlen(asm_buffer) == 0) //skipping line if its length is zero after formatting
			continue;


		printf("Read line: %s\n", asm_buffer);
		
		if(asm_buffer[0] == '@')
		{
			//handle A-instruction

			//handle variables
			if(is_variable(asm_buffer)) {
				int variable_value = 0;

				//get the variable name
				char variable_name[128] = {0};
				strncpy(variable_name, &asm_buffer[1], 128);
				if(variable_name[127] != '\0') {
					handle_asm_code_error(asm_buffer, A_INSTRUCTION);
				}

				//first check if the variable already exists in the symbol table
				if(contains_entry_symbol_table(variable_name)) {
					variable_value = get_address_entry_symbol_table(variable_name);
				}
				else {
					//add new variable to symbol table, assigning it the next free ram address
					add_entry_symbol_table(variable_name, lowest_RAM_address);
					variable_value = lowest_RAM_address;
					lowest_RAM_address++;
					printf("Added \"%s\" with value %i to symbol table!\n", variable_name, variable_value);
				}

				//parse the decimal number into asm_buffer
				sprintf(&asm_buffer[1], "%i", variable_value);
			}
			
			//convert the decimal number to binary code 
			convert_dec_to_bin(asm_buffer);
		
			//Parse the instruction into the .hack file
			printf("A-Instruction Line: %s\n", asm_buffer);
			fputs(asm_buffer, hack_file);
			fputc('\n', hack_file);
		}

		else if(asm_buffer[0] == '(') {
			//handle labels
			//labels are handled in the first pass, thus a label will just be skipped in the second pass
			continue;
		}
		else {
		
			//handle D-instruction

			//handle jump part
			int jump_loc = contains_jump(asm_buffer);
			char *jump_binary = NULL;
			if(jump_loc != 0) {
				jump_binary = get_jump_binary(asm_buffer, jump_loc);
				printf("D-Instruction Jump: %s\n", jump_binary);
			}
			else {
				printf("D-Instruction does not contain jump!\n");
				jump_binary = "000";
			}

			//handle dest part
			int dest_loc = contains_dest(asm_buffer);
			char *dest_binary = NULL;
			if(dest_loc != 0) {
				dest_binary = get_dest_binary(asm_buffer, dest_loc);
				printf("D-Instruction Dest: %s\n", dest_binary);
				
			}
			else {
				printf("D-Instruction does not contain dest!\n");
				dest_binary = "000";
			}
			
			//handle comp part
			char *comp_binary = NULL;
			comp_binary = get_comp_binary(asm_buffer, jump_loc, dest_loc);
			printf("D-Instruction Comp: %s\n", comp_binary);
		

			//concatenate all parts together and parse into .hack file
			char D_instruction_binary[17] = "111";
			strcat(D_instruction_binary, comp_binary);
			strcat(D_instruction_binary, dest_binary);
			strcat(D_instruction_binary, jump_binary);
			fputs(D_instruction_binary, hack_file);
			fputc('\n', hack_file);

		}
	}


	fclose(asm_file);
	fclose(hack_file);

	return 0;
}