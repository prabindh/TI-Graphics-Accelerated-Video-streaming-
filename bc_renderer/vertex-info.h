/******************************************************************************
*****************************************************************************
 * Vertex info
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


//Vertices for rectangle covering the entire display resolution - this is for example only.
// Any layout is possible

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


