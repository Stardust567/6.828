#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXARG       32  // max exec arguments

// replace the left side "|" with "\0"
// return the rest of the string or NULL
char *simple_tok(char *p, char d)
{
    while (*p != '\0' && *p != d)  p++;
    if (*p == '\0') return 0;
    *p = '\0';
    return p + 1;
}

// trim spaces on both side
char *trim(char *c)
{
    char *e = c;
    while (*e)  e++; // point to the end
    while (*c == ' ')  *(c++) = '\0'; // delete spaces on left side
    while (*(--e) == ' '); // skip spaces on right side
    *(e + 1) = '\0';
    return c;
}

void redirect(int k, int pd[])
{
    close(k);
    dup(pd[k]);
    close(pd[0]);
    close(pd[1]);
}

void handle(char *cmd)
{
    cmd = trim(cmd);
    
	char buf[MAXARG][MAXARG]; char *pass[MAXARG];
    for (int i = 0; i < MAXARG; i++) pass[i] = buf[i];

    int argc = 0; char *c = buf[argc];
    int input_pos = 0, output_pos = 0;
    for (char *p = cmd; *p; p++) 
    {
        if (*p == ' ')
        {
            *c = '\0';
            argc++;
            c = buf[argc];
        }
        else {
            if(*p == '<')  input_pos = argc + 1;
            if(*p == '>')  output_pos = argc + 1;
            *c++ = *p;
        }
    }
    // gets() returns no '\n'
    *c = '\0';
    argc++;
    pass[argc] = 0;

    if(input_pos) {
        close(0);
        open(pass[input_pos], O_RDONLY);
    }

    if(output_pos) {
        close(1);
        open(pass[output_pos], O_WRONLY | O_CREATE);
    }

    char *pass2[32]; int argc2 = 0;
    for(int pos = 0; pos < argc; pos++) {
        if(pos == input_pos - 1) pos += 2;
        if(pos == output_pos - 1) pos += 2;
        pass2[argc2++] = pass[pos];
    }
    pass2[argc2] = 0;

    if (fork())
    {
        wait(0);
    }
    else
    {
        exec(pass2[0], pass2);
    }
}

void handle_cmd(char *a, char *n)
{
    if (a)
    {
        int pd[2];
        pipe(pd);

        if(!fork()){
            if(n) redirect(1, pd);
            handle(a);
        } else if(!fork()) {
            if(n) {
                redirect(0, pd);
                a = n;
                n = simple_tok(a, '|');
                handle_cmd(a, n);
            }
        }

        close(pd[0]);
        close(pd[1]);
        wait(0);
        wait(0);
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    
	char cmd_buf[1024];
    while (1)
    {
        fprintf(1, "@ ");
        memset(cmd_buf, 0, 1024);
        gets(cmd_buf, 1024);

        if (cmd_buf[0] == 0) exit(0);

        *strchr(cmd_buf, '\n') = '\0';

        if (fork())
        {
            wait(0);
        }
        else
        {
            char *a = cmd_buf;
            char *n = simple_tok(a, '|');
            handle_cmd(a, n);
        }
    }

    exit(0);
}
