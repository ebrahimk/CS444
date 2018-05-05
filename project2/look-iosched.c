/*
 * elevator look
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

//This variable will keep track of where the disk head is
sector_t disk_head = -1;

struct look_data {
	struct list_head queue;
};

static void look_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}


static int look_dispatch(struct request_queue *q, int force)
{

	struct look_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		
		//rq is the request which is most recently dispatched, thus the disk head is pointing to this sector after dispatching the request
		disk_head = blk_rq_pos(rq);
                printk(KERN_DEBUG "LOOK_DISPATCH: %llu\n", blk_rq_pos(rq));

		list_del_init(&rq->queuelist);	
		
		//perform organizational work of the submission queue in look_add_request() then just add request from that queue in the normal order
		elv_dispatch_add_tail(q, rq);			
		//elv_dispatch_sort(q, rq);		

		return 1;
	}
	return 0;
}

static void look_add_request(struct request_queue *q, struct request *rq)
{
	
	struct look_data *nd = q->elevator->elevator_data;
	struct list_head* current_position;
	struct request* current_request;

	//TRY TO MERGE
	
	//If the incoming request is located at a sector less than the sector at which the 
	//disk head is located, we want to continue servicing request to right of the disk head until we reach the maximum request.
	//We thus continue to iterate through our linked list until we pass the maximium request at which point
	//the head of the disk goes to the far terminal of the plate. Thus we want to insert this request at a sector less 
	//than the disk_head that maintains ascending order.
	//QUEUE should look like (* indicates disk head and requests are serviced from left to right) 
	//NEW REQUEST = 500  
	//last serviced request @ 676 
	// * 7000 8888 19675 20768 (we slide the head to the far terminal)  2 3 7 14 444 (place new request here) 890 ...
	list_for_each(current_position, &nd->queue){
		current_request = list_entry(current_position, struct request, queuelist);
		if(blk_rq_pos(rq) < disk_head)
		{	
			if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) < blk_rq_pos(current_request))
			{
				//break out of loop, current_request retains its value and we can add our request outside of the loop
				break;
			}
		}
		//if the new request is greater than or equal to the last serviced request we simply add the request so that it will be processed within this run 
		//of the disk_head before it resets its position at the far terminal
		if(blk_rq_pos(rq) >= disk_head)
			if(blk_rq_pos(rq) < blk_rq_pos(current_request))
				break;
		}

	//now current_request is pointing to the value of the node where the new request should be added 		
	//printk(KERN_DEBUG "LOOK_ADD: %llu\n", blk_rq_pos(rq));
        list_add_tail(&rq->queuelist, current_position);
}

static struct request *
look_former_request(struct request_queue *q, struct request *rq)
{
	struct look_data *nd = q->elevator->elevator_data;
	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
look_latter_request(struct request_queue *q, struct request *rq)
{
	struct look_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int look_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct look_data *nd;
	struct elevator_queue *eq;
	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void look_exit_queue(struct elevator_queue *e)
{
	struct look_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_look = {
	.ops = {
		.elevator_merge_req_fn		= look_merged_requests,
		.elevator_dispatch_fn		= look_dispatch,
		.elevator_add_req_fn		= look_add_request,
		.elevator_former_req_fn		= look_former_request,
		.elevator_latter_req_fn		= look_latter_request,
		.elevator_init_fn		= look_init_queue,
		.elevator_exit_fn		= look_exit_queue,
	},
	.elevator_name = "look",
	.elevator_owner = THIS_MODULE,
};

static int __init look_init(void)
{
	return elv_register(&elevator_look);
}

static void __exit look_exit(void)
{
	elv_unregister(&elevator_look);
}

module_init(look_init);
module_exit(look_exit);


MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");
