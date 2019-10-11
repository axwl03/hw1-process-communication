#include "com_app.h"

int main(int argc, char *argv[])
{
    char umsg[MAX_LEN];
    struct user_msg_info u_info;

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
    saddr.nl_pid = atoi(argv[1]);
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

    //registration
    send_kernel(umsg);
    recv_kernel(&u_info);

    while(0)
    {
        //send
        //send ack from kernel

        //recv
        //recv data or "Fail"
    }
    close(skfd);
    free(nlh);
    return 0;
}
void send_kernel(char *msg)
{
    memcpy(NLMSG_DATA(nlh), msg, MAX_LEN);
    int ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("error sending");
        close(skfd);
        exit(1);
    }
    printf("send kernel: %s\n", msg);
}
void recv_kernel(struct user_msg_info *u_info_p)
{
    memset(u_info_p, 0, sizeof(struct user_msg_info));
    int ret = recvfrom(skfd, u_info_p, sizeof(struct user_msg_info), 0, (struct sockaddr *)&daddr, &len);
    if(!ret)
    {
        perror("error recv from kernel\n");
        close(skfd);
        exit(1);
    }
    printf("recv kernel: %s\n", u_info_p->buf);
}
