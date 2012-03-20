/******************************************************************************
*****************************************************************************
 * Pipe functions
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

#include "pipe-interface.h"

// Size of the texture we create
#define BCSINK_FIFO_NAME "gstbcsink_fifo"
#define BCINIT_FIFO_NAME "gstbcinit_fifo"
#define BCACK_FIFO_NAME "gstbcack_fifo"

int fd_bcsink_fifo_rec;
int fd_bcinit_fifo_rec;
int fd_bcack_fifo_rec;

bc_gstpacket bcbuf_receive;
bc_gstpacket bcbuf;

int initPipes(imgstream_device_attributes* initparams)
{
	int n=-1;
	/*************************************************************************************
	* Open Named pipes for communication with the gst-bcsink plugin
	**************************************************************************************/
	fd_bcinit_fifo_rec = open( BCINIT_FIFO_NAME, O_RDONLY);
	if(fd_bcinit_fifo_rec < 0)
	{
		printf (" Failed to open bcinit_fifo FIFO - fd: %d\n", fd_bcinit_fifo_rec);
		goto exit1;
	}

	fd_bcsink_fifo_rec = open( BCSINK_FIFO_NAME, O_RDONLY );
	if(fd_bcsink_fifo_rec < 0)
	{
		printf (" Failed to open bcsink_fifo FIFO - fd: %d\n", fd_bcsink_fifo_rec);
		goto exit2;
	}

	fd_bcack_fifo_rec = open( BCACK_FIFO_NAME, O_WRONLY);
	if(fd_bcack_fifo_rec < 0)
	{
		printf (" Failed to open bcack_fifo FIFO - fd: %d\n", fd_bcack_fifo_rec);
		goto exit3;
	}

	//n = read(fd_bcinit_fifo_rec, initparams, sizeof(imgstream_device_attributes));

	close(fd_bcack_fifo_rec);
exit3:
	close(fd_bcsink_fifo_rec);
exit2:
	close(fd_bcinit_fifo_rec);
exit1:	
	return n;
}

int read_pipe()
{
	int n;
	//n = read(fd_bcsink_fifo_rec, &bcbuf, sizeof(bc_gstpacket));
	return n;
}

int write_pipe()
{
	int n;
	//n = write(fd_bcack_fifo_rec, &bcbuf.buf, sizeof(GstBufferClassBuffer*));
	return n;
}

void deinit_pipes()
{
	close(fd_bcsink_fifo_rec);
	close(fd_bcack_fifo_rec);
}
