#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h> 

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/ip.h>

MODULE_LICENSE("GPL");

static struct nf_hook_ops nfho; // struct holding set of hook function options
static __be32 hb_sender = 0x0b00000a; // ip: 10.0.0.11

/*hook function*/
unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, 
        const struct net_device *in, const struct net_device *out, 
        int (*okfn)(struct sk_buff *))
{
        const struct iphdr *iph; 
        iph = ip_hdr(skb);
        if(hb_sender == iph->saddr) { // check if ip comes from the one we are waiting for.
                printk(KERN_DEBUG "[msx] heartbeat comes from %pI4\n", &iph->saddr);
        }
        return NF_ACCEPT; 
}

int init_module()
{
        nfho.hook = hook_func;        
        nfho.hooknum = 0; // nfho.hooknum = NF_IP_POST_ROUTING;  
        nfho.pf = PF_INET; // IPV4 packets
        nfho.priority = NF_IP_PRI_FIRST; // set to highest priority over all other hook functions
        nf_register_hook(&nfho);  
        return 0; 
}

void cleanup_module()
{
        nf_unregister_hook(&nfho);   
}
