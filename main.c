//#define __STDC_WANT_LIB_EXT2__  1
//#include <errno.h>
#define MAX_SIZE 255   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



float data[] = {19.29, -26.94, -49.24/*PUT UR DATA HERE*/};


int             namelist_size;
int             i;
char            * namelist[MAX_SIZE];
char            * tmp_str[255];
char            * t_end = NULL;
FILE            * ff;
FILE            * fo[MAX_SIZE];
float           * memcontainer[MAX_SIZE];
unsigned long   memcontCounter = 0;
float           tmpdat;
char            *ptr = NULL;
char            *buf;
long start, fsize, memcsize, memlim = 0;


void stringCleaner(char *input) {
	while(ptr = strpbrk(input, ","))  {*ptr = '.';    }   // , -->  . for string to float conversion
	while(ptr = strpbrk(input, "\r")) {*ptr = ' ';   }
	while(ptr = strpbrk(input, "\n")) {*ptr = '\0';   }
	while(ptr = strpbrk(input, "\\")) {*ptr = ' ';    }
	while(ptr = strpbrk(input, "/"))  {*ptr = ' ';    }
	while(ptr = strpbrk(input, "?"))  {*ptr = ' ';    }
}

void ffgets (char ** buffer, char** input){
	t_end = NULL;
    if (*input[0] != '\0') {t_end = strchr(*input, '\n');
		*buffer = *input;
		*input = t_end;
        *input[0] = '\0';
        *input = *input+1;
    }
	else{*buffer = "\0";}

}

int main(int argc, char *argv[]){
	start = clock();
	if (argc-1 == 0){
		ff = fopen("magnetometer_bt.raw","wb");
		fwrite(data, sizeof(data), 1, ff);
		fflush(ff);
		fclose(ff);	}
	else{
		namelist_size = 0;
		ff = fopen(argv[1],"rb");
		
		if (ff == NULL) {printf("NO SUCH FILE AS %s !!!\n",argv[1]); getchar(); exit(0);}
        fseek(ff,0,SEEK_END);
		fsize = ftell(ff);
		rewind(ff);
		buf = (char*)malloc(fsize);
		fread(buf,fsize,1,ff);
		fclose(ff);
		ffgets(tmp_str,&buf);			//fgets((char*)tmp_str,255, ff);
		namelist[0] = strtok((char*)*tmp_str, ";");
		namelist_size += 1;
        while(1){
			namelist[namelist_size] = strtok(NULL, ";");
			if (namelist[namelist_size] != NULL) {
				if (strlen(namelist[namelist_size])>0){
					namelist_size += 1;}
			}
			else{break;}	}
        memcsize = fsize/namelist_size;
        memlim = memcsize/sizeof(float);
		for(i=0;i<namelist_size;i++){	stringCleaner(namelist[i]); 
            memcontainer[i] = (float*)malloc(memcsize);
            fo[i] = fopen(namelist[i],"wb");
            printf("fileName: %s\n",namelist[i] );	}
		while(1){
			ffgets(tmp_str,&buf);//fgets((char*)tmp_str,255, ff);
			stringCleaner((char*)*tmp_str);
			if (strlen((char*)*tmp_str)>0)
			{
				tmpdat = atof(strtok((char*)*tmp_str, ";"));           //<==========SUPERSLOW!!!!
                memcpy(memcontainer[0]+memcontCounter,&tmpdat, sizeof(float));
                for(i=1;i<namelist_size;i++)
				{
					tmpdat = atof(strtok(NULL, ";"));                 //<==========SUPERSLOW!!!!
                    memcpy(memcontainer[i]+memcontCounter,&tmpdat, sizeof(float));
				}
				memcontCounter++;
			}else{break;}
			*tmp_str = NULL;
			if (buf == 0){break;}
		}
		for(i=0;i<namelist_size;i++){fwrite(memcontainer[i], sizeof(float)*memcontCounter, 1, fo[i]); fflush(fo[i]);	fclose(fo[i]);		}
    }
    printf("wasted time %f sec\n", (float)(clock()-start)/(float)CLOCKS_PER_SEC);

    exit(0);

}
