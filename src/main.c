#include "editor.h"
#include<string.h>
#include<stdlib.h>

extern FILE *fp;
extern FILE *ed_log;

extern  char fname[20];

int main(int argc,char *argv[]){
	if(argc<2)
		strcpy(fname,DEFAULT_FILE);
	else
    		strcpy(fname,argv[1]);

	ed_log=fopen("editor.log","w");
	fprintf(ed_log,"=========================LOGS=============================\n");
  	if(!(fp=fopen(fname,"r+w"))){
  		if(!(fp=fopen(fname,"w"))){
   			perror("ERROR : Unexpected error accured while tryong to open/create file.\n");
  		}
 	}

  	//run the editor
  	edit();
  	return 0;

}
