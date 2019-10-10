#ifndef COM_APP_H
#define COM_APP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#define NETLINK_USER 30
#define MSG_LEN 256
#define MAX_PLOAD 256

struct user_msg_info
{
    struct nlmsghdr hdr;
    char buf[MSG_LEN];
};

#endif  //ifndef COM_APP_H
