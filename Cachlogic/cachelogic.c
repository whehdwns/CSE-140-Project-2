//whehdwns
#include "tips.h"

/* The following two functions are defined in util.c */

/* finds the highest 1 bit, and returns its position, else 0xFFFFFFFF */
unsigned int uint_log2(word w); 

/* return random int from 0..x-1 */
int randomint( int x );

/*
  This function allows the lfu information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lfu information
 */
char* lfu_to_string(int assoc_index, int block_index)
{
	/* Buffer to print lfu information -- increase size as needed. */
	static char buffer[9];
	sprintf(buffer, "%u", cache[assoc_index].block[block_index].accessCount);

	return buffer;
}

/*
  This function allows the lru information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lru information
 */
char* lru_to_string(int assoc_index, int block_index)
{
	/* Buffer to print lru information -- increase size as needed. */
	static char buffer[9];
	sprintf(buffer, "%u", cache[assoc_index].block[block_index].lru.value);

	return buffer;
}

/*
  This function initializes the lfu information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

 */
void init_lfu(int assoc_index, int block_index)
{
	cache[assoc_index].block[block_index].accessCount = 0;
}

/*
  This function initializes the lru information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

 */
void init_lru(int assoc_index, int block_index)
{
	cache[assoc_index].block[block_index].lru.value = 0;
}

/*
  This is the primary function you are filling out,
  You are free to add helper functions if you need them

  @param addr 32-bit byte address
  @param data a pointer to a SINGLE word (32-bits of data)
  @param we   if we == READ, then data used to return
              information back to CPU

              if we == WRITE, then data used to
              update Cache/DRAM
 */
