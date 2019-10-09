#include "com_kmodule.h"

static int __init com_kmodule_init(void)
{
    printk(KERN_INFO "Enter module. Hello world!\n");
    return 0;
}

static void __exit com_kmodule_exit(void)
{
    printk(KERN_INFO "Exit module. Bye~\n");
}

module_init(com_kmodule_init);
module_exit(com_kmodule_exit);

void mailbox_init(struct mailbox *m, unsigned char type)
{
    m->type = type;
    m->msg_data_count = 0;
    m->msg_data_head = NULL;
    m->msg_data_tail = NULL;
}
void mailbox_add(struct mailbox *m, struct msg_data *msg)
{
    msg->next = NULL;
    if(mailbox_isEmpty(m))
    {
        m->msg_data_head = msg;
        m->msg_data_tail = msg;
    }
    else
    {
        m->msg_data_tail->next = msg;
        m->msg_data_tail = msg;
    }
    m->msg_data_count++;
    printk(KERN_INFO "size %d: add success.\n", m->msg_data_count);
}
bool mailbox_del(struct mailbox *m)
{
    struct msg_data *temp;
    if(!mailbox_isEmpty(m))
    {
        temp = m->msg_data_head;
        m->msg_data_head = temp->next;
        kfree(temp);
        m->msg_data_count--;
        printk(KERN_INFO "size %d: remove success.\n", m->msg_data_count);
        return true;
    }
    else
    {
        printk(KERN_INFO "size 0: mailbox empty.\n");
        return false;
    }
}
void mailbox_print(struct mailbox *m)
{
    struct msg_data *temp = m->msg_data_head;
    int index = 1;
    printk(KERN_INFO "print mailbox:\n");
    for(; temp != NULL; temp = temp->next)
        printk(KERN_INFO "index: %d. content: %s", index++, temp->buf);
    printk(KERN_INFO "\n");
}
bool mailbox_isEmpty(struct mailbox *m)
{
    if(m->msg_data_count == 0)
        return true;
    else
        return false;
}
bool mailbox_isFull(struct mailbox *m)
{
    if(m->msg_data_count < 3)
        return false;
    else
        return true;
}
