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
	//Remove newline
	strtok(string, "\n");
	char *tokens;
	// tokenize by white space
	tokens = strtok(string," ");
	char *command = malloc(strlen(tokens));
	strcpy(command,tokens);
	char* tokens1 = strtok(NULL," ");
	char *path = malloc(strlen(tokens1));
	strcpy(path,tokens1);
	if(strcmp(command,"DocumentRoot") == 0){
		documentroot = malloc(strlen(path)+1);
		strcpy(documentroot,path);
	}
	else if(strcmp(command, "OutputArea") == 0){
		outputarea = malloc(strlen(path)+1);
		strcpy(outputarea,path);
	}
	else if(strcmp(command, "Alias") == 0){
		if(count >= 20){
			printf("Aliases reached maximum value of 20 \n");
			return;
		}
		else{
			char* token = strtok(NULL," ");
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

char* checkforalias(char *filename){
	for(int i=0;i<count;i++){
		if(strncmp(filename,*(alias+i),strlen(*(alias+i))) == 0){
			char *newfilename = malloc(strlen(aliasvalue[i])+strlen(filename));
			strcpy(newfilename,aliasvalue[i]);
			strcat(newfilename,"/");
			strcat(newfilename,filename+strlen(alias[i]));
			return newfilename;
		}
	}
	return NULL;
}

char* parseinputfile(char *filename){
	char *newinputfile;
	int index = 0;
	char **tokens = malloc(sizeof(char *)*50);
	char *token = strtok(filename,"/");
	while(token!=NULL){
		if(strcmp(token,"..") == 0){
			index--;
		}
		else if(strcmp(token,".") !=0){
			*(tokens+index) = token;
			index++;
		}
		token = strtok(NULL,"/");
		if(index <0){
			return NULL;
		}
	}
	newinputfile = malloc(strlen(filename));
	for(int i=0;i<index; i++){
		strcat(newinputfile,"/");
		strcat(newinputfile,*(tokens+i));
	}
	return newinputfile;
}

void copyfiles(char* my_string){
	char *files;
	strtok (my_string,"\n");
	files = strtok(my_string," ");
	char *file1 = files;
	files = strtok (NULL, " ");
	char *file2 = files;
	char *newinputfile = parseinputfile(file1);
	if(newinputfile == NULL){
		printf("Input file goes out of document directory \n");
		return;
	}
	FILE *src_ptr, *dest_ptr;
	char *src_filename = malloc(strlen(documentroot)+strlen(newinputfile)+2);
	if(newinputfile[0] != '/' && documentroot[strlen(documentroot)-1] != '/'){
   	strcpy(src_filename,documentroot);
   	strcat(src_filename,"/");
   	strcat(src_filename,newinputfile);	
	}
	else if(newinputfile[0] == '/' && documentroot[strlen(documentroot)-1] == '/'){
		strcpy(src_filename,documentroot);
   	strcat(src_filename,newinputfile+1);
	}
	else{
		strcpy(src_filename,documentroot);
   	strcat(src_filename,newinputfile);
	}
	char buf;
	src_ptr = fopen(src_filename,"r");
	// Check error for opening the file
	if ( src_ptr == NULL){
		newinputfile = checkforalias(newinputfile);
		fclose(src_ptr);
		if(newinputfile == NULL){
			printf("No alias found : Invalid input file \n");
			return;
		}
		src_ptr = fopen(newinputfile,"r");
		if(src_ptr == NULL){
			printf("%s: X %s \n",newinputfile, strerror(errno));
			return;
		}
	}
	pid_t pid;
	int status;
	if ((pid = fork()) < 0) {
 		printf("fork error \n");
	} else if (pid == 0) {      /* child */
		if(chroot(outputarea) !=0){
			printf("chroot :%s \n",strerror(errno));
			exit(127);
		}
		if(chdir("/")!=0){
			printf("chdir : %s \n",strerror(errno));
			exit(127);
		}
		dest_ptr = fopen(file2,"w");
		if ( dest_ptr == NULL){
			printf("%s: X %s \n",file2, strerror(errno));
			exit(127);
		}
		// Copy contents until end of line
		buf = fgetc(src_ptr);
		while(buf != EOF){
			fputc(buf,dest_ptr);
			buf = fgetc(src_ptr);
		}

		printf("Copied files \n");
		// Close file pointer
		fclose(dest_ptr);
		fclose(src_ptr);
		exit(127);
	}
	if ((pid = waitpid(pid, &status, 0)) < 0)
		printf("waitpid error \n");

	// Close file pointer
	fclose(src_ptr);
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
