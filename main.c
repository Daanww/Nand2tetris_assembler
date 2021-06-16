#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

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
int current_line_raw = 0; //current line in the unformatted document that is being read as input, starting at line 0
bool keep_reading = true;

void handle_error(char* erroneous_string, int string_data_type, int error_type) {


	if(error_type == QUIT_APPLICATION) {
		char *string_data_type_string = NULL;

		const char A_instruction_string[] = "A_instruction";
		const char DEST_string[] = "dest";
		const char COMP_string[] = "comp";
		const char JUMP_string[] = "jump";
		switch (string_data_type)
		{
		case A_INSTRUCTION:
			string_data_type_string = &A_instruction_string;
			break;
		
		case DEST:
			string_data_type_string = &DEST_string;
			break;

		case COMP:
			string_data_type_string = &COMP_string;
			break;

		case JUMP:
			string_data_type_string = &JUMP_string;
			break;

		default:
			break;
		}

		printf("ERROR: On line %i: \"%s\" is not a %s.\n", current_line_raw, erroneous_string, string_data_type_string);

		
	}
	else if(error_type == EOF_INDICATOR) {
		printf("End of file reached!\n");
	}
	keep_reading = false;
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

//converts the decimal number contained in the A-instruction to a binary representation that can be parsed into the .hack file
int convert_dec_to_bin(char *buffer)
{
	int error = 0;
	//check if buffer contains only @ and digits
	int digit_length = 0;
	char digits[] = "1234567890";
	digit_length = strspn(&buffer[1], digits); //start scanning at buffer[1] because buffer[0] should be @
	int string_length = strlen(buffer);
	if((digit_length + 1) != string_length) {
		printf("ERROR: An A-instruction someting other than digits.\n");
		error = QUIT_APPLICATION;
		return error;
	}

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
		error = QUIT_APPLICATION;
		return error;
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

	return error;
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
//returns NULL on error
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
					return NULL;
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
				break;
		}

	}
	else {
		printf("ERROR: Cant find 'J' in jump part of D-instruction.\n");
		return NULL;
	}
	return jump_binary;
}

//returns a pointer to string of 3 characters that are the dest part of the binary D-instruction
//returns NULL on error
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
		if(buffer[i] == 'D')
			dest_binary[1] = '1';
		if(buffer[i] == 'M')
			dest_binary[2] = '1';
	}
	return dest_binary;
}

//returns a pointer to string of 7 characters (a and c1 to c6) that are the comp part of the binary D-instruction
//returns NULL on error
char* get_comp_binary(char *buffer, int dest_character_location, int jump_character_location) {

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
	int comp_length = end_of_comp - start_of_comp;

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
			break;
		}
	}
	else if(comp_length == 2) {
		//handle comp (-1, !D, !A, -D, -A, !M, -M)
	}
	else if(comp_length == 3) {
		//handle comp (D+1, A+1, D-1, A-1, D+A, D-A, A-D, D&A, D|A, M+1, M-1, D+M, D-M, M-D, D&M, D|M)
	}
	else {
		//handle errors
	}

	return comp_binary;
}



int main( int argc, char *argv[]) {

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

	//parsing lines from asm file, converting them, then parsing into hack file
	char asm_buffer[MAX_LINE_LENGTH] = {0};

	while(read_line(asm_file, asm_buffer) == 0) {
		//format line, removing comments and whitespace
		format_line(asm_buffer);
		if(strlen(asm_buffer) == 0) //skipping line if its length is zero after formatting
			continue;


		printf("Read line: %s\n", asm_buffer);
		
		if(asm_buffer[0] == '@')
		{
			//handle A-instruction

			if(convert_dec_to_bin(asm_buffer) == QUIT_APPLICATION)
				return 0;
			printf("A-Instruction Line: %s\n", asm_buffer);
		}
		else {
		
			//handle D-instruction
			int jump_loc = contains_jump(asm_buffer);
			char *jump_binary = NULL;
			if(jump_loc != 0) {
				jump_binary = get_jump_binary(asm_buffer, jump_loc);
				printf("D-Instruction Jump: %s\n", jump_binary);
			}
			else {
				printf("D-Instruction does not contain jump!\n");
			}

			int dest_loc = contains_dest(asm_buffer);
			char *dest_binary = NULL;
			if(dest_loc != 0) {
				dest_binary = get_dest_binary(asm_buffer, dest_loc);
				printf("D-Instruction Dest: %s\n", dest_binary);
			}
			
		
		}
	}


	fclose(asm_file);
	fclose(hack_file);

	return 0;
}