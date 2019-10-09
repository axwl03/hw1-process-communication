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

void mailbox_init(struct mailbox *m)
{
    //not setting queued or unqueued
    m->msg_data_count = 0;
    m->msg_data_head = NULL;
    m->msg_data_tail = NULL;
}
void mailbox_add(struct mailbox *m, struct msg_data *msg)
{
    msg->next = NULL;
    if(m->msg_data_count == 0)
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
    printk("size %d: add success.\n", m->msg_data_count);
}
bool mailbox_del(struct mailbox *m)
{
    struct msg_data *temp;
    if(m->msg_data_count != 0)
    {
        temp = m->msg_data_head;
        m->msg_data_head = temp->next;
        kfree(temp);
        m->msg_data_count--;
        printk("size %d: remove success.\n", m->msg_data_count);
        return true;
    }
    else
    {
        printk("size 0: queue empty.\n");
        return false;
    }
}
void mailbox_print(struct mailbox *m)
{
    struct msg_data *temp = m->msg_data_head;
    int index = 1;
    printk("print mailbox:\n");
    for(; temp != NULL; temp = temp->next)
        printk("index: %d. content: %s", index++, temp->buf);
    printk("\n");
}
