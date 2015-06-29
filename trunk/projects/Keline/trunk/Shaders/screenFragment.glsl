/*****************************************************************************
 * ==> screenFragment.glsl --------------------------------------------------*
 *****************************************************************************
 * Description : Fragment shader program for post-processing effects         *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#version 150

// uniform input
uniform sampler2D qr_ScreenTexture;

// input from vertex program
in vec2 qr_TexCoords;

void main()
{
    // compute final color from multi-sampled texture
    gl_FragColor = texture(qr_ScreenTexture, qr_TexCoords);
}
