#ifndef HELP_MANAGER_CPP
#define HELP_MANAGER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

int rotation_start_index;
void HelpManager::rotate_start( )
{
    rotation_start_index = GetWindowDrawList( )->VtxBuffer.Size;
}

ImVec2 HelpManager::rotation_center( )
{
    ImVec2 l( FLT_MAX, FLT_MAX ), u( -FLT_MAX, -FLT_MAX );

    const auto& buf = GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        l = ImMin( l, buf [ i ].pos ), u = ImMax( u, buf [ i ].pos );

    return ImVec2( ( l.x + u.x ) / 2, ( l.y + u.y ) / 2 );
}

void HelpManager::rotate_end( float rad, ImVec2 center )
{
    float s = sin( rad ), c = cos( rad );
    center = ImRotate( center, s, c ) - center;

    auto& buf = GetWindowDrawList( )->VtxBuffer;
    for ( int i = rotation_start_index; i < buf.Size; i++ )
        buf [ i ].pos = ImRotate( buf [ i ].pos, s, c ) - center;
}

#endif // !HELP_MANAGER_CPP
