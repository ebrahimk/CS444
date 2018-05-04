/*
 * elevator look
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

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

	//This will contain the location of the request
	sector_t location;
	struct request* former_request;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		
		location = blk_rq_pos(rq);
		printk("look_dispatch adding request at %d\n", location);
					
		//now we need to check to see if we want to merge the request
		//if the position is adjacent to the last serviced request we want to front merge it 

		elv_dispatch_sort(q, rq);		

		return 1;
	}
	return 0;
}

//Theory: Two queues:
//device queue == q 
//noop_dipatch() is to dispatch a request from internal queue to device queue
//noop_dispatch() calls elv_dispatch_sort() to sort the
//request in the ascending order.

static void look_add_request(struct request_queue *q, struct request *rq)
{
	struct look_data *nd = q->elevator->elevator_data;
	struct list_head *current = NULL;
	//list_for_each(current, &nd->queue)   //Definied in list.h to equal "for (pos = (head)->next; pos != (head); pos = pos->next)"
	//we are iterating through the list we need to insert the request in the proper locatio
	//{}
		location = blk_rq_pos(rq);
                printk("look_add adding request at %d\n", location);
	
	list_add_tail(&rq->queuelist, &nd->queue);
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
