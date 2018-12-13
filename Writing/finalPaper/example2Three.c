struct page {
	unsigned long flags;
	atomic_t _count;
	atomic_t _mapcount;
	unsigned long private;
	struct address_space *mapping;
	pgoff_t index;
	struct list_head lru;
	void *virtual;
	};
