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
int queue_exists= 0;


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
                if(queue_exists > 0){
			printk(KERN_DEBUG "LOOK_DISPATCH: %llu\n", blk_rq_pos(rq));
		}
		//disk_head is the location of the last serviced request, and  blk_rq_pos(rq) is the
		//new request. If direction is negative we are travelling upward (ascending), positive we are travelling down (descending)
		//direction = disk_head -  blk_rq_pos(rq);
		
		
			
		list_del_init(&rq->queuelist);	
		elv_dispatch_add_tail(q, rq);			
		disk_head = blk_rq_pos(rq);
		//elv_dispatch_sort(q, rq);		

		return 1;
	}
	return 0;
}

static void look_add_request(struct request_queue *q, struct request *rq)
{
	
	struct look_data *nd = q->elevator->elevator_data;
	struct list_head* current_position = NULL;
	struct request* current_request; 

	printk(KERN_DEBUG "ADDING %llu, last serviced request was at %llu\n", blk_rq_pos(rq), disk_head); 	
	if (list_empty(&nd->queue)) {
		printk(KERN_DEBUG "ADD: queue list empty adding item to queue. \n");
		queue_exists = 0;	
		list_add(&rq->queuelist, &nd->queue);
	}
	else{
	queue_exists = 1;
	list_for_each(current_position, &nd->queue)
	{			
		current_request = list_entry(current_position, struct request, queuelist);
		printk(KERN_DEBUG "ADDING %llu, QUEUE: %llu\n", blk_rq_pos(rq), blk_rq_pos(current_request));
		if(blk_rq_pos(rq) < disk_head)
		{
		//less than disk head service on the next run of the disk head	
			if((blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request)-1) || (blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request))){
				printk(KERN_DEBUG "MERGING REQUESTS: %llu with %llu\n",blk_rq_pos(rq), blk_rq_pos(current_request)); 
				elv_merge_requests(q,rq,current_request);
				return;
			}
			//if the following if statement fails then we have requestwhich is smaller than the disk head,
			//that will be serviced on the next run. Thus this request goes to the very end of the queue 
			if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) < blk_rq_pos(current_request))
			{
				//break out of loop, current_request retains its value and we can add our request outside of the loop
				printk(KERN_DEBUG "LOOK_ADD: adding request which will be serviced on the next run of the disk head %llu\n", blk_rq_pos(rq));
				break;
			}
			else{
				printk(KERN_DEBUG "LOOK_ADD: request going to very end of the queue  %llu\n", blk_rq_pos(rq));
			}
		}
                //if the new request is greater than or equal to the last serviced request we simply add the request so that it will be processed within this run 
                //of the disk_head before it resets its position at the far terminal
		else
		{
		        if((blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request)-1) ||(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) && blk_rq_pos(rq) ==  blk_rq_pos(current_request))){
                                //we have a request in the queue that is adjacent to the new request, merge them
                                printk(KERN_DEBUG "MERGING REQUESTS: %llu with %llu\n",blk_rq_pos(rq), blk_rq_pos(current_request));
                                elv_merge_requests(q,rq,current_request);
                                return;
                        }	

			//printk(KERN_DEBUG "LOOK_ADD_IN_GREATER: after disk head %llu\n", blk_rq_pos(rq));
			//iterate through the queue until we either find a location that is less than the max request or rq is the new max request 
			if(blk_rq_pos(current_request) < disk_head || blk_rq_pos(rq) < blk_rq_pos(current_request))
			{
				printk(KERN_DEBUG "LOOK_ADD: adding a request which will be serviced on this run of the disk head %llu\n", blk_rq_pos(rq));
				break;
			}
		}

	//now current_request is pointing to the value of the node where the new request should be added 		
	//printk(KERN_DEBUG "LOOK_ADD: %llu\n", blk_rq_pos(rq));
	}
        list_add_tail(&rq->queuelist, current_position);
	}
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
