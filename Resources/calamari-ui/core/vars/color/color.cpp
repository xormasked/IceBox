#ifndef COLOR_CPP
#define COLOR_CPP

#include <impl/includes.hpp>

ImColor c_color::green_to_red( float factor )
{
    float r = ( factor >= 0.50f ) ?
        255.00f : 
        static_cast<u8>( 255.00f * factor / 0.50f );

    float g = ( factor >= 0.50f ) ? 
        static_cast<u8>( 255.00f - 255.00f * ( factor - 0.50f ) / 0.50f ) :
        255.00f;

    return ImColor( r / 255.f, g / 255.f, 0.0f, 255.0f );
}

#endif // !COLOR_CPP