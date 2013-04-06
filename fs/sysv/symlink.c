/*
 *  linux/fs/sysv/symlink.c
 *
 *  minix/symlink.c
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  coh/symlink.c
 *  Copyright (C) 1993  Pascal Haible, Bruno Haible
 *
 *  sysv/symlink.c
 *  Copyright (C) 1993  Bruno Haible
 *
 *  SystemV/Coherent symlink handling code
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/sysv_fs.h>
#include <linux/stat.h>

#include <asm/uaccess.h>

static int sysv_readlink(struct inode *, char *, int);

/*
 * symlinks can't do much...
 */
struct inode_operations sysv_symlink_inode_operations = {
	NULL,			/* no file-operations */
	NULL,			/* create */
	NULL,			/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	sysv_readlink,		/* readlink */
	NULL,			/* readpage */
	NULL,			/* writepage */
	NULL,			/* bmap */
	NULL,			/* truncate */
	NULL			/* permission */
};

static int sysv_readlink(struct inode * inode, char * buffer, int buflen)
{
	struct buffer_head * bh;
	char * bh_data;
	int i;
	char c;

	if (buflen > inode->i_sb->sv_block_size_1)
		buflen = inode->i_sb->sv_block_size_1;
	bh = sysv_file_bread(inode, 0, 0);
	iput(inode);
	if (!bh)
		return 0;
	bh_data = bh->b_data;
	i = 0;
	while (i<buflen && (c = bh_data[i])) {
		i++;
		put_user(c,buffer++);
	}
	brelse(bh);
	return i;
}
