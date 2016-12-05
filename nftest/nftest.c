#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

static struct nf_hook_ops nfho;         //struct holding set of hook function options

//function to be called by hook
unsigned int hook_func(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
        printk(KERN_INFO "[msx] packet accepted\n");                        
        return NF_ACCEPT; 
}

//Called when module loaded using 'insmod'
int init_module()
{
        //function to call when conditions below met
        nfho.hook = hook_func;        
        //called right after packet recieved, first hook in Netfilter
        //nfho.hooknum = NF_IP_PRE_ROUTING;  
        nfho.hooknum = 0;  
        //IPV4 packets
        nfho.pf = PF_INET;   
        //set to highest priority over all other hook functions
        nfho.priority = NF_IP_PRI_FIRST; 
        //register hook
        nf_register_hook(&nfho);  
        
        return 0; 
}

//Called when module unloaded using 'rmmod'
void cleanup_module()
{
        nf_unregister_hook(&nfho);   
}