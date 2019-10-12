#ifndef COM_KMODULE_H
#define COM_KMODULE_H

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/slab.h>
#define NETLINK_USER 30
#define MAX_USER 10

struct mailbox
{
    //0: unqueued
    //1: queued
    unsigned char type;
    unsigned char msg_data_count;
    struct msg_data *msg_data_head;
    struct msg_data *msg_data_tail;
};

struct msg_data
{
    char buf[256];
    struct msg_data *next;
};

struct mailbox_id
{
    int id;
    struct mailbox *m;
};

int send_usrmsg(int pid, char *buf, int len);
static void netlink_recv_msg(struct sk_buff *skb);

struct netlink_kernel_cfg cfg =
{
    .input = netlink_recv_msg,
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Apple pie");
MODULE_DESCRIPTION("A Simple Hello World module");

void mailbox_init(struct mailbox *m, unsigned char type);
bool mailbox_add(struct mailbox *m, struct msg_data *msg);	/*insert msg_data at the rear of the mailbox*/
bool mailbox_del(struct mailbox *m);	/*remove msg_data at the front of the mailbox*/
void mailbox_print(struct mailbox *m);
bool mailbox_isEmpty(struct mailbox *m);	/*check if mailbox is empty*/
bool mailbox_isFull(struct mailbox *m);	/*check if mailbox is full*/
struct msg_data mailbox_read(struct mailbox *m);	/*read mailbox msg_data*/
bool mailbox_write(struct mailbox *m, struct msg_data msg);	/*write msg_data to mailbox*/
int getVacant(int id);	/*return vacant slot*/
int search_id(int id);
#endif  //ifndef COM_KMODULE_H
