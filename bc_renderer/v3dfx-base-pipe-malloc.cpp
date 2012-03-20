#include <cmem.h>

void mem_cmem_init();
int mem_cmem_alloc(int numBytes, void**virtualAddress, int* physicalAddress);
int mem_cmem_free(void* virtualAddress);
void mem_cmem_deinit();

static CMEM_AllocParams cmemParams = { CMEM_POOL, CMEM_NONCACHED, 4096 };

void mem_cmem_init()
{
	CMEM_init();
}

int mem_cmem_alloc(int numBytes, void**virtualAddress, int* physicalAddress)
{
	void* virtualAddress; 
	int physicalAddress;

	//setup CMEM pointers  
	*virtualAddress = CMEM_alloc(numBytes, &cmemParams);
	if(!*virtualAddress)
	{
		printf("Error in CMEM_alloc\n");
		return 1;
	}
	*physicalAddress = CMEM_getPhys(virtualAddress);
	if(!*physicalAddress)                                      
	{
		printf("Error in CMEM_getPhys\n");
		return 2;
	}
	return 0;
}

int mem_cmem_free(void* virtualAddress)
{
	CMEM_free(virtualAddress, &cmemParams);
	return 0;
}

void mem_cmem_deinit()
{
	CMEM_exit();
}
