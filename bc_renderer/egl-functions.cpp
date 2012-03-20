#include <stdio.h>
#include <EGL/egl.h>

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
