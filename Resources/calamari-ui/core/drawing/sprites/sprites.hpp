#ifndef SPRITES_HPP
#define SPRITES_HPP

namespace sprites_interface {

	struct sprite_data {
		LPCVOID data;
		UINT size;

		ImVec2 icon_size;
		ID3D11Texture2D* texture;
	};

	class c_sprite {
	public:
		c_sprite( );
		~c_sprite( ) = default;

	private:
		std::unordered_map<std::string, sprite_data> sprites_cache;

	public:
		sprite_data get( const std::string& item_name );
	};

} inline auto sprites = std::make_shared<sprites_interface::c_sprite>( );

#endif // !SPRITES_HPP