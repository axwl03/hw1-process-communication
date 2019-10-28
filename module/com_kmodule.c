#include "com_kmodule.h"

struct sock *nlsk = NULL;
extern struct net init_net;
struct mailbox_id arr[MAX_USER];

static int __init com_kmodule_init(void)
{
    int i = 0;
    nlsk = (struct sock *)netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if(nlsk == NULL)
    {
        printk(KERN_INFO "netlink_kernel_create failed.\n");
        return -1;
    }
    for(; i < MAX_USER; ++i)
    {
        arr[i].id = 0;
        arr[i].m = NULL;
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
    return ret;
}
static void netlink_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = NULL;
    int pid, ret, id, temp;
    char *umsg = NULL, type[10], buf[256], test[1], test2 = 0;
    struct msg_data *msg;
    memset(type, 0, sizeof(type));
    memset(buf, 0, sizeof(buf));
    if(skb->len >= nlmsg_total_size(0))
    {
        nlh = nlmsg_hdr(skb);
        pid = NETLINK_CREDS(skb)->pid;
        umsg = NLMSG_DATA(nlh);
        printk(KERN_INFO "message from user: %s\n", umsg);
        if((ret = sscanf(umsg, "Registration. id=%d, type=%10s", &id, type)) == 2)
        {
            //create queue with id and type
            temp = getVacant(id);
            if(temp == -1) 			//id has been used
            {
                send_usrmsg(pid, "Fail", strlen("Fail"));
                return;
            }
            else if(temp >= 0) 		//vacant slot
            {
                arr[temp].id = id;
                if((arr[temp].m = kmalloc(sizeof(struct mailbox), GFP_KERNEL)) == NULL)
                {
                    printk("mailbox allocation failed\n");
                    send_usrmsg(pid, "Fail", strlen("Fail"));
                    return;
                }
                if(strcmp(type, "queued") == 0)
                    mailbox_init(arr[temp].m, 1);
                else
                    mailbox_init(arr[temp].m, 0);
                send_usrmsg(pid, "Success", strlen("Success"));
            }
            else					//full
            {
                printk("registration failed: mailbox arr full\n");
                send_usrmsg(pid, "Fail", strlen("Fail"));
            }
        }
        else if((ret = sscanf(umsg, "Send %d%1[ ]%255c%c", &id, test, buf, &test2)) == 3)
        {
            buf[255] = '\0';
            if((msg = kmalloc(sizeof(struct msg_data), GFP_KERNEL)) == NULL)
            {
                printk("msg_data allocation failed\n");
                send_usrmsg(pid, "Fail", strlen("Fail"));
                return;
            }
            strcpy(msg->buf, buf);
            temp = search_id(id);
            if(temp == -1)
            {
                printk("id not exist\n");
                send_usrmsg(pid, "Fail", strlen("Fail"));
            }
            else
            {
                if(mailbox_add(arr[temp].m, msg))
                    send_usrmsg(pid, "Success", strlen("Success"));
                else send_usrmsg(pid, "Fail", strlen("Fail"));
            }
        }
        else if(ret == 4) 	//data more than 255bytes
        {
            send_usrmsg(pid, "Fail", strlen("Fail"));
            printk("data more than 255bytes\n");
        }
        else if((ret = sscanf(umsg, "Recv %d", &id)) == 1)
        {
            struct msg_data msg2;
            temp = search_id(id);
            msg2 = mailbox_read(arr[temp].m);
            if(strcmp(msg2.buf, "") == 0)
                send_usrmsg(pid, "Fail", strlen("Fail"));
            else send_usrmsg(pid, msg2.buf, strlen(msg2.buf));
        }
        else if((ret = sscanf(umsg, "exit %d", &id)) == 1)
        {
            temp = search_id(id);
            mailbox_clear(arr[temp].m);
            arr[temp].id = 0;
        }
        else
        {
            printk("user message \"%s\" not recognized\n", umsg);
            send_usrmsg(pid, "Fail", strlen("Fail"));
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
    if(m->type == 1) 	//queued
    {
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
        return true;
    }
    else 				//unqueued
    {
        if(!mailbox_isEmpty(m))
            mailbox_del(m);
        m->msg_data_head = msg;
        m->msg_data_tail = msg;
        m->msg_data_count++;
        return true;
    }
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
        return true;
    }
    else
        return false;
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
        if(mailbox_isEmpty(m))
            memset(&msg, 0, sizeof(msg));
        else
        {
            msg = *(m->msg_data_head);
            msg.next = NULL;
        }
    }
    else 				/*queue*/
    {
        if(mailbox_isEmpty(m))
            memset(&msg, 0, sizeof(msg));
        else
        {
            msg = *(m->msg_data_head);
            msg.next = NULL;
            mailbox_del(m);
        }
    }
    return msg;
}
void mailbox_clear(struct mailbox *m)
{
    for(; m->msg_data_count > 0;)
        mailbox_del(m);
}
int getVacant(int id)
{
    int i = 0;
    for(; i < MAX_USER; ++i)
        if(arr[i].id == id)
            return -1;	//registered
    for(i = 0; i < MAX_USER; ++i)
        if(arr[i].id == 0)
            return i;	//vacant id
    return -2;	//full
}
int search_id(int id)
{
    int i = 0;
    for(; i < MAX_USER; ++i)
        if(arr[i].id == id)
            return i;
    return -1;	//not found
}
