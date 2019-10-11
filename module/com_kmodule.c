#include "com_kmodule.h"

struct sock *nlsk = NULL;
extern struct net init_net;

static int __init com_kmodule_init(void)
{
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if(nlsk == NULL)
    {
        printk(KERN_INFO "netlink_kernel_create failed.\n");
        return -1;
    }
    printk("test_netlink_init\n");
    return 0;
}

static void __exit com_kmodule_exit(void)
{
    printk(KERN_INFO "Exit module. Bye~\n");
    if(nlsk)
    {
        netlink_kernel_release(nlsk);
        nlsk = NULL;
    }
}

module_init(com_kmodule_init);
module_exit(com_kmodule_exit);

int send_usrmsg(int pid, char *buf, int len)
{
    struct sk_buff *nl_skb;
    struct nlmsghdr *nlh;
    int ret;

    nl_skb = nlmsg_new(len, GFP_ATOMIC);
    if(!nl_skb)
    {
        printk(KERN_INFO "netlink allocation failed.\n");
        return -1;
    }

    nlh = nlmsg_put(nl_skb, 0, 0, NETLINK_USER, len, 0);
    if(nlh == NULL)
    {
        printk(KERN_INFO "nlmsg_put failed.\n");
        nlmsg_free(nl_skb);
        return -1;
    }

    memcpy(nlmsg_data(nlh), buf, len);
    ret = netlink_unicast(nlsk, nl_skb, pid, MSG_DONTWAIT);
    printk(KERN_INFO "ret: %d pid: %d buf: %s\n", ret, pid, buf);
    return ret;
}
static void netlink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    int pid;
    char *umsg = NULL;
    char *kmsg = "Hello user!!\n";
    if(skb->len >= nlmsg_total_size(0))
    {
        //    nlh = nlmsg_hdr(skb);
        nlh = (struct nlmsghdr *)skb->data;
        pid = NETLINK_CREDS(skb)->pid;
        printk(KERN_INFO "pid: %d\n", pid);
        umsg = NLMSG_DATA(nlh);
        if(umsg)
        {
            printk(KERN_INFO "kernel recv from user: %s\n", umsg);
            send_usrmsg(pid, kmsg, strlen(kmsg));
        }
    }
}
void mailbox_init(struct mailbox *m, unsigned char type)
{
    m->type = type;
    m->msg_data_count = 0;
    m->msg_data_head = NULL;
    m->msg_data_tail = NULL;
}
bool mailbox_add(struct mailbox *m, struct msg_data *msg)
{
    msg->next = NULL;
    if(mailbox_isEmpty(m))
    {
        m->msg_data_head = msg;
        m->msg_data_tail = msg;
    }
    else if(mailbox_isFull(m))
    {
        printk(KERN_INFO "size %d: add fail.\n", m->msg_data_count);
        return false;
    }
    else
    {
        m->msg_data_tail->next = msg;
        m->msg_data_tail = msg;
    }
    m->msg_data_count++;
    printk(KERN_INFO "size %d: add success.\n", m->msg_data_count);
    return true;
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
struct msg_data mailbox_read(struct mailbox *m)
{
    struct msg_data msg;
    if(m->type == 0) 	/*unqueue*/
    {
        msg = *(m->msg_data_head);
        msg.next = NULL;
        if(!mailbox_isEmpty(m))
            mailbox_del(m);
    }
    else 				/*queue*/
    {
        if(mailbox_isEmpty(m))
        {
            memset(&msg, 0, sizeof(msg));
        }
        else
        {
            msg = *(m->msg_data_head);
            msg.next = NULL;
            mailbox_del(m);
        }
    }
    return msg;
}
bool mailbox_write(struct mailbox *m, struct msg_data msg)
{
    struct msg_data *p = kmalloc(sizeof(msg), GFP_KERNEL);
    *p = msg;
    return mailbox_add(m, p);
}
