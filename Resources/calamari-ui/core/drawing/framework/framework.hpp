#ifndef FRAMEWORK_HPP
#define FRAMEWORK_HPP

namespace framework_interface {

	enum class healthbar_orientation : u8 {
		vertical,
		horizontal
	};

	enum class healthbar_direction : u8 {
		normal,
		reverse
	};

	class c_framework {
	public:
		c_framework( ) = default;
		~c_framework( ) = default;

	public:
		c_color get_gradient_color( int current, int max );

		void text( const std::string& text, ImVec2 position, float size, ImColor color, ImDrawList* ImDrawList, size_t max_symbols = 255 );

		void draw_line( ImVec2 start, ImVec2 end, ImColor color, float thickness, ImDrawList* ImDrawList );

		void draw_3d_box( const std::vector<math::vector2>& points, ImColor color, float thickness, ImDrawList* ImDrawList );
		void draw_2d_box( ImVec2 position, ImVec2 size, ImColor color, float thickness, ImDrawList* ImDrawList );
		void draw_cornered_box( ImVec2 position, ImVec2 size, ImColor color, float thickness, ImDrawList* ImDrawList );

		void draw_triangle_fill( ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color, ImDrawList* ImDrawList );
		void draw_triangle( ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color, float thickness, ImDrawList* ImDrawList );

		void draw_circle( ImVec2 center, float radius, ImColor color, float thickness, ImDrawList* ImDrawList );

		void add_image( ImTextureRef tex_ref, ImVec2 position, ImVec2 size, ImDrawList* ImDrawList );

		void draw_health_bar( u64 player, ImVec2 position, ImVec2 size, float health, float max, float thickness, healthbar_orientation orientation, healthbar_direction direction, ImDrawList* ImDrawList );
		bool drag_component( ImVec2& position, ImVec2 size, const std::string& name );
	};

} inline const auto framework = std::make_shared<framework_interface::c_framework>( );

#endif // ! guard