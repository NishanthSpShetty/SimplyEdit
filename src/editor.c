#include "editor.h"
#include <termio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "CSyntax.h"

char pos[30];
FILE *fp,*ed_log;
int indent,LINE=1,nlines=0,num_char=0,MODIFIED=0,ins=-1;
int rows,cols,copyN=0;
struct line_t *p[20];
int PASTE=-1;
char buff[80];
char fname[20];
char words[20];
int w_top=0;


int empty_word(){
	if(w_top==0)
		return 1;
	else return 0;
}

void push_word(char ch){
	words[w_top++]=ch;
}
void pop_words(){
	w_top--;
}

void clear_word(){
	int w_i=0;
	for(w_i=0;w_i<w_top;w_i++)
		words[w_i]=0;
	w_top=0;
}

//compare the word string to C tokens
int isCToken(){
	words[w_top]=0;
	yy_scan_string(words);
	return yylex();
}

/*
 * Returns true if character is grouping character'
 *
 */

int is_grouping(char ch){
	if(ch=='('||ch=='{'||ch=='\"' ||ch=='<')
		return 1;
	else
		return 0;
}

/*
 * syntax highlighter
 *
 */
void highlightsyntax(int type){
	int mb=0;
	for(;mb<w_top;mb++)  printf("%c%c%c",27,'[','D');
	switch(type){
		case DATA_TYPE:
				printf("%s",KRED);
			  	break;
		case LOOP:
			       	printf("%s",KGRN);
				break;
		case NUMBER:
				printf("%s",KYEL);
			     	break;
		case LANG_CONSTRUCT:
				printf("%s",KBLU);
				break;
		case RETURN : 
				printf("%s",KMAG);
				break;
	}
	printf("%s%s",words,KNRM);
}


int getch(){
	struct termios oldt,newt;
	int ch;
	tcgetattr(0,&oldt);
  //save old setting to restore it back when read is done
  newt=oldt;
  //disable canonical mode, read one character at a time
	newt.c_lflag&=~(ICANON|ECHO);
	tcsetattr(0,TCSANOW,&newt);
	ch=getchar();
  //reset to original state
	tcsetattr(0,TCSANOW,&oldt);
	return ch;
}



int prev(){
	int type,ind=0;
	char ch;
	fprintf(ed_log,"PREV_MODE");
	if(nlines>0){
		fprintf(ed_log," nlines %d",nlines);
		for(;ind<nlines;ind++)
			free(p[ind]);
	}
	nlines = 0;
	p[nlines]=(struct line_t*)malloc(sizeof(struct line_t));
	if(!p[nlines]){
		printf("out of Memory");
		
		exit(0);
		}
	p[nlines]->nchar=0;

printf("*----------------------------------------------------------------------------------------------------------------------------------------------------*\n");
	fseek(fp,0,0);
//	printf("%2d~",LINE);
	while(1){

		if((ch=fgetc(fp))==-1) break;


		push_word(ch);
		if(ch==' ' || !isalnum(ch) || is_grouping(ch)){
			//printf("DDDDD");
			pop_words();
			if((type=isCToken())){

				highlightsyntax(type);
			}
				clear_word();

		}
		putchar(ch);



		//putchar(ch);
		num_char++;
		if(num_char>139&&ch!='\n'){
			printf("\n   ");
			num_char=0;
		}
		cols++;
		if(ch=='}') indent--;
		if(ch=='\t'){ cols+=7;
				indent++;
			}

		if(ch=='\n'){
			p[nlines]->nchar=cols;
			cols=0;
			num_char=0;
//			printf("%2d~",++LINE);
			nlines++;
			p[nlines]=(struct line_t*)malloc(sizeof(struct line_t));
			p[nlines]->nchar=0;
			if(!p[nlines]){ printf("Out of memory");
					return 1;
			}
			while(ind<indent-1){ putchar('\t'); ind++; }
		}
			ind=0;
		push(ch);
}
	rows=nlines;
return 0;
}


//editor is switched to command mode

int cmdmode(){

	char ch;
	system("tput cup 1 0");
	rows=0,cols=0;
	while(1){
	
	ch=getch();
	fprintf(ed_log,"\nLOG : getch read [ESC] %c ",ch);
	if(ch==':'){
		system("tput cup 40 0");
		printf("%140s\r"," ");
		putchar(ch);
		ch=getchar();

		if(ch=='q'){

		ch=getch();
  		if(MODIFIED){
			system("clear");

			 printf("\n\n\nFile not saved");
			printf("\nDo you want save the changes to the file [Y/N] :");
			fflush(stdin);
			ch=getchar();
			if(ch=='y'||ch=='Y') {
				flushtofile();
				exit(0);
				}
			else {
			   printf("\nChanges descarded..");
				exit(0);
			}
		}

	 	system("clear");
	 	exit(0);

	}
	if(ch=='w'){
		if(flushtofile()){
			printf("\rChanges made to the file is descared...");
		} else printf("\rChanges saved to file : %s",fname);
	}
	else if(ch=='i'){
			insert();
			edit();
			return 0;
		}
	else if(ch=='e'){

		fprintf(ed_log,"\nLOG : switching ed mode  ");
		flushtofile();
	//	fclose(fp);
		 edit();
		}
		else printf("Invalid editor command");
	}
	else if(ch=='a')
		moveback();
	else if(ch=='d')
		movefront();
	else if(ch=='w')
		moveup();
	else if(ch=='s')
		movedown();
	else if(ch=='r')
		delete();
	else if(ch=='z') cutline();
	else if(ch=='l') deleteline();
	else if(ch=='x') cut();
	else if(ch=='v') paste();
		else if(ch=='c') copy();
	}
}


