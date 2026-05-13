#ifndef FONTS_HPP
#define FONTS_HPP

namespace fonts_interface {

	class c_fonts {
	public:
		c_fonts( ) = default;
		~c_fonts( ) = default;

	private:
		std::unordered_map<std::string, std::unordered_map<float, ImFont*>> fonts;

	public:
		bool add( const std::string& filename, std::vector<float> size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges );
		bool add( const std::string& name, void* font_data, int font_data_size, std::vector<float> size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges );

	public:
		ImFont* get( const std::string& name, float size );

		void clear_cached_fonts( )
		{
			fonts.clear( );
		}
	};

} inline auto fonts = std::make_shared<fonts_interface::c_fonts>( );

#endif // !FONTS_HPP
