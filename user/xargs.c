#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    
	argc--;
    argv++;
  
  	char xargv[MAXARG][MAXARG];
  	char *pass[MAXARG];
  	for (int i = 0; i < MAXARG; i++) pass[i] = xargv[i];
    int xargc = argc;
  	for (int i = 0; i < argc; i++) strcpy(xargv[i], argv[i]);

  	char buf[512];int index = 0;
  	while (read(0, buf+index, sizeof(char)) == sizeof(char)) {          
      	if (*(buf+index) == '\n') {
            *(buf+index) = '\0';
        	strcpy(xargv[xargc++], buf);
           	pass[xargc+1] = 0;
            if (fork()) wait();
    		else exec(pass[0], pass);
                
            pass[xargc+1] = xargv[xargc+1];
            xargc = argc;
            index = 0;
  		}else if(*(buf+index) == ' ') {
            *(buf+index) = '\0';
           	strcpy(xargv[xargc++], buf);
            index = 0;
        }else {
            index++;
        }
    }
  	exit();
}
