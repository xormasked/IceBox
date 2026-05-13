#ifndef SPRITES_CPP
#define SPRITES_CPP

#include <impl/includes.hpp>

sprites_interface::c_sprite::c_sprite( )
{

}

auto sprites_interface::c_sprite::get( const std::string& item_name ) -> sprite_data
{
	auto it = sprites_cache.find( item_name );
	if ( it == sprites_cache.end( ) )
		return { };

	sprite_data out;

	out.texture = textures->get_texture_from_memory(
		item_name,
		it->second.data,
		it->second.size
	);

	out.icon_size = it->second.icon_size;

	return out;
}

#endif // SPRITES_CPP