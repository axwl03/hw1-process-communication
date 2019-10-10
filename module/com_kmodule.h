#ifndef COM_KMODULE_H
#define COM_KMODULE_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

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
#endif  //ifndef COM_KMODULE_H
