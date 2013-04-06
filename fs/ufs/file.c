/*
 *  linux/fs/ufs/file.c
 *
 * Copyright (C) 1998
 * Daniel Pirkl <daniel.pirkl@email.cz>
 * Charles University, Faculty of Mathematics and Physics
 *
 *  from
 *
 *  linux/fs/ext2/file.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  ext2 fs regular file handling primitives
 */

#include <asm/uaccess.h>
#include <asm/system.h>

#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/ufs_fs.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/stat.h>
#include <linux/locks.h>
#include <linux/mm.h>
#include <linux/pagemap.h>

#define	NBUF	32

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/*
 * Make sure the offset never goes beyond the 32-bit mark..
 */
static long long ufs_file_lseek(
	struct file *file,
	long long offset,
	int origin )
{
	long long retval;
	struct inode *inode = file->f_dentry->d_inode;

	switch (origin) {
		case 2:
			offset += inode->i_size;
			break;
		case 1:
			offset += file->f_pos;
	}
	retval = -EINVAL;
	/* make sure the offset fits in 32 bits */
	if (((unsigned long long) offset >> 32) == 0) {
		if (offset != file->f_pos) {
			file->f_pos = offset;
			file->f_reada = 0;
			file->f_version = ++event;
		}
		retval = offset;
	}
	return retval;
}

static inline void remove_suid(struct inode *inode)
{
	unsigned int mode;

	/* set S_IGID if S_IXGRP is set, and always set S_ISUID */
	mode = (inode->i_mode & S_IXGRP)*(S_ISGID/S_IXGRP) | S_ISUID;

	/* was any of the uid bits set? */
	mode &= inode->i_mode;
	if (mode && !suser()) {
		inode->i_mode &= ~mode;
		mark_inode_dirty(inode);
	}
}

static int ufs_writepage (struct file *file, struct page *page)
{
	struct dentry *dentry = file->f_dentry;
	struct inode *inode = dentry->d_inode;
	unsigned long block;
	int *p, nr[PAGE_SIZE/512];
	int i, err, created;
	struct buffer_head *bh;

	i = PAGE_SIZE >> inode->i_sb->s_blocksize_bits;
	block = page->offset >> inode->i_sb->s_blocksize_bits;
	p = nr;
	bh = page->buffers;
	do {
		if (bh && bh->b_blocknr)
			*p = bh->b_blocknr;
		else
			*p = ufs_getfrag_block(inode, block, 1, &err, &created);
		if (!*p)
			return -EIO;
		i--;
		block++;
		p++;
		if (bh)
			bh = bh->b_this_page;
	} while (i > 0);

	brw_page(WRITE, page, inode->i_dev, nr, inode->i_sb->s_blocksize, 1);
	return 0;
}

static long ufs_write_one_page(struct file *file, struct page *page, unsigned long offset, unsigned long bytes, const char *buf)
{
	return block_write_one_page(file, page, offset, bytes, buf, ufs_getfrag_block);
}

/*
 * Write to a file (through the page cache).
 */
static ssize_t
ufs_file_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	return generic_file_write(file, buf, count, ppos, ufs_write_one_page);
}

/*
 * Called when an inode is released. Note that this is different
 * from ufs_open: open gets called at every open, but release
 * gets called only when /all/ the files are closed.
 */
static int ufs_release_file (struct inode * inode, struct file * filp)
{
	return 0;
}

/*
 * We have mostly NULL's here: the current defaults are ok for
 * the ufs filesystem.
 */
static struct file_operations ufs_file_operations = {
	ufs_file_lseek,	/* lseek */
	generic_file_read,	/* read */
	ufs_file_write, 	/* write */
	NULL,			/* readdir - bad */
	NULL,			/* poll - default */
	NULL, 			/* ioctl */
	generic_file_mmap,	/* mmap */
	NULL,			/* no special open is needed */
	NULL,			/* flush */
	ufs_release_file,	/* release */
	NULL, 			/* fsync */
	NULL,			/* fasync */
	NULL,			/* check_media_change */
	NULL			/* revalidate */
};

struct inode_operations ufs_file_inode_operations = {
	&ufs_file_operations,/* default file operations */
	NULL,			/* create */
	NULL,			/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	generic_readpage,	/* readpage */
	ufs_writepage,		/* writepage */
	ufs_bmap,		/* bmap */
	ufs_truncate,		/* truncate */
	NULL, 			/* permission */
	NULL,			/* smap */
	NULL,			/* revalidate */
	block_flushpage,	/* flushpage */
};
