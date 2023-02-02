#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "commands.h"

int main(int argc, char *argv[])
{
	char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

	char input_buf[MAX_STR_LEN + 1];
	char input_cpy[MAX_STR_LEN + 1];
	input_buf[MAX_STR_LEN] = '\0';
	char *token_arr[MAX_STR_LEN] = {NULL};
	char *token_cpy[MAX_STR_LEN] = {NULL};
	int variablecount=0;
	Node *front = create_head();

	while (1)
	{
		// Prompt and input tokenization

		display_message(prompt);

		int ret = get_input(input_buf);
		strcpy(input_cpy, input_buf);
		size_t token_count = tokenize_input(input_buf, token_arr);

		// clean exits
		if (ret != -1 && (token_count == 0 || (strcmp("exit", token_arr[0]) == 0)))
		{
			if (token_count != 0)
			{
				freeList(front);
				break;
			}
			else if (token_count == 0 && (strcmp("", input_buf) == 0))
			{
				freeList(front);
				break;
			}
			else
			{
				char *blank = "";
				display_message(blank);
			}
		}

		// Command execution
		if (token_count >= 1)
		{
			int i, checkcommand2;
			const char *ch2 = "$";
			char ch_arr[64];
			for (i = 0; i < token_count; i++)
			{
				// if it is then it checks the linked list to see if the variable exists
				checkcommand2 = strncmp(token_arr[i], ch2, 1);
				if (checkcommand2 == 0)
				{
					strcpy(ch_arr, token_arr[i]);
					char *contents_chopped = ch_arr + 1;
					int j;
					j = checker(contents_chopped, front);
					if (j == 0)
					{
						strcpy(token_arr[i], contents_chopped);
						set_value(token_arr[i], front);
						variablecount++;
					}
				}
			}

			// variables
			int j, pipecount = 0;
			const char ch3 = '|';
			char *ret;
			for (j = 0; j < token_count; j++){
				// pipes dont change directory, assign variable, normal ls, echo normal
				// checks if pipe
				ret = strchr(token_arr[j], ch3);
				if (ret != NULL){
				pipecount++;
				}
			}
			if (pipecount > 0){
				int pipecountnum=0;
				// its here, need to get the updated one instead of inputcpy
				if (variablecount != 0){
					variablecount = 0;
				}
				else{
					token_count = tokenize_pipe(input_cpy, token_arr);
				}
				
				token_count = tokenize_pipe_2(token_arr, token_cpy, token_count);

				pipecountnum = pipecounter(token_cpy, token_count);

				if (piper(token_cpy, token_count, pipecountnum) == 0){
					freeList(front);
					break;
				}
				}

			else{

			bn_ptr builtin_fn = check_builtin(token_arr[0]);
			if (builtin_fn != NULL)
			{
				ssize_t err = builtin_fn(token_arr);
				if (err == -1)
				{
					display_error("ERROR: Builtin failed: ", token_arr[0]);
				}
			}
			else
			{
				// checks if potential assignment
				char *checkstr;
				const char ch = '=';
				checkstr = strchr(token_arr[0], ch);
				if (checkstr != NULL)
				{
					if (token_count > 1)
					{
						// if more than 1 token then its not a real assignment.
						display_error("ERROR: Invalid Assignment: ", token_arr[0]);
					}
					else
					{
						// otherwise, make a linked list node for the variable.
						front = assign_variable(token_arr[0], front);
					}
				}
				else
				{
					display_error("ERROR: Unrecognized command: ", token_arr[0]);
				}
			}
		}
		}
	}

	return 0;
}
