#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	//printk()
#include <linux/fs.h>	//Aparently everything...
#include <linux/errno.h>
#include <linux/vmallic.h>
#include <linux/genhd.h>
#include <blkdev.h>
#include <linux/hdreg.h>

static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int nsectors = 1024; /* How big the drive is */
module_param(nsectors, int, 0);

#define KERNEL_SECTOR_SIZE 512

static struct request_queue *Queue;

static struct sbd_device {
	unsigned li=ong size;
	spinlock_t lock;
	u8 *data;
	struct gendisk *gd;
} Device;

static void sbd_transfer(struct sbd_device *dev, sector_t sector,
	unsigned long nsect, char *buffer, int write) 
{
	unsigned long offset = sector * logical_block_size;
	unsigned long nbytes = nsect * logical_block_size;
	if ((offset + nbytes) > dev->size) {
		printk (KERN_NOTICE "sbd: Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	if (write)
		//encrypt
		memcpy(dev->data + offset, buffer, nbytes);
	else
		//decrypt
		memcpy(buffer, dev->data + offset, nbytes);
}

static void sbd_request(struct request_queue *q) {
	struct request *req;
	req = blk_fetch_request(q);
	while (req != NULL) {
		// blk_fs_request() was removed in 2.6.36 - many thanks to
		// Christian Paro for the heads up and fix...
		if (req == NULL || (req->cmd_type != REQ_TYPE_FS)) {
			printk (KERN_NOTICE "Skip non-CMD request\n");
			__blk_end_request_all(req, -EIO);
			continue;
		}
		sbd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),req->buffer, rq_data_dir(req));
		if ( ! __blk_end_request_cur(req, 0) ) {
			req = blk_fetch_request(q);
		}
	}
}

int sbd_getgeo(struct block_device * block_device, struct hd_geometry * geo) {
	long size;
	/* We have no real geometry, of course, so make something up. */
	size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	return 0;
}

static int __init sbd_init(void){
	Device.size = nsectors * logical_block_size;
	spin_lock_init(&Device.lock);
	Device.data = vmalloc(Device.size);
	if(Device.data == NULL)
		return -ENOMEM;
	//Get a request queue for the device
	Queue = blk_init_queue(sbd_request, &Device.lock);
	//Register the driver
	major_num = register_blkdev(major_num, "sbd");
	if(major_num < 0){
		printl("sbd: unable to get major number \n");
		goto out;
	}
	//Get the gen disk structure
	Device.gd = alloc_disk(16);
	if(!Device.gd)
		goto out_unregister;
	//Go through and set gendisk values
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	strcpy(Device.gd->dick_name, "sbd0");
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	add_disk(Device.gd);

	return 0;

	out_unregister:
		unregister_blkdev(major_num, "sbd");
	out:
		vfree(Device.data);
		return -ENOMEM;
}

static void __exit sbd_exit(void){
	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "sbd");
	blk_cleanup_queue(Queue);
	vfree(Device.data);
}

module_init(sbd_init);
module_exit(sbd_exit);
