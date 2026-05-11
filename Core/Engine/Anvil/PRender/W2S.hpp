#include "../scimitar.hpp"
#include "../../../Maths/havok_math.hpp"
#include "../../../Utils/memory.hpp"
#include "../../../Maths/vectors.hpp"



inline bool W2S( ubiVector3 position, ubiVector2& Screen )
{
    ubiVector3 temp = position - Scimitar::view_translation::get( )->get_view_translation( );
    float x = temp.Dot( Scimitar::view_translation::get( )->get_view_right( ) );
    float y = temp.Dot( Scimitar::view_translation::get( )->get_view_up( ) );
    float z = temp.Dot( Scimitar::view_translation::get( )->get_view_forward( ) * -1.f );

    int width = GetSystemMetrics( SM_CXSCREEN );
    int height = GetSystemMetrics( SM_CYSCREEN );

    Screen.x = ( width / 2.f ) * ( 1.f + x / Scimitar::view_translation::get( )->get_view_fovX( ) / z );
    Screen.y = ( height / 2.f ) * ( 1.f - y / Scimitar::view_translation::get( )->get_view_fovY( ) / z );

    return z >= 1.0f ? true : false;
}