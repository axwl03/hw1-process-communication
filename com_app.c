#include "com_app.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("The format should be: ./com_app [id] [type]\n");
        exit(1);
    }
    if(strcmp(argv[2], "queued") == 0) 	/*queued application*/
    {
    }
    else if(strcmp(argv[2], "unqueued") == 0) 	/*unqueued application*/
    {
    }
    else
    {
        printf("The type should be \"queued\" or \"unqueued\".\n");
        exit(1);
    }
}
