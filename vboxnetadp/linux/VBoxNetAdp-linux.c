/* $Id: VBoxNetAdp-linux.c $ */
/** @file
 * VBoxNetAdp - Virtual Network Adapter Driver (Host), Linux Specific Code.
 */

/*
 * Copyright (C) 2009 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "the-linux-kernel.h"
#include "version-generated.h"
#include "product-generated.h"
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/miscdevice.h>

#define LOG_GROUP LOG_GROUP_NET_ADP_DRV
#include <VBox/log.h>
#include <VBox/err.h>
#include <iprt/process.h>
#include <iprt/initterm.h>
#include <iprt/mem.h>

/*
#include <iprt/assert.h>
#include <iprt/semaphore.h>
#include <iprt/spinlock.h>
#include <iprt/string.h>
#include <iprt/uuid.h>
#include <iprt/alloca.h>
*/

#define VBOXNETADP_OS_SPECFIC 1
#include "../VBoxNetAdpInternal.h"

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define VBOXNETADP_LINUX_NAME      "vboxnet%d"
#define VBOXNETADP_CTL_DEV_NAME    "vboxnetctl"

#define VBOXNETADP_FROM_IFACE(iface) ((PVBOXNETADP) ifnet_softc(iface))

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int  VBoxNetAdpLinuxInit(void);
static void VBoxNetAdpLinuxUnload(void);

static int VBoxNetAdpLinuxOpen(struct inode *pInode, struct file *pFilp);
static int VBoxNetAdpLinuxClose(struct inode *pInode, struct file *pFilp);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int VBoxNetAdpLinuxIOCtl(struct inode *pInode, struct file *pFilp,
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
static long VBoxNetAdpLinuxIOCtlUnlocked(struct file *pFilp,
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
                                unsigned int uCmd, unsigned long ulArg);

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
module_init(VBoxNetAdpLinuxInit);
module_exit(VBoxNetAdpLinuxUnload);

MODULE_AUTHOR(VBOX_VENDOR);
MODULE_DESCRIPTION(VBOX_PRODUCT " Network Adapter Driver");
MODULE_LICENSE("GPL");
#ifdef MODULE_VERSION
MODULE_VERSION(VBOX_VERSION_STRING " (" RT_XSTR(INTNETTRUNKIFPORT_VERSION) ")");
#endif

/**
 * The (common) global data.
 */
static struct file_operations gFileOpsVBoxNetAdp =
{
    owner:      THIS_MODULE,
    open:       VBoxNetAdpLinuxOpen,
    release:    VBoxNetAdpLinuxClose,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    ioctl:      VBoxNetAdpLinuxIOCtl,
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
    unlocked_ioctl: VBoxNetAdpLinuxIOCtlUnlocked,
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
};

/** The miscdevice structure. */
static struct miscdevice g_CtlDev =
{
    minor:      MISC_DYNAMIC_MINOR,
    name:       VBOXNETADP_CTL_DEV_NAME,
    fops:       &gFileOpsVBoxNetAdp,
# if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 17)
    devfs_name: VBOXNETADP_CTL_DEV_NAME
# endif
};

struct VBoxNetAdpPriv
{
    struct net_device_stats Stats;
};

typedef struct VBoxNetAdpPriv VBOXNETADPPRIV;
typedef VBOXNETADPPRIV *PVBOXNETADPPRIV;

static int vboxNetAdpLinuxOpen(struct net_device *pNetDev)
{
    netif_start_queue(pNetDev);
    return 0;
}

static int vboxNetAdpLinuxStop(struct net_device *pNetDev)
{
    netif_stop_queue(pNetDev);
    return 0;
}

static int vboxNetAdpLinuxXmit(struct sk_buff *pSkb, struct net_device *pNetDev)
{
    PVBOXNETADPPRIV pPriv = netdev_priv(pNetDev);

    /* Update the stats. */
    pPriv->Stats.tx_packets++;
    pPriv->Stats.tx_bytes += pSkb->len;
    /* Update transmission time stamp. */
    pNetDev->trans_start = jiffies;
    /* Nothing else to do, just free the sk_buff. */
    dev_kfree_skb(pSkb);
    return 0;
}

struct net_device_stats *vboxNetAdpLinuxGetStats(struct net_device *pNetDev)
{
    PVBOXNETADPPRIV pPriv = netdev_priv(pNetDev);
    return &pPriv->Stats;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)
static const struct net_device_ops vboxNetAdpNetdevOps = {
    .ndo_open               = vboxNetAdpLinuxOpen,
    .ndo_stop               = vboxNetAdpLinuxStop,
    .ndo_start_xmit         = vboxNetAdpLinuxXmit,
    .ndo_get_stats          = vboxNetAdpLinuxGetStats
};
#endif

static void vboxNetAdpNetDevInit(struct net_device *pNetDev)
{
    PVBOXNETADPPRIV pPriv;

    ether_setup(pNetDev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29)
    pNetDev->netdev_ops = &vboxNetAdpNetdevOps;
#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29) */
    pNetDev->open = vboxNetAdpLinuxOpen;
    pNetDev->stop = vboxNetAdpLinuxStop;
    pNetDev->hard_start_xmit = vboxNetAdpLinuxXmit;
    pNetDev->get_stats = vboxNetAdpLinuxGetStats;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29) */

    pPriv = netdev_priv(pNetDev);
    memset(pPriv, 0, sizeof(*pPriv));
}