//editor is in edit mode

void edit(){
	int type;
	char ch;
	int ind=0;
	system("clear");
	if(!prev()){
	while(1){
	ch=getch();
	if(ch==27){
		cmdmode();
		return;
	}else if(ch==127)
		pop(); //backspace : pop and descard the charcter
	else {
		push_word(ch);
		if(ch==' ' || !isalnum(ch) || is_grouping(ch)){
			//printf("DDDDD");
			pop_words();
			if((type=isCToken())){

				highlightsyntax(type);
			}
				clear_word();
			}
		putchar(ch);
		cols++;
		if(ch=='}') indent--;
		if(ch=='\t'){ cols+=7; indent++; }
		if(ch=='\n'){
//			printf("%2d~",++LINE);
			rows=++nlines;
			p[nlines]=(struct line_t*)malloc(sizeof(struct line_t));
			if(!p[nlines]) {
				printf("Buffer overflow");
				return;
			}
			cols=0;
			while(ind<indent){
				ind++; putchar('\t');
				}
				ind=0;
		}
		//p[nlines]->nchar=cols;
		push(ch);
		num_char++;
			if(num_char>139){
				printf("\n   ");
				num_char=0;
  				}
	}
	}
	perror("Status ");
	}
	return;

}




char copy(){
	copyN=rows;
	PASTE=rows;
	strcpy(buff,p[rows]->lines);
}

char cut(){
	copyN=rows;
	PASTE=rows;
	strcpy(buff,p[rows]->lines);
	p[rows]->lines[0]=0;
	printf("\r%140s\r"," ");
}



void paste(){
	int r=rows;
	if(PASTE>=0){

	if(copyN>nlines){
	nlines++;
	if(!(p[nlines]=(struct line_t*)malloc(sizeof(struct line_t)))){
		perror("Error"); exit(1);
	}
	rows=nlines;
	copyN++;
	}
	if(r!=PASTE) r++;
	p[rows]->lines[0]='\n';
	p[rows]->nchar=cols+strlen(p[rows]->lines);
	strcpy(p[rows]->lines+cols+1,buff);
	printf("\r%s\r",buff);
	//printf("\r%2d~%s\r",r,buff);
}
}
void deleteline(){
	p[rows]->lines[0]=0;
	p[rows]->nchar=0;
	printf("\r%140s\r"," ");
}

void cutline(){
	p[rows]->lines[cols+1]='\0';
	p[rows]->nchar=cols;
	printf("\r%s%100s\r",p[rows]->lines+1," ");
}

void insert(){
	system("tput cup 38 0");
	printf("Enter the line number : ");
	scanf("%d",&ins);
	printf("\rEnter the text :");
	getch();
	fflush(stdin);
	fgets(buff,sizeof(buff),stdin);
	flushtofile();

}

int flushtofile(){
	int i=0;
	fclose(fp);
	if(!(fp=fopen(fname,"w"))){
		printf("File Error ");
		return -1;
	}

	for(i=0;i<=nlines;i++){
		if(ins==i+1) fprintf(fp,"\n%s",buff);
		if(strlen(p[i]->lines)!=0)
			fprintf(fp,p[i]->lines);
			fprintf(fp,"%c",0);
			fseek(fp,-1,SEEK_CUR);
	}
	MODIFIED=0;
	ins=-1;
	return 0;
}

void push(char ch){
	p[nlines]->lines[(p[nlines]->nchar)++]=ch;
		MODIFIED=1;
	p[nlines]->lines[(p[nlines]->nchar)]=0;
}

void pop(){
	int x=0;
	if(nlines>0&&p[nlines]->nchar==0){
		free(p[nlines]);
		printf("\r%120s\r   "," ");
		LINE--;
		nlines--;
		while(x<p[nlines]->nchar){
			printf("%c%c%c",27,'[','C');
			x++;
		}
		printf("%c%c%c",27,'[','A');
	}
	if(nlines>=0&&(p[nlines]->nchar>0)){
		p[nlines]->lines[--(p[nlines]->nchar)]=0;
		printf("%c%c%c%c%c%c%c",27,'[','D',' ',27,'[','D');
	}else{
		nlines=0;
		if(!(p[nlines]=(struct line_t*)malloc(sizeof(struct line_t)))){ perror("Error"); exit(0); }
		 p[nlines]->nchar=0;
	}
}

void moveback(){
	if(cols>0){
	printf("%c%c%c",27,'[','D');
	cols--;}
}

void movefront(){
	if(cols+1<p[rows]->nchar-2){
	cols++;
	printf("%c%c%c",27,'[','C');
	}
}

void moveup(){
	int up=0;
	system("tput cup 0 0");
	while(up<=rows){ up++;
		printf("%c%c%c",27,'[','B');
	}

	if(rows>0){
	rows--;
	printf("%c%c%c",27,'[','A');
	}
}

void movedown(){
	if(rows+1<=nlines){ rows++;
	printf("%c%c%c",27,'[','B');
	}
	if(PASTE){
//	printf("%c%c%c",27,'[','B');
	 copyN++;
	}
}

void replace(){
	char ch;
	ch=getch();
	putchar(ch);
	p[rows]->lines[cols]=ch;
}

void delete(){
	char ch;
	short i=1;
	strcpy((p[rows]->lines+cols),(p[rows]->lines+cols+1));
	p[rows]->lines[--(p[rows]->nchar)]=0;
	putchar(' ');
	putchar('\r');
	while((ch=p[rows]->lines[i++])!='\0') if(ch!='\n') putchar(ch);
//	cols++;

}
