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
#define MAX_LEN 300

int skfd, own_id;
socklen_t len = sizeof(struct sockaddr_nl);
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl saddr, daddr;
struct iovec iov;
struct msghdr msg;

void send_kernel(char *m);
void recv_kernel();
void readline(char *str);

#endif  //ifndef COM_APP_H
