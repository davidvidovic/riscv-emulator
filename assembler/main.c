#include <stdlib.h>
#include <stdio.h>

int main()
{
	char* line;
	char instruction[3][10];
	size_t len = 0;
	size_t read;
	int j = 0;
	FILE* input_asm_file;
	input_asm_file = fopen("../input.asm", "r");
	
	while((read = getline(&line, &len, input_asm_file)) != -1)
	{
		int i = 0;
        while(line[i] != ' ')
        {
        	instruction[j][i] = line[i];
        	i++;
        }
        instruction[j++][i++] = 0;
        
        // TERMINATING ASSEMBLER FOR NOW
        
    }
    
    fclose(input_asm_file);
}
