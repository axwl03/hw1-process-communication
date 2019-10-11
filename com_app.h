#ifndef COM_APP_H
#define COM_APP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#define NETLINK_USER 30
#define MAX_LEN 256

struct user_msg_info
{
    struct nlmsghdr hdr;
    char buf[MAX_LEN];
};

int skfd;
socklen_t len = sizeof(struct sockaddr_nl);
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl saddr, daddr;

void send_kernel(char *msg);
void recv_kernel(struct user_msg_info *u_info_p);

#endif  //ifndef COM_APP_H
