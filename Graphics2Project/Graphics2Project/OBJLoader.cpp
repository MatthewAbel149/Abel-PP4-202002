#include "OBJLoader.h"

bool LoadOBJ(const char* filepath, OBJ_DATA modelOutput)
{
	FILE* file;
	fopen_s(&file, filepath, "r");

	if (file == NULL) {
		printf("Cannot open file!\n");
		return false;
	}

	while (true)
	{
		char lineheader[128];
		// read the first word of a line
		int res = fscanf_s(file, "%s", lineheader);
		
		if (res == EOF)
			break; //if the end of the file is reached, break the loop
		
		// else parse the line header
		if (strcmp(lineheader, "v") == 0) 
		{

		}

	}



}