#include "v3dfx_imgstream.h"

#include "gst_render_bridge.h"

// Size of the texture we create
#define BCSINK_FIFO_NAME "gstbcsink_fifo"
#define BCINIT_FIFO_NAME "gstbcinit_fifo"
#define BCACK_FIFO_NAME "gstbcack_fifo"


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
			goto exit;
		}

		fd_bcsink_fifo_rec = open( BCSINK_FIFO_NAME, O_RDONLY );
		if(fd_bcsink_fifo_rec < 0)
		{
			printf (" Failed to open bcsink_fifo FIFO - fd: %d\n", fd_bcsink_fifo_rec);
			goto exit;
		}

		fd_bcack_fifo_rec = open( BCACK_FIFO_NAME, O_WRONLY);
		if(fd_bcack_fifo_rec < 0)
		{
			printf (" Failed to open bcack_fifo FIFO - fd: %d\n", fd_bcack_fifo_rec);
			goto exit;
		}

        n = read(fd_bcinit_fifo_rec, initparams, sizeof(imgstream_device_attributes));

		close(fd_bcinit_fifo_rec);
		
		return n;
}

int read_pipe()
{
	int n;
	n = read(fd_bcsink_fifo_rec, &bcbuf, sizeof(bc_gstpacket));
	return n;
}

int write_pipe()
{
	int n;
	n = write(fd_bcack_fifo_rec, &bcbuf.buf, sizeof(GstBufferClassBuffer*));
	return n;
}

void deinit_pipes()
{
	close(fd_bcsink_fifo_rec);
	close(fd_bcack_fifo_rec);
}