/*****************************************************************************
 * ==> screenVertex.glsl ----------------------------------------------------*
 *****************************************************************************
 * Description : Vertex shader program for post-processing effects           *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#version 330 core

// memory input
layout (location = 0) in vec2 qr_Position;
layout (location = 1) in vec2 qr_UV;

// output to fragment shader
out vec2 qr_TexCoords;

void main()
{
    // compute final vertex position (static, because final render surface is
    // only a square always shown on the same location on the screen)
    gl_Position = vec4(qr_Position.x, qr_Position.y, 0.0f, 1.0f);

    // send texture position to fragment shader
    qr_TexCoords = qr_UV;
}
