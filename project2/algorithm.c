

static void look_add_request(struct request_queue *q, struct request *rq)
{

	struct look_data *nd = q->elevator->elevator_data;
	struct list_head* current_position = NULL;
	struct request* current_request;

	printk(KERN_DEBUG "ADDING %llu, last serviced request was at %llu\n", blk_rq_pos(rq), disk_head);
	if (list_empty(&nd->queue)) {
		printk(KERN_DEBUG "ADD: queue list empty adding item to queue. \n");
		list_add(&rq->queuelist, &nd->queue);
	}
	else{
		list_for_each(current_position, &nd->queue){
			current_request = list_entry(current_position, struct request, queuelist);
			if(blk_rq_pos(rq) < disk_head){
				if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request)-1){
					printk(KERN_DEBUG "MERGING REQUESTS: %llu with %llu\n",blk_rq_pos(rq), blk_rq_pos(current_request));
					elv_merge_requests(q,rq,current_request);
					return;
				}
				if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) < blk_rq_pos(current_request)){
					printk(KERN_DEBUG "LOOK_ADD: adding request which will be serviced on the next run of the disk head %llu\n", blk_rq_pos(rq));
					break;
				}
			}
			else{
				if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request)-1){
					printk(KERN_DEBUG "MERGING REQUESTS: %llu with %llu\n",blk_rq_pos(rq), blk_rq_pos(current_request));
					elv_merge_requests(q,rq,current_request);
					return;
				}
				if(blk_rq_pos(current_request) < disk_head || blk_rq_pos(rq) < blk_rq_pos(current_request)){
					printk(KERN_DEBUG "LOOK_ADD: adding a request which will be serviced on this run of the disk head %llu\n", blk_rq_pos(rq));
					break;
				}
			}

		}
		list_add_tail(&rq->queuelist, current_position);
	}
}