int vboxNetAdpOsCreate(PVBOXNETADP pThis, PCRTMAC pMACAddress)
{
    int rc = VINF_SUCCESS;
    struct net_device *pNetDev;

    /* No need for private data. */
    pNetDev = alloc_netdev(sizeof(VBOXNETADPPRIV), VBOXNETADP_LINUX_NAME, vboxNetAdpNetDevInit);
    if (pNetDev)
    {
        int err;

        memcpy(pNetDev->dev_addr, pMACAddress, ETH_ALEN);
        Log2(("vboxNetAdpOsCreate: pNetDev->dev_addr = %.6Rhxd\n", pNetDev->dev_addr));
        err = register_netdev(pNetDev);
        if (!err)
        {
            strncpy(pThis->szName, pNetDev->name, VBOXNETADP_MAX_NAME_LEN);
            pThis->u.s.pNetDev = pNetDev;
            Log2(("vboxNetAdpOsCreate: pThis=%p pThis->szName = %p\n", pThis, pThis->szName));
            return VINF_SUCCESS;
        }
        free_netdev(pNetDev);
        rc = RTErrConvertFromErrno(err);
    }
    return rc;
}

void vboxNetAdpOsDestroy(PVBOXNETADP pThis)
{
    struct net_device *pNetDev = pThis->u.s.pNetDev;
    AssertPtr(pThis->u.s.pNetDev);

    pThis->u.s.pNetDev = NULL;
    unregister_netdev(pNetDev);
    free_netdev(pNetDev);
}

/**
 * Device open. Called on open /dev/vboxnetctl
 *
 * @param   pInode      Pointer to inode info structure.
 * @param   pFilp       Associated file pointer.
 */
static int VBoxNetAdpLinuxOpen(struct inode *pInode, struct file *pFilp)
{
    Log(("VBoxNetAdpLinuxOpen: pid=%d/%d %s\n", RTProcSelf(), current->pid, current->comm));

    /*
     * Only root is allowed to access the device, enforce it!
     */
    if (!capable(CAP_SYS_ADMIN))
    {
        Log(("VBoxNetAdpLinuxOpen: admin privileges required!\n"));
        return -EPERM;
    }

    return 0;
}


/**
 * Close device.
 *
 * @param   pInode      Pointer to inode info structure.
 * @param   pFilp       Associated file pointer.
 */
static int VBoxNetAdpLinuxClose(struct inode *pInode, struct file *pFilp)
{
    Log(("VBoxNetAdpLinuxClose: pid=%d/%d %s\n",
         RTProcSelf(), current->pid, current->comm));
    pFilp->private_data = NULL;
    return 0;
}

