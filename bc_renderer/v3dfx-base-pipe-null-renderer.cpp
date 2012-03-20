/******************************************************************************
*****************************************************************************
 * v3dfx-base-pipe-renderer.c
 * v3dfx-base renderer with pipe interface and draws to
 *     NULL window system - Implementation file
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

#include "v3dfx_imgstream.h"
#include <sys/time.h>
#include <cmem.h>
#include <getopt.h>
#include <math.h>
#include <gst/gst.h>
#include <linux/fb.h>
#include <pthread.h>

#include "vertex-info.h"

/******************************************************************************
Globals
******************************************************************************/
int fd_bcsink_fifo_rec;
int fd_bcinit_fifo_rec;
int fd_bcack_fifo_rec;

bc_gstpacket bcbuf_receive, bcbuf;

TISGXStreamTexIMGSTREAM* texClass;
TISGXStreamIMGSTREAMDevice* deviceClass;
imgstream_device_attributes initAttrib;
int lastDeviceClass = 0;

unsigned long *paArray;
unsigned long *freeArray; 
void* virtualAddress; 
int physicalAddress;

extern void deInitEGL();
extern int initEGL(int *surf_w, int *surf_h, int profile);
void mem_cmem_init();
int mem_cmem_alloc(int numBytes, void**virtualAddress, int* physicalAddress);
int mem_cmem_free(void* virtualAddress);
void mem_cmem_deinit();


// Primary function that sets up v3dfx-base
int init_view()
{
	int matrixLocation;
	deviceClass = new TISGXStreamIMGSTREAMDevice();
	texClass = new TISGXStreamTexIMGSTREAM();

	deviceClass->init(&tempAttrib, lastDeviceClass, paArray);
	texClass->init(lastDeviceClass);
	texClass->load_v_shader(NULL);
	texClass->load_f_shader(NULL);
	texClass->load_program();	

	matrixLocation = texClass->get_uniform_location("MVPMatrix");
	set_mvp(matrixLocation);	
	
	return 1;
}

// Destroys the v3dfx-base classes
void release_view()
{	
	// Frees the OpenGL handles for the program and the 2 shaders
	deviceClass->destroy();
}

//Helper functions
int set_mvp(int location)
{
	int m_fAngle = 0;

	 /*
                Bind the projection model view matrix (PMVMatrix) to the
                corresponding uniform variable in the shader.
                This matrix is used in the vertex shader to transform the vertices.
        */

	 float aPMVMatrix[] =
        {
                cos(m_fAngle),  0,      sin(m_fAngle),  0,
                0,              1,      0,              0,
                -sin(m_fAngle), 0,      cos(m_fAngle),  0,
                0,              0,      0,              1
        };

	// Then passes the matrix to that variable
	glUniformMatrix4fv(location, 1, GL_FALSE, aPMVMatrix);
	return 1;
}

//Draws rectangiular quads
void drawRect(int isfullscreen)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

   if(isfullscreen)
   {	 
	   /* Draw Quad-1 */
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, rect_vertices);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, rect_texcoord);
	    glDrawArrays(GL_TRIANGLES, 0, 6);
   }
   else
   {
	   /* Draw Quad-1 */
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, rect_vertices0);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, rect_texcoord);
	    glDrawArrays(GL_TRIANGLES, 0, 6);

	     /* Draw Quad-2 */
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, rect_vertices1);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, rect_texcoord);
	    glDrawArrays(GL_TRIANGLES, 0, 6);

	      /* Draw Quad-3 */
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, rect_vertices2);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, rect_texcoord);
	    glDrawArrays(GL_TRIANGLES, 0, 6);

	       /* Draw Quad-4 */
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, rect_vertices3);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, rect_texcoord);
	    glDrawArrays(GL_TRIANGLES, 0, 6);
   }
    glDisableVertexAttribArray (0);
    glDisableVertexAttribArray (1);

}


//Render video on the triangles
void render(int buf_index)
{
    GLuint tex_obj;
    static  struct timeval tvp, tv;
    unsigned long tdiff = 0;
    static int fcount = 0;

	deviceClass->qTexImage2DBuf(paArray);
	//TODO - need to wait, but not doing now
		//deviceClass->signal_draw(); 
		//deviceClass->dqTexImage2DBuf(freeArray);
	
	// Pass 1 for full screen and 0 for quad mode
	drawRect(0);
	eglSwapBuffers(dpy, surface);
	gettimeofday(&tv, NULL);
	fcount++;
	if (fcount == 1000) {
		tdiff = (unsigned long)(tv.tv_sec*1000 + tv.tv_usec/1000 -
							tvp.tv_sec*1000 - tvp.tv_usec/1000);
		printf("Frame Rate: %ld \n", (1000*1000)/tdiff);
		gettimeofday(&tvp, NULL);
		fcount = 0;
	}
}


// Main function with event loop
int main(void)
{
	int n = -1;
	int count = 0;
	static bc_buf_ptr_t buf_pa;
	int dev_fd;

	printf("Initializing egl..\n\n");
	if( 0 == initEGL())
	{	
		printf("EGL init failed");
		goto exitNone;
	}
	
	//Also initialise the pipes
	n = initPipes(&initAttrib);
	if(n) 	{ 		goto exitPipes; 	}
	
	if(mem_cmem_init()) {	goto exitPipes;	}
	//Allocate mem
	chunkSize = initAttrib.widthPixels* 
							initAttrib.heightPixels* 
							initAttrib.bytesPerPixel*  
							initAttrib.numBuffers;
	if(mem_cmem_alloc( chunkSize, &virtualAddress, physicalAddress )) {goto exitCMEMInit;}

	paArray = (unsigned long*)malloc(initAttrib.numBuffers * sizeof(unsigned long));
	freeArray = (unsigned long*)malloc(initAttrib.numBuffers * sizeof(unsigned long));	
	if(!paArray || !freeArray) {goto exitCMEMAlloc;}

	for(count = 0; count < initAttrib.numBuffers; count++)
	{
		paArray[count]  = physicalAddressBase + count*(chunkSize/initAttrib.numBuffers);
		freeArray[count] = 0;
	}
	//TODO - give the allocated buffers back to requestor via answer
	//write_init_buffer_pipe();
	
	init_view();

	//Loop reading new data and rendering, till something happens
	//TODO - exit cleanly using last msg
	while(read_pipe() != -1)
	{
		render(bcbuf.index);
		
		if( write_pipe() != sizeof(GstBufferClassBuffer *))
		{	
			printf("Error Writing into Init Queue\n");
			//TODO - try again n times ?
			break;
		}
	}
exit:
	release_view();
	deInitEGL();
exitCMEMAlloc:
	mem_cmem_free( virtualAddress);
exitPipes:	
	if(paArray) free(paArray);
	if(freeArray) free(freeArray);
	deinit_pipes();
exitCMEMInit:
	mem_cmem_deinit();
exitNone:
	return 0;
}
