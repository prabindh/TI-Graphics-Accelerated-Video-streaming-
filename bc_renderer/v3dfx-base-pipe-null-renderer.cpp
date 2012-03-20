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

#include "v3dfx-base-pipe-renderer.h"

/******************************************************************************
 Defines
******************************************************************************/

static int init_view();
static void release_view();
static int setScene();

int fd_bcsink_fifo_rec;
int fd_bcinit_fifo_rec;
int fd_bcack_fifo_rec;

EGLDisplay dpy;
EGLSurface surface = EGL_NO_SURFACE;
static EGLContext context = EGL_NO_CONTEXT;

static void dump_egl_error(char *name)
{
    char *err_str[] = {
          "EGL_SUCCESS",
          "EGL_NOT_INITIALIZED",
          "EGL_BAD_ACCESS",
          "EGL_BAD_ALLOC",
          "EGL_BAD_ATTRIBUTE",    
          "EGL_BAD_CONFIG",
          "EGL_BAD_CONTEXT",   
          "EGL_BAD_CURRENT_SURFACE",
          "EGL_BAD_DISPLAY",
          "EGL_BAD_MATCH",
          "EGL_BAD_NATIVE_PIXMAP",
          "EGL_BAD_NATIVE_WINDOW",
          "EGL_BAD_PARAMETER",
          "EGL_BAD_SURFACE" };

    EGLint ecode = eglGetError();

    printf("'%s': egl error '%s' (0x%x)\n",
           name, err_str[ecode-EGL_SUCCESS], ecode);
}

void deInitEGL()
{

    eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT)
        eglDestroyContext(dpy, context);
    if (surface != EGL_NO_SURFACE)
        eglDestroySurface(dpy, surface);
    eglTerminate(dpy);
}

int initEGL(int *surf_w, int *surf_h, int profile)
{

    EGLint  context_attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    EGLint            disp_w, disp_h;
    EGLNativeDisplayType disp_type;
    EGLNativeWindowType  window;
    EGLConfig         cfgs[2];
    EGLint            n_cfgs;
    EGLint            egl_attr[] = {
                         EGL_BUFFER_SIZE, EGL_DONT_CARE,
                         EGL_RED_SIZE,    8,
                         EGL_GREEN_SIZE,  8,
                         EGL_BLUE_SIZE,   8,
                         EGL_DEPTH_SIZE,  8,

                         EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  
                         EGL_NONE };

    disp_type = (EGLNativeDisplayType)EGL_DEFAULT_DISPLAY;
    window  = 0;

    dpy = eglGetDisplay(disp_type);

    if (eglInitialize(dpy, NULL, NULL) != EGL_TRUE) {
        dump_egl_error("eglInitialize");
        return -1;
    }

    if (eglGetConfigs(dpy, cfgs, 2, &n_cfgs) != EGL_TRUE) {
        dump_egl_error("eglGetConfigs");
        goto cleanup;
    }
    
    if (eglChooseConfig(dpy, egl_attr, cfgs, 2, &n_cfgs) != EGL_TRUE) {
        dump_egl_error("eglChooseConfig");
        goto cleanup;
    }

    surface = eglCreateWindowSurface(dpy, cfgs[0], window, NULL);
    if (surface == EGL_NO_SURFACE) {
        dump_egl_error("eglCreateWindowSurface");
        goto cleanup;
    }

	//Use this rather than fb info, as this is generic
	eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &surf_w);
	eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &surf_h);

	
    if (surf_w && surf_h) {
        *surf_w = disp_w;
        *surf_h = disp_h;
    }

    context = eglCreateContext(dpy, cfgs[0], EGL_NO_CONTEXT, context_attr);
    
    if (context == EGL_NO_CONTEXT) {
		//TODO - try with 16bpp configuration, then only exit
        dump_egl_error("eglCreateContext");
        goto cleanup;
    }

    if (eglMakeCurrent(dpy, surface, surface, context) != EGL_TRUE) {
        dump_egl_error("eglMakeCurrent");
        goto cleanup;
    }

    /* do not sync with video frame if profile enabled */
    if (profile == 1) {
        if (eglSwapInterval(dpy, 0) != EGL_TRUE) {
            dump_egl_error("eglSwapInterval");
            goto cleanup;
        }
    }
    return 0;

cleanup:
    deInitEGL();
    return -1;
}

bc_gstpacket bcbuf_receive, bcbuf;

TISGXStreamTexIMGSTREAM* texClass;
TISGXStreamIMGSTREAMDevice* deviceClass;
imgstream_device_attributes initAttrib = {inTextureWidth, inTextureHeight, 2,
inTextureWidth*inTextureHeight*2,
PVRSRV_PIXEL_FORMAT_UYVY,
2 //2 //2 buffers used
};
int lastDeviceClass = 0;

