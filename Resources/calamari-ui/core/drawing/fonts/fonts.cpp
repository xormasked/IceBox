#ifndef FONTS_CPP
#define FONTS_CPP

#include <impl/includes.hpp>

bool fonts_interface::c_fonts::add( const std::string& filename, std::vector<float> size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges )
{
	for ( const auto& size : size_pixels )
	{
		ImFont* font = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( filename.c_str( ), size, font_cfg_template, glyph_ranges );
		if ( !font )
			continue;

		fonts [ filename ][ size ] = font;
	}

	return true;
}

bool fonts_interface::c_fonts::add( const std::string& name, void* font_data, int font_data_size, std::vector<float> size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges )
{
	for ( const auto& size : size_pixels )
	{
		ImFont* font = ImGui::GetIO( ).Fonts->AddFontFromMemoryTTF( font_data, font_data_size, size, font_cfg_template, glyph_ranges );
		if ( !font )
			continue;

		fonts [ name ][ size ] = font;
	}

	return true;
}

ImFont* fonts_interface::c_fonts::get( const std::string& name, float size )
{
	auto it = fonts.find( name );
	if ( it == fonts.end( ) )
		return nullptr;

	const auto& sizes = it->second;

	auto it_size = sizes.find( size );
	if ( it_size == sizes.end( ) )
		return nullptr;

	return it_size->second;
}

#endif // !FONTS_CPP