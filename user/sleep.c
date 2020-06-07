#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
    if(argc != 2) write(2, "Error message", strlen("Error message"));
    int time = atoi(argv[1]);
    sleep(time);
    exit();
}
