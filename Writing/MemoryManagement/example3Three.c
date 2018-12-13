struct slab { 
	struct list_head list;
	unsigned long colouroff; 
	void *s_mem;
	unsigned int inuse;
	kmem_bufctl_t free; \
	};
