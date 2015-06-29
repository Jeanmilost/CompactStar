/*****************************************************************************
 * ==> vertex.glsl ----------------------------------------------------------*
 *****************************************************************************
 * Description : Vertex shader program                                       *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#version 150

// vertex buffer input
in vec3 qr_vPosition;

// uniform input
uniform mat4 qr_uModel;
uniform mat4 qr_uPerspective;
uniform mat4 qr_uCamera;

// output to fragment shader
out vec4 qr_fColor;

void main()
{
    // compute color per vertex
    qr_fColor = vec4(0.0, 0.0, 0.0, 1.0);

    // transform vertex coordinates
    gl_Position = qr_uPerspective * qr_uCamera * qr_uModel * vec4(qr_vPosition, 1);
}
