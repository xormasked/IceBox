#ifndef TEXTURES_CPP
#define TEXTURES_CPP

#include <impl/includes.hpp>

ID3D11Texture2D* textures_interface::c_textures::create_texture_from_file( ID3D11Device* device, const fs::path& path )
{
	( void )device;
	( void )path;
	return nullptr;
}

ID3D11Texture2D* textures_interface::c_textures::create_texture_from_memory( ID3D11Device* device, const LPCVOID& data, UINT size )
{
	( void )device;
	( void )data;
	( void )size;
	return nullptr;
}

ID3D11Texture2D* textures_interface::c_textures::get_texture_from_file( const std::string& name, const fs::path& path )
{
	std::unique_lock<std::shared_mutex> lock( mutex );

	auto it = texture_cache.find( name );
	if ( it != texture_cache.end( ) )
		return it->second;

	fs::path image_path = path / ( name + HASH_STR( ".png" ) );
	if ( !fs::exists( image_path ) )
		return nullptr;

	ID3D11Texture2D* texture = create_texture_from_file( render->g_pd3dDevice, image_path );
	if ( !texture )
		return nullptr;

	texture_cache.emplace( name, texture );
	return texture;
}

ID3D11Texture2D* textures_interface::c_textures::get_texture_from_memory( const std::string& name, const LPCVOID& data, UINT size )
{
	std::unique_lock<std::shared_mutex> lock( mutex );

	auto it = texture_cache.find( name );
	if ( it != texture_cache.end( ) )
		return it->second;

	ID3D11Texture2D* texture = create_texture_from_memory( render->g_pd3dDevice, data, size );
	if ( !texture )
		return nullptr;

	texture_cache.emplace( name, texture );
	return texture;
}

#endif // !TEXTURES_CPP