unsigned long *paArray;
unsigned long *freeArray; 

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

/*!****************************************************************************
 @Function		release_view
******************************************************************************/
void release_view()
{
	deInitEGL();
	
	// Frees the OpenGL handles for the program and the 2 shaders
	deviceClass->destroy();
}

/*!****************************************************************************
 @Function		set mvp matrix - fixed value in this case
******************************************************************************/
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

/******************************************************************************
******************************************************************************/

/* Vertices for rectagle covering the entire display resolution */
GLfloat rect_vertices[6][3] =
{   // x     y     z
 
   /* 1st Traingle */
    {-1.0,  1.0,  0.0}, // 0 
    {-1.0, -1.0,  0.0}, // 1
    { 1.0,  1.0,  0.0}, // 2
  
   /* 2nd Traingle */
    { 1.0,  1.0,  0.0}, // 1
    {-1.0, -1.0,  0.0}, // 0
    { 1.0, -1.0,  0.0}, // 2
};

GLfloat rect_vertices0[6][3] =
{   // x     y     z
 
   /* 1st Traingle */
    {-1.0,  1.0,  0.0}, // 0 
    {-1.0,  0.0,  0.0}, // 1
    { 0.0,  1.0,  0.0}, // 2
  
   /* 2nd Traingle */
    { 0.0,  1.0,  0.0}, // 1
    {-1.0, -0.0,  0.0}, // 0
    { 0.0, -0.0,  0.0}, // 2
};

GLfloat rect_vertices1[6][3] =
{   // x     y     z
 
   /* 1st Traingle */
    {-0.0,  1.0,  0.0}, // 0 
    {-0.0, -0.0,  0.0}, // 1
    { 1.0,  1.0,  0.0}, // 2
  
   /* 2nd Traingle */
    { 1.0,  1.0,  0.0}, // 1
    {-0.0, -0.0,  0.0}, // 0
    { 1.0, -0.0,  0.0}, // 2
};

GLfloat rect_vertices2[6][3] =
{   // x     y     z
 
   /* 1st Traingle */
    {-1.0,  0.0,  0.0}, // 0 
    {-1.0, -1.0,  0.0}, // 1
    { 0.0,  0.0,  0.0}, // 2
  
   /* 2nd Traingle */
    { 0.0,  0.0,  0.0}, // 1
    {-1.0, -1.0,  0.0}, // 0
    { 0.0, -1.0,  0.0}, // 2
};

GLfloat rect_vertices3[6][3] =
{   // x     y     z
 
   /* 1st Traingle */
    {-0.0,  0.0,  0.0}, // 0 
    {-0.0, -1.0,  0.0}, // 1
    { 1.0,  0.0,  0.0}, // 2
  
   /* 2nd Traingle */
    { 1.0,  0.0,  0.0}, // 1
    {-0.0, -1.0,  0.0}, // 0
    { 1.0, -1.0,  0.0}, // 2
};

/* Texture Co-ordinates */
GLfloat rect_texcoord[6][2] =
{   // x     y     z  alpha

   /* 1st Traingle */
    { 0.0, 0.0},
    { 0.0, 1.0},
    { 1.0, 0.0},

   /* 2nd Traingle */
    { 1.0,  0.0}, 
    { 0.0,  1.0},
    { 1.0,  1.0},

};

/* Texture Co-ordinates */
GLfloat rect_texcoord1[6][2] =
{   // x     y     z  alpha

   /* 1st Traingle */
    { 0.0, 0.0},
    { 0.0, 1.0},
    { 1.0, 1.0},

   /* 2nd Traingle */
    { 1.0,  0.0}, 
    { 0.0,  1.0},
    { 1.0,  1.0},

};

/* Draws rectangiular quads */
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
		goto exitEGL;
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
	physicalAddressBase = mem_cmem_alloc(	chunkSize 	);
	paArray = (unsigned long*)malloc(initAttrib.numBuffers * sizeof(unsigned long));
	freeArray = (unsigned long*)malloc(initAttrib.numBuffers * sizeof(unsigned long));	
	if(!paArray || !freeArray) {goto exitPipes;}

	/*************************************************************************************
	**************************************************************************************/
	for(count = 0; count < initAttrib.numBuffers; count++)
	{
		paArray[count]  = physicalAddressBase + chunkSize;
		freeArray[count] = 0;
	}
	//TODO - give the buffers back to requestor via answer
	//write_init_buffer_pipe();
	
	init_view();

	while(read_pipe() != -1)
	{
		render(bcbuf.index);
		
		if( write_pipe() != sizeof(GstBufferClassBuffer *))
		{	
			printf("Error Writing into Init Queue\n");
			goto exit;
		}
	}

exit:
	release_view();
exitPipes:	
	deinit_pipes();	
exitEGL:
	return 0;
}