void accessMemory(address addr, word* data, WriteEnable we)
{
	/* Declare variables here */
	//Cache : address , cache size, block size, associativity, tag bits, index bits, offset bits, 
	unsigned int tagbits, indexbits, offsetbits, tags; //offset_value, index_value,
	unsigned int hit = 0, LRU_data = 0, LRU_value = 0;
		//lru.data - pointer to lru information
		//lru.value - int that represents lru information
	//If the hit is 0, then that is miss
	//If the hit is 1, then that is hit
	address old = 0;
	TransferUnit byte_size = 0;
	/* handle the case of no cache at all - leave this in */
	if(assoc == 0) {
		accessDRAM(addr, (byte*)data, WORD_SIZE, we);
		return;
	}

	/*
  You need to read/write between memory (via the accessDRAM() function) and
  the cache (via the cache[] global structure defined in tips.h)

  Remember to read tips.h for all the global variables that tell you the
  cache parameters

  The same code should handle random, LFU, and LRU policies. Test the policy
  variable (see tips.h) to decide which policy to execute. The LRU policy
  should be written such that no two blocks (when their valid bit is VALID)
  will ever be a candidate for replacement. In the case of a tie in the
  least number of accesses for LFU, you use the LRU information to determine
  which block to replace.

  Your cache should be able to support write-through mode (any writes to
  the cache get immediately copied to main memory also) and write-back mode
  (and writes to the cache only gets copied to main memory when the block
  is kicked out of the cache.

  Also, cache should do allocate-on-write. This means, a write operation
  will bring in an entire block if the block is not already in the cache.

  To properly work with the GUI, the code needs to tell the GUI code
  when to redraw and when to flash things. Descriptions of the animation
  functions can be found in tips.h
	 */

	/* Start adding code here */

	//*** My code segment ***//

	//Compute bit size
	indexbits = uint_log2(set_count);//number of sets   /   # blocks/cache
	offsetbits = uint_log2(block_size);//Cache block size in bytes
	tagbits = 32 - indexbits - offsetbits; // addrlength –offset –index
	
	//Compute the values
	unsigned int offset = addr << (tagbits + indexbits);
	offset = offset >> (tagbits + indexbits);

	unsigned int index = addr << (tagbits);
	index = index >> (tagbits + offsetbits);

	tags = addr >> (offsetbits + indexbits);

	//byte size Block size(2n bytes where n ranges from 2 to 5)
	//Cache block size in bytes
	switch (block_size) {
	case(4): byte_size = 2; break;
	case(8): byte_size = 3; break;
	case(16): byte_size = 4; break;
	case(32): byte_size = 5; break;
	}
	// memoryaccess read case
	//READ -> WRITE
	//WRITE ENABLE =we

	//READ
	if (policy == LRU)
	{
		for (int i = 0; i < assoc; i++)
			cache[index].block[i].lru.value++;
	}
	
	if (we == READ)
	{
		// Service the read
		hit = 0;//miss
		for (int i = 0; i < assoc; i++)
		{ 
			if (tags == cache[index].block[i].tag && cache[index].block[i].valid == 1) // a block hit
			{
				cache[index].block[i].lru.value = 0;
				cache[index].block[i].valid = 1;
				hit = 1;//hit
				memcpy (data,(cache[index].block[i].data + offset), 4);
				//void* memcpy(void* dest, void* src, size_t amount);
			}
		}

		if (hit == 0) // a set miss
		{
			//FOR CACHE, it uses LRU or Random for replacement.
			// least recently used/  cache out block which has been accessed (read or write) least recently
			//recent past use implies likely future use : in fact, this is a very effective policy
			if (policy == LRU)
			{
				for (int i = 0; i < assoc; i++)
					if (LRU_value < cache[index].block[i].lru.value)
					{
						LRU_value = cache[index].block[i].lru.value;
						LRU_data = i;
					}

			}
			//if no invaild LRUi, then replacement with random.//Cache replacement policy
			else if (policy == RANDOM)
				LRU_data = randomint(assoc);

			// If the block we are about to replace is dirty, replace it in memory
			if(cache[index].block[LRU_data].dirty == DIRTY)
			{
				// Compute the address
				old = cache[index].block[LRU_data].tag << (indexbits + offsetbits) + (index << offsetbits);
				accessDRAM(old, (cache[index].block[LRU_data].data), byte_size, WRITE);
			}
			//Perform load from dram
			accessDRAM(addr, (cache[index].block[LRU_data].data), byte_size, READ);
			cache[index].block[LRU_data].lru.value = 0;
			cache[index].block[LRU_data].valid = 1;	
			cache[index].block[LRU_data].dirty = VIRGIN; //VIRGIN is from the tips.h 
			//VIRGIN == The dirty bit revert back to 0 if a dirty cache block is sent from the cache to physical memory.
			cache[index].block[LRU_data].tag = tags;

			memcpy (data,(cache[index].block[LRU_data].data + offset), 4);	
		}

	}
	//END READ Policy

	// memoryaccess write case

	//Write Policy (Write through v. write back)
	//write case :WRite back / Write through similar to READ.

	//WRITE BACK :update word in cache block
	else /// write case
	{
		hit = 0;
		//From tips.h Memory sync policy  
		if (memory_sync_policy == WRITE_BACK)
		{
			for (int i = 0; i < assoc; i++)
			{ 
				if (tags == cache[index].block[i].tag && cache[index].block[i].valid == 1) // a block hit
				{
					cache[index].block[i].dirty = DIRTY;
					cache[index].block[i].lru.value = 0;
					cache[index].block[i].valid = 1;
					hit = 1;//hit
					memcpy(data, (cache[index].block[i].data + offset), 4);
				}

			}

			if (hit == 0)
			{
				// least recently used
				if (policy == LRU)
				{
					for (int i = 0; i < assoc; i++)
						if(LRU_value < cache[index].block[i].lru.value)
						{
							LRU_data = i;
							LRU_value = cache[index].block[i].lru.value;
						}

				}
				//if no invaild LRUi, then replacement with random.
				else if (policy == RANDOM)
					LRU_data = randomint(assoc);

				//add ‘dirty’ bit to each block indicating that memory needs to be updated when block is replaced
				// If the block we are about to replace is dirty, replace it in memory
				if(cache[index].block[LRU_data].dirty == DIRTY)
				{
					old = cache[index].block[LRU_data].tag << (indexbits + offsetbits) + (index << offsetbits);
					accessDRAM(old, (cache[index].block[LRU_data].data), byte_size, WRITE);
				}
				//Perform load from dram
				cache[index].block[LRU_data].lru.value = 0;
				cache[index].block[LRU_data].valid = 1;
				cache[index].block[LRU_data].dirty = VIRGIN;
				cache[index].block[LRU_data].tag = tags;

				accessDRAM(addr, (cache[index].block[LRU_data].data), byte_size, READ);
				memcpy ((cache[index].block[LRU_data].data + offset),data, 4);
			}
		}
		else // write-through case : update the word in cache block and corresponding word in memory
		{
			for (int i = 0; i < assoc; i++)
			{
				if (tags == cache[index].block[i].tag && cache[index].block[i].valid == 1) // a block hit
				{
					memcpy ((cache[index].block[i].data + offset),data, 4);
					cache[index].block[i].dirty = VIRGIN;
					cache[index].block[i].lru.value = 0;
					cache[index].block[i].valid = 1;
					hit = 1;//hit
					accessDRAM(addr, (cache[index].block[LRU_data].data), byte_size, WRITE);

				}
			}

			if (hit == 0)
			{
				if (policy == LRU)// least recently used
				{
					for (int i = 0; i < assoc; i++)
						if(LRU_value < cache[index].block[i].lru.value)
						{
							LRU_data = i;
							LRU_value = cache[index].block[i].lru.value;
						}

				}
				//if no invaild LRUi, then replacement with random.
				else if (policy == RANDOM)
					LRU_data = randomint(assoc);

				//Perform load from dram	
				accessDRAM(addr, (cache[index].block[LRU_data].data), byte_size, READ);
				cache[index].block[LRU_data].lru.value = 0;
				cache[index].block[LRU_data].valid = 1;
				cache[index].block[LRU_data].dirty = VIRGIN;
				cache[index].block[LRU_data].tag = tags;

				memcpy ((cache[index].block[LRU_data].data + offset),data, 4);
			}
		}

	}



}
//END WRITE {WRITE BACK, WRITE THROUGH}
