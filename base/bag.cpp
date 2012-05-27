#include "base/bag.hpp"

#include <sys/mman.h>

BagMemoryHandler::PageHeader* BagMemoryHandler::allocate_page() {
	static const size_t PageSize = 4096;
	size_t sz = element_size_;
	if (sz < PageSize) sz = PageSize;
	byte* memory = (byte*)mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	PageHeader* p = new(memory) PageHeader;
	p->next = nullptr;
	p->begin = memory + sizeof(PageHeader);
	p->current = p->begin;
	p->end = memory + sz;
	return p;
}

void BagMemoryHandler::deallocate_page(PageHeader* p) {
	munmap(p, p->end - p->begin + sizeof(PageHeader));
}

byte* BagMemoryHandler::allocate() {
	if (free_list_ == nullptr) {
		if (current_ == nullptr) {
			current_ = allocate_page();
			head_ = current_;
		}
		
		if (current_->end - current_->current < element_size_) {
			PageHeader* p = allocate_page();
			current_->next = p;
			current_ = p;
		}
		
		byte* memory = current_->current;
		current_->current += element_size_;
		return memory;
	} else {
		byte* memory = *free_list_;
		free_list_ = (byte**)*memory;
		return memory;
	}
}

void BagMemoryHandler::deallocate(byte* ptr) {
	*(byte***)ptr = free_list_;
	free_list_ = (byte**)ptr;
}

void BagMemoryHandler::clear() {
	PageHeader* p = head_;
	while (p != nullptr) {
		PageHeader* next = p->next;
		deallocate_page(p);
		p = next;
	}
	current_ = nullptr;
	head_ = nullptr;
}