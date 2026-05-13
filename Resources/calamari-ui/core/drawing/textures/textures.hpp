#ifndef TEXTURES_HPP
#define TEXTURES_HPP

namespace textures_interface {

	class c_textures {
	public:
		c_textures( ) = default;
		~c_textures( ) = default;

	private:
		std::shared_mutex mutex;
		std::unordered_map<std::string, ID3D11Texture2D*> texture_cache;

	public:
		ID3D11Texture2D* create_texture_from_file( ID3D11Device* device, const fs::path& path );
		ID3D11Texture2D* create_texture_from_memory( ID3D11Device* device, const LPCVOID& data, UINT size );

		ID3D11Texture2D* get_texture_from_file( const std::string& name, const fs::path& path );
		ID3D11Texture2D* get_texture_from_memory( const std::string& name, const LPCVOID& data, UINT size );
	};

} inline auto textures = std::make_shared<textures_interface::c_textures>( );

#endif // !TEXTURES_HPP