/**
 * Device I/O Control entry point.
 *
 * @param   pFilp       Associated file pointer.
 * @param   uCmd        The function specified to ioctl().
 * @param   ulArg       The argument specified to ioctl().
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int VBoxNetAdpLinuxIOCtl(struct inode *pInode, struct file *pFilp,
#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
static long VBoxNetAdpLinuxIOCtlUnlocked(struct file *pFilp,
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36) */
                                unsigned int uCmd, unsigned long ulArg)
{
    VBOXNETADPREQ Req;
    PVBOXNETADP pAdp;
    int rc;

    Log(("VBoxNetAdpLinuxIOCtl: param len %#x; uCmd=%#x; add=%#x\n", _IOC_SIZE(uCmd), uCmd, VBOXNETADP_CTL_ADD));
    if (RT_UNLIKELY(_IOC_SIZE(uCmd) != sizeof(Req))) /* paraonia */
    {
        Log(("VBoxNetAdpLinuxIOCtl: bad ioctl sizeof(Req)=%#x _IOC_SIZE=%#x; uCmd=%#x.\n", sizeof(Req), _IOC_SIZE(uCmd), uCmd));
        return -EINVAL;
    }

    switch (uCmd)
    {
        case VBOXNETADP_CTL_ADD:
            Log(("VBoxNetAdpLinuxIOCtl: _IOC_DIR(uCmd)=%#x; IOC_OUT=%#x\n", _IOC_DIR(uCmd), IOC_OUT));
            rc = vboxNetAdpCreate(&pAdp);
            if (RT_FAILURE(rc))
            {
                Log(("VBoxNetAdpLinuxIOCtl: vboxNetAdpCreate -> %Rrc\n", rc));
                return -EINVAL;
            }

            Assert(strlen(pAdp->szName) < sizeof(Req.szName));
            strncpy(Req.szName, pAdp->szName, sizeof(Req.szName) - 1);
            Req.szName[sizeof(Req.szName) - 1] = '\0';

            if (RT_UNLIKELY(copy_to_user((void *)ulArg, &Req, sizeof(Req))))
            {
                /* this is really bad! */
                /** @todo remove the adapter again? */
                printk(KERN_ERR "VBoxNetAdpLinuxIOCtl: copy_to_user(%#lx,,%#zx); uCmd=%#x!\n", ulArg, sizeof(Req), uCmd);
                return -EFAULT;
            }
            Log(("VBoxNetAdpLinuxIOCtl: Successfully added '%s'\n", Req.szName));
            break;

        case VBOXNETADP_CTL_REMOVE:
            if (RT_UNLIKELY(copy_from_user(&Req, (void *)ulArg, sizeof(Req))))
            {
                Log(("VBoxNetAdpLinuxIOCtl: copy_from_user(,%#lx,) failed; uCmd=%#x.\n", ulArg, uCmd));
                return -EFAULT;
            }
            Log(("VBoxNetAdpLinuxIOCtl: Remove %s\n", Req.szName));

            pAdp = vboxNetAdpFindByName(Req.szName);
            if (!pAdp)
            {
                Log(("VBoxNetAdpLinuxIOCtl: '%s' not found\n", Req.szName));
                return -EINVAL;
            }

            rc = vboxNetAdpDestroy(pAdp);
            if (RT_FAILURE(rc))
            {
                Log(("VBoxNetAdpLinuxIOCtl: vboxNetAdpDestroy('%s') -> %Rrc\n", Req.szName, rc));
                return -EINVAL;
            }
            Log(("VBoxNetAdpLinuxIOCtl: Successfully removed '%s'\n", Req.szName));
            break;

        default:
            printk(KERN_ERR "VBoxNetAdpLinuxIOCtl: unknown command %x.\n", uCmd);
            return -EINVAL;
    }

    return 0;
}

int  vboxNetAdpOsInit(PVBOXNETADP pThis)
{
    /*
     * Init linux-specific members.
     */
    pThis->u.s.pNetDev = NULL;

    return VINF_SUCCESS;
}



/**
 * Initialize module.
 *
 * @returns appropriate status code.
 */
static int __init VBoxNetAdpLinuxInit(void)
{
    int rc;
    /*
     * Initialize IPRT.
     */
    rc = RTR0Init(0);
    if (RT_SUCCESS(rc))
    {
        Log(("VBoxNetAdpLinuxInit\n"));

        rc = vboxNetAdpInit();
        if (RT_SUCCESS(rc))
        {
            rc = misc_register(&g_CtlDev);
            if (rc)
            {
                printk(KERN_ERR "VBoxNetAdp: Can't register " VBOXNETADP_CTL_DEV_NAME " device! rc=%d\n", rc);
                return rc;
            }
            LogRel(("VBoxNetAdp: Successfully started.\n"));
            return 0;
        }
        else
            LogRel(("VBoxNetAdp: failed to register vboxnet0 device (rc=%d)\n", rc));
    }
    else
        LogRel(("VBoxNetFlt: failed to initialize IPRT (rc=%d)\n", rc));

    return -RTErrConvertToErrno(rc);
}


/**
 * Unload the module.
 *
 * @todo We have to prevent this if we're busy!
 */
static void __exit VBoxNetAdpLinuxUnload(void)
{
    int rc;
    Log(("VBoxNetFltLinuxUnload\n"));

    /*
     * Undo the work done during start (in reverse order).
     */

    vboxNetAdpShutdown();
    /* Remove control device */
    rc = misc_deregister(&g_CtlDev);
    if (rc < 0)
    {
        printk(KERN_ERR "misc_deregister failed with rc=%x\n", rc);
    }

    RTR0Term();

    Log(("VBoxNetFltLinuxUnload - done\n"));
}
