void* memory;
size_t num_bytes = 10; 
memory = kmalloc(num_bytes, GFP_DMA);
printk("Allocated 10 bytes of memory from zone DMA\n");
kfree(memory); 
