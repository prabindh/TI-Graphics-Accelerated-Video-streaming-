/******************************************************************************
 * Malloc via CMEM
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Primary author: Aditya Nellutla (aditya.n@ti.com)
 * Modified to include v3dfx-base: Prabindh Sundareson (prabu@ti.com)
 ****************************************************************************/

#include <cmem.h>

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
