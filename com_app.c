#include "com_app.h"

int main(int argc, char *argv[])
{
    char umsg[MAX_LEN];

    if(argc != 3)
    {
        printf("The format should be: ./com_app [id] [type]\n");
        exit(1);
    }
    if(atoi(argv[1]) < 1 || atoi(argv[1]) > 1000)
    {
        printf("id should be 1~1000.\n");
        exit(1);
    }
    if(strcmp(argv[2], "queued") == 0) 	//queued application
        snprintf(umsg, MAX_LEN, "Registration. id=%s, type=queued", argv[1]);
    else if(strcmp(argv[2], "unqueued") == 0) 	//unqueued application
        snprintf(umsg, MAX_LEN, "Registration. id=%s, type=unqueued", argv[1]);
    else
    {
        printf("type should be \"queued\" or \"unqueued\".\n");
        exit(1);
    }
    //setting socket parameters
    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(skfd == -1)
    {
        perror("error creating socket");
        exit(1);
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK;
    saddr.nl_pid = getpid();
    saddr.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("error binding socket");
        close(skfd);
        exit(1);
    }
    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0;
    daddr.nl_groups = 0;

    nlh = malloc(NLMSG_SPACE(MAX_LEN));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_LEN);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid;
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    //registration
    send_kernel(umsg);
    recv_kernel();
    if(strcmp("Success", (char *)NLMSG_DATA(nlh)) == 0)
        own_id = atoi(argv[1]);

    char test_str[10];
    int ret = 0;
    while(1)
    {
        readline(umsg);
        if((ret = sscanf(umsg, "Send %*d %10s", test_str)))
        {
            send_kernel(umsg);
            recv_kernel();
        }
        else if(strcmp(umsg, "exit") == 0)
            break;

        //recv
        //recv data or "Fail"
    }
    close(skfd);
    free(nlh);
    return 0;
}
void send_kernel(char *m)
{
    memcpy(NLMSG_DATA(nlh), m, MAX_LEN);
    printf("send kernel: %s\n", m);
    sendmsg(skfd, &msg, 0);
}
void recv_kernel()
{
    memset(NLMSG_DATA(nlh), 0, MAX_LEN);
    printf("recv kernel:\n");
    recvmsg(skfd, &msg, 0);
    printf("%s\n", (char *)NLMSG_DATA(nlh));
}
void readline(char *str)
{
    char ch;
    for(int i = 0; i < 300; ++i)
    {
        ch = getchar();
        if(ch == '\n')
        {
            str[i] = '\0';
            return;
        }
        str[i] = ch;
    }
}
