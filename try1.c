#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *documentroot = NULL;
char *outputarea = NULL;
char **alias = NULL;
char **aliasvalue = NULL;
int count = 0;



void parsetext(char* string){
	strtok(string, "\n");
	char *tokens;
	tokens = strtok(string," ");
	char *command = malloc(strlen(tokens));
	strcpy(command,tokens);
	char* tokens1 = strtok(NULL," ");
	char *path = malloc(strlen(tokens1));
	strcpy(path,tokens1);
	if(strcmp(command,"DocumentRoot") == 0){
		documentroot = malloc(strlen(path)+1);
		strcpy(documentroot,path);
		//printf("Document Root :%s \n",documentroot);
	}
	else if(strcmp(command, "OutputArea") == 0){
		outputarea = malloc(strlen(path)+1);
		strcpy(outputarea,path);
		//printf("Output Area :%s \n",outputarea);
	}
	else if(strcmp(command, "Alias") == 0){
		if(count >= 20){
			printf("Aliases reached maximum value of 20 \n");
			return;
		}
		else{
			char* token = strtok(NULL," ");
			//printf("Alis %s and alias count %s \n",tokens1,token);
			alias[count] = malloc(strlen(tokens1)*sizeof(char));
			strcpy(alias[count],tokens1);
			aliasvalue[count] = malloc(strlen(token)*sizeof(char));
			strcpy(aliasvalue[count],token);
			count++;
		}
	}
}

void parseconfigfile(char* configfile){
	FILE *src_ptr;
	src_ptr = fopen(configfile,"r");
	if(src_ptr == NULL){
		printf("%s : %s \n", configfile, strerror(errno));
		return;
	}
	int bytes_read;
	size_t nbytes = 0;
	char* my_string = NULL;
	while((bytes_read = getline (&my_string, &nbytes, src_ptr)) != -1){
		parsetext(my_string);
		free(my_string);
		my_string = NULL;
		nbytes = 0;
	}
	fclose(src_ptr);
}

void copyfiles(char* my_string){
	for(int i = 0; i<count; i++){
		printf("Alias :%s, Alias value :%s \n",alias[i],aliasvalue[i]);
	}
	char *files;
	strtok (my_string,"\n");
	files = strtok(my_string," ");
	char *file1 = files;
	files = strtok (NULL, " ");
	char *file2 = files;
	FILE *src_ptr, *dest_ptr;
	char *src_filename = malloc(strlen(documentroot)+strlen(file1)+2);
	if(file1[0] != '/' && documentroot[strlen(documentroot)-1] != '/'){
   	strcpy(src_filename,documentroot);
   	strcat(src_filename,"/");
   	strcat(src_filename,file1);	
	}
	else if(file1[0] == '/' && documentroot[strlen(documentroot)-1] == '/'){
		strcpy(src_filename,documentroot);
   	strcat(src_filename,file1+1);
	}
	else{
		strcpy(src_filename,documentroot);
   	strcat(src_filename,file1);
	}
	char buf;
	src_ptr = fopen(src_filename,"r");
	// Check error for opening the file
	if ( src_ptr == NULL){
		printf("%s: X %s \n",src_filename,strerror(errno));
		return ;
	}
	dest_ptr = fopen(file2,"w");
	if ( dest_ptr == NULL){
		printf("%s: X %s \n",file2, strerror(errno));
		return ;
	}
	// Copy contents until end of line
	buf = fgetc(src_ptr);
	while(buf != EOF){
		fputc(buf,dest_ptr);
		buf = fgetc(src_ptr);
	}
	// Close file pointers
	fclose(src_ptr);
	fclose(dest_ptr);
}

int main(int argc,char *argv[]){

	alias = malloc(sizeof(char *)*20);
	aliasvalue = malloc(sizeof(char *)*20);
	if(argc != 3){
		printf(" Invalid number of arguments \n");
	}
	else{
		if(strcmp(argv[1],"-c") != 0){
			printf("Invalid argument \n");
		}
		else{
			char *configfile = argv[2];
			parseconfigfile(configfile);
			int bytes_read;
			size_t nbytes = 0;
			char *my_string = NULL;
			while((bytes_read = getline (&my_string, &nbytes, stdin)) != -1){
				copyfiles(my_string);
			}
		}
	}
	return 0;
}
