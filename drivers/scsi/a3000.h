#ifndef A3000_H

/* $Id: a3000.h,v 1.2 1996/02/29 22:10:29 root Exp root $
 *
 * Header file for the Amiga 3000 built-in SCSI controller for Linux
 *
 * Written and (C) 1993, Hamish Macdonald, see a3000.c for more info
 *
 */

#include <linux/types.h>

int a3000_detect(Scsi_Host_Template *);
const char *wd33c93_info(void);
int wd33c93_queuecommand(Scsi_Cmnd *, void (*done)(Scsi_Cmnd *));
int wd33c93_abort(Scsi_Cmnd *);
int wd33c93_reset(Scsi_Cmnd *);

#ifndef NULL
#define NULL 0
#endif

#ifndef CMD_PER_LUN
#define CMD_PER_LUN 2
#endif

#ifndef CAN_QUEUE
#define CAN_QUEUE 16
#endif

#ifdef HOSTS_C

extern struct proc_dir_entry proc_scsi_a3000;

#define A3000_SCSI {  /* next */                NULL,            \
		      /* usage_count */         NULL,	         \
		      /* proc_dir_entry */      &proc_scsi_a3000, \
		      /* proc_info */           NULL,            \
		      /* name */                "Amiga 3000 built-in SCSI", \
		      /* detect */              a3000_detect,    \
		      /* release */             NULL,            \
		      /* info */                NULL,	         \
		      /* command */             NULL,            \
		      /* queuecommand */        wd33c93_queuecommand, \
		      /* abort */               wd33c93_abort,   \
		      /* reset */               wd33c93_reset,   \
		      /* slave_attach */        NULL,            \
		      /* bios_param */          NULL, 	         \
		      /* can_queue */           CAN_QUEUE,       \
		      /* this_id */             7,               \
		      /* sg_tablesize */        SG_ALL,          \
		      /* cmd_per_lun */	        CMD_PER_LUN,     \
		      /* present */             0,               \
		      /* unchecked_isa_dma */   0,               \
		      /* use_clustering */      DISABLE_CLUSTERING }
#else

/*
 * if the transfer address ANDed with this results in a non-zero
 * result, then we can't use DMA.
 */ 
#define A3000_XFER_MASK  (0x00000003)

typedef struct {
             unsigned char      pad1[2];
    volatile unsigned short     DAWR;
    volatile unsigned int       WTC;
             unsigned char      pad2[2];
    volatile unsigned short     CNTR;
    volatile unsigned long      ACR;
             unsigned char      pad3[2];
    volatile unsigned short     ST_DMA;
             unsigned char      pad4[2];
    volatile unsigned short     FLUSH;
             unsigned char      pad5[2];
    volatile unsigned short     CINT;
             unsigned char      pad6[2];
    volatile unsigned short     ISTR;
	     unsigned char      pad7[30];
    volatile unsigned short     SP_DMA;
             unsigned char      pad8;
    volatile unsigned char      SASR;
             unsigned char      pad9;
    volatile unsigned char      SCMD;
} a3000_scsiregs;

#define DAWR_A3000		(3)

/* CNTR bits. */
#define CNTR_TCEN		(1<<5)
#define CNTR_PREST		(1<<4)
#define CNTR_PDMD		(1<<3)
#define CNTR_INTEN		(1<<2)
#define CNTR_DDIR		(1<<1)
#define CNTR_IO_DX		(1<<0)

/* ISTR bits. */
#define ISTR_INTX		(1<<8)
#define ISTR_INT_F		(1<<7)
#define ISTR_INTS		(1<<6)
#define ISTR_E_INT		(1<<5)
#define ISTR_INT_P		(1<<4)
#define ISTR_UE_INT		(1<<3)
#define ISTR_OE_INT		(1<<2)
#define ISTR_FF_FLG		(1<<1)
#define ISTR_FE_FLG		(1<<0)

#endif /* else def HOSTS_C */

#endif /* A3000_H */
