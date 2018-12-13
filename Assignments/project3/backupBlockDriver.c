#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	//printk()
#include <linux/fs.h>	//Aparently everything...
#include <linux/errno.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/crypto.h>

static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int nsectors = 1024; /* How big the drive is */
module_param(nsectors, int, 0);

//Define key module parameter
//m,odule_param(variable name, variable type character pointer (charp), read permissions)
char key[32]="laksi4839kdm1pls90dmnwop49cnflmq";
module_param(key, charp, 0);

#define KERNEL_SECTOR_SIZE 512
#define KEY_LEN 32	//AES key size can be 128, 192, or 256 BITS.

//Define Request Queue
static struct request_queue *Queue;

//Define Device structure
static struct sbd_device {
	unsigned long size;
	spinlock_t lock;
	u8 *data;
	struct gendisk *gd;

	struct crypto_cipher *cipher_handle;
	u8 key[KEY_LEN];
} Device;

static void sbd_transfer(struct sbd_device *dev, sector_t sector,
	unsigned long nsect, char *buffer, int write) 
{
	unsigned long offset = sector * logical_block_size;
	unsigned long nbytes = nsect * logical_block_size;
	unsigned long block_size = crypto_cipher_blocksize(dev->cipher_handle);
	char* location;
	unsigned long i;
	if ((offset + nbytes) > dev->size) {
		printk (KERN_NOTICE "sbd: Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	location = dev->data +  offset;
	if (write)
		//encrypt
		//We need to loop through because AES encrypts in blocks, and our data may be larger than a single block.  
		//The AES blocksize is 512 BITS
		for(i=0; i<nbytes; i+=block_size){
			crypto_cipher_encrypt_one(dev->cipher_handle, &location[i],&buffer[i]);
		}
	else
		//decrypt
		for(i=0; i<nbytes; i+=block_size){
			crypto_cipher_decrypt_one(dev->cipher_handle, &location[i], &buffer[i]);
		}
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
		//Apparently req->buffer doesn't exist.  We need to find where the buffer actually is, but I haven't figured out where it is.
		sbd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),bio_data(req->bio), rq_data_dir(req));
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

static struct block_device_operations sbd_ops = {
	.owner = THIS_MODULE,
	.getgeo = sbd_getgeo,
};

static int __init sbd_init(void){
	Device.size = nsectors * logical_block_size;
	
	//Stop multiple writes at the same time.
	spin_lock_init(&Device.lock);
	Device.data = vmalloc(Device.size);
	if(Device.data == NULL)
		return -ENOMEM;
	
	//Get a request queue for the device
	Queue = blk_init_queue(sbd_request, &Device.lock);
	
	//Register the driver
	major_num = register_blkdev(major_num, "sbd");
	if(major_num < 0){
		printk("sbd: unable to get major number \n");
		goto out;
	}
	
	//Get the gen disk structure
	Device.gd = alloc_disk(16);
	if(!Device.gd)
		goto out_unregister;
	
	//Go through and set gendisk values.  these are used when we add the disk.
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	unsigned int result;

	//Setup cipher
	//Allocate the cipher
	Device.cipher_handle = crypto_alloc_cipher("aes", 0, CRYPTO_ALG_ASYNC);
	if(IS_ERR(Device.cipher_handle)){
		printk("sbd: failed to create the cipher object\n");
		goto out_unregister;
	}
	
	//Generate and set the key
	//TODO: currently incorrect.  Needs to be able to be set from outside the module
	//memcpy(Device.key,key,KEY_LEN);		
	result = crypto_cipher_setkey(Device.cipher_handle, key, KEY_LEN);
	if(result != 0){
		//error out
		printk("sbd: failed to set cipher key\n");
		goto out_unregister;
	}
	
	strcpy(Device.gd->disk_name, "sbd0");
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	add_disk(Device.gd);

	return 0;
	//this is the error area.  If we encounter an error in the function, GOTO here and free memory.
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
