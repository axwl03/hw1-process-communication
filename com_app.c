#include "com_app.h"

int main(int argc, char *argv[])
{
    int skfd;
    int ret;
    struct user_msg_info u_info;
    socklen_t len;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl saddr, daddr;
    char *umsg = "hello netlink!!\n";

    skfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if(skfd == -1)
    {
        perror("error creating socket\n");
        exit(1);
    }
    memset(&saddr, 0, sizeof(saddr));
    saddr.nl_family = AF_NETLINK;
    saddr.nl_pid = 100; //port
    saddr.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        perror("error binding socket\n");
        close(skfd);
        exit(1);
    }
    memset(&daddr, 0, sizeof(daddr));
    daddr.nl_family = AF_NETLINK;
    daddr.nl_pid = 0;
    daddr.nl_groups = 0;

    nlh = malloc(NLMSG_SPACE(MAX_PLOAD));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = saddr.nl_pid;
    memcpy(NLMSG_DATA(nlh), umsg, strlen(umsg));
    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&daddr, sizeof(struct sockaddr_nl));
    if(!ret)
    {
        perror("error sending\n");
        close(skfd);
        exit(1);
    }
    printf("send kernel: %s", umsg);

    memset(&u_info, 0, sizeof(u_info));
    len = sizeof(struct sockaddr_nl);
    ret = recvfrom(skfd, &u_info, sizeof(struct user_msg_info), 0, (struct sockaddr *)&daddr, &len);
    if(!ret)
    {
        perror("error recv from kernel\n");
        close(skfd);
        exit(1);
    }
    printf("recv kernel: %s", u_info.buf);
    close(skfd);
    free(nlh);
    return 0;

    /*    if(argc != 3)
        {
            printf("The format should be: ./com_app [id] [type]\n");
            exit(1);
        }
        if(strcmp(argv[2], "queued") == 0) 	//queued application
        {
        }
        else if(strcmp(argv[2], "unqueued") == 0) 	//unqueued application
        {
        }
        else
        {
            printf("The type should be \"queued\" or \"unqueued\".\n");
            exit(1);
        }*/
}
