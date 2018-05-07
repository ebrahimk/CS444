static void look_add_request(struct request_queue *q, struct request *rq)
{

	struct look_data *nd = q->elevator->elevator_data;
	struct list_head* current_position = NULL;
	struct request* current_request;

	printk(KERN_DEBUG "ADDING %llu, last serviced request was at %llu\n", blk_rq_pos(rq), disk_head);
	if (list_empty(&nd->queue)) {
		queue_exists = 0;
		list_add(&rq->queuelist, &nd->queue);
	}
	else{
		queue_exists = 1;
		list_for_each(current_position, &nd->queue)
		{
			current_request = list_entry(current_position, struct request, queuelist);
			if(blk_rq_pos(rq) < disk_head)
			{
				if( (blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) == blk_rq_pos(current_request)-1) || 
				    (blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request))) {

					elv_merge_requests(q,rq,current_request);
					return;
				}
				if(blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) < blk_rq_pos(current_request))
				{
					break;
				}
				else{
					printk(KERN_DEBUG "LOOK_ADD: request going to very end of the queue  %llu\n", blk_rq_pos(rq));
				}
			}
			else{
				if( (blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) ==  blk_rq_pos(current_request)-1) ||
				    (blk_rq_pos(current_request) < disk_head && blk_rq_pos(rq) && 
				     blk_rq_pos(rq) ==  blk_rq_pos(current_request))) {

					elv_merge_requests(q,rq,current_request);
					return;
				}
				if(blk_rq_pos(current_request) < disk_head || blk_rq_pos(rq) < blk_rq_pos(current_request)){
					break;
				}
			}

		}
		list_add_tail(&rq->queuelist, current_position);
	}
}
