#ifndef FRAMEWORK_CPP
#define FRAMEWORK_CPP

#include <impl/includes.hpp>

c_color framework_interface::c_framework::get_gradient_color( int current, int max )
{
	if ( current < 0 ) current = 0;
	if ( current > max ) current = max;

	float normalized = static_cast< float >( current ) / static_cast< float >( max );

	int r, g, b;

	if ( normalized <= 0.33f ) 
	{
		float t = normalized / 0.33f;
		r = 255;
		g = static_cast< int >( 128 * t );
		b = 0;
	}
	else if ( normalized <= 0.66f ) 
	{
		float t = ( normalized - 0.33f ) / 0.33f;
		r = 255;
		g = static_cast< int >( 128 + 127 * t );
		b = 0;
	}
	else 
	{
		float t = ( normalized - 0.66f ) / 0.34f;
		r = static_cast< int >( 255 * ( 1 - t ) );
		g = 255;
		b = 0;
	}

	return c_color( r, g, b, 255 );
}

void framework_interface::c_framework::text( const std::string& text, ImVec2 position, float size, ImColor color, ImDrawList* ImDrawList, size_t max_symbols )
{
    ImColor shadow_col = ImColor( 0.00f, 0.00f, 0.00f, color.Value.w * 0.50f );
    ImVec2 shadow_offset = ImVec2( 1.00f, 1.00f );

	size_t symbols = std::min( text.size( ), max_symbols );

    ImDrawList->AddText( ImGui::GetFont( ), size, position + shadow_offset, shadow_col, text.c_str( ), text.c_str( ) + text.size( ) );
    ImDrawList->AddText( ImGui::GetFont( ), size, position, color, text.c_str( ), text.c_str( ) + text.size( ) );
}

void framework_interface::c_framework::draw_line( ImVec2 start, ImVec2 end, ImColor color, float thickness, ImDrawList* ImDrawList )
{
    ImDrawList->AddLine( start, end, color, thickness );
}

void framework_interface::c_framework::draw_3d_box( const std::vector<math::vector2>& points, ImColor color, float thickness, ImDrawList* ImDrawList )
{
	ImDrawList->AddLine( ImVec2( points [ 0 ].x, points [ 0 ].y ), ImVec2( points [ 1 ].x, points [ 1 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 1 ].x, points [ 1 ].y ), ImVec2( points [ 2 ].x, points [ 2 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 2 ].x, points [ 2 ].y ), ImVec2( points [ 3 ].x, points [ 3 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 3 ].x, points [ 3 ].y ), ImVec2( points [ 0 ].x, points [ 0 ].y ), color, thickness );

	ImDrawList->AddLine( ImVec2( points [ 4 ].x, points [ 4 ].y ), ImVec2( points [ 5 ].x, points [ 5 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 5 ].x, points [ 5 ].y ), ImVec2( points [ 6 ].x, points [ 6 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 6 ].x, points [ 6 ].y ), ImVec2( points [ 7 ].x, points [ 7 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 7 ].x, points [ 7 ].y ), ImVec2( points [ 4 ].x, points [ 4 ].y ), color, thickness );

	ImDrawList->AddLine( ImVec2( points [ 0 ].x, points [ 0 ].y ), ImVec2( points [ 4 ].x, points [ 4 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 1 ].x, points [ 1 ].y ), ImVec2( points [ 5 ].x, points [ 5 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 2 ].x, points [ 2 ].y ), ImVec2( points [ 6 ].x, points [ 6 ].y ), color, thickness );
	ImDrawList->AddLine( ImVec2( points [ 3 ].x, points [ 3 ].y ), ImVec2( points [ 7 ].x, points [ 7 ].y ), color, thickness );
}

void framework_interface::c_framework::draw_2d_box( ImVec2 position, ImVec2 size, ImColor color, float thickness, ImDrawList* ImDrawList )
{
	ImDrawList->AddRect( position, ImVec2( position.x + size.x, position.y + size.y ), color, 0.00f, 0, thickness );
}

void framework_interface::c_framework::draw_cornered_box( ImVec2 position, ImVec2 size, ImColor color, float thickness, ImDrawList* ImDrawList )
{
    const float w = size.x;
    const float h = size.y;

    const float cw = w / 3.0f;
    const float ch = h / 3.0f;

    ImDrawList->AddRectFilled( ImVec2( position.x, position.y ), ImVec2( position.x + cw, position.y + thickness ), color );
    ImDrawList->AddRectFilled( ImVec2( position.x, position.y ), ImVec2( position.x + thickness, position.y + ch ), color );

    ImDrawList->AddRectFilled( ImVec2( position.x + w - cw, position.y ), ImVec2( position.x + w, position.y + thickness ), color );
    ImDrawList->AddRectFilled( ImVec2( position.x + w - thickness, position.y ), ImVec2( position.x + w, position.y + ch ), color );

    ImDrawList->AddRectFilled( ImVec2( position.x, position.y + h - thickness ), ImVec2( position.x + cw, position.y + h ), color );
    ImDrawList->AddRectFilled( ImVec2( position.x, position.y + h - ch ), ImVec2( position.x + thickness, position.y + h ), color );

    ImDrawList->AddRectFilled( ImVec2( position.x + w - cw, position.y + h - thickness ), ImVec2( position.x + w, position.y + h ), color );
    ImDrawList->AddRectFilled( ImVec2( position.x + w - thickness, position.y + h - ch ), ImVec2( position.x + w, position.y + h ), color );
}

void framework_interface::c_framework::draw_triangle_fill( ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color, ImDrawList* ImDrawList )
{
	ImDrawList->AddTriangleFilled( p1, p2, p3, color );
}

void framework_interface::c_framework::draw_triangle( ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor color, float thickness, ImDrawList* ImDrawList )
{
    ImDrawList->AddTriangle( p1, p2, p3, color, thickness );
}

void framework_interface::c_framework::draw_circle( ImVec2 center, float radius, ImColor color, float thickness, ImDrawList* ImDrawList )
{
	ImDrawList->AddCircle( center, radius, color, static_cast<int>( radius ), thickness );
}

void framework_interface::c_framework::add_image( ImTextureRef tex_ref, ImVec2 position, ImVec2 size, ImDrawList* ImDrawList )
{
	ImDrawList->AddImage( tex_ref, position, position + size );
}

void framework_interface::c_framework::draw_health_bar( u64 player, ImVec2 position, ImVec2 size, float health, float max, float thickness, healthbar_orientation orientation, healthbar_direction direction, ImDrawList* ImDrawList )
{
	static std::unordered_map<u64, float> animated_values;
	static std::unordered_map<u64, float> last_update_time;

	float current_time = ImGui::GetTime( );

	if ( animated_values.find( player ) == animated_values.end( ) )
	{
		animated_values [ player ] = health;
		last_update_time [ player ] = current_time;
	}

	float& animated_health = animated_values [ player ];
	float& last_time = last_update_time [ player ];

	const float animation_speed = 5.0f;

	float delta_time = current_time - last_time;
	last_time = current_time;

	if ( animated_health > health )
	{
		float difference = animated_health - health;
		animated_health -= difference * animation_speed * delta_time;

		if ( animated_health < health )
			animated_health = health;
	}
	else
	{
		animated_health = health;
	}

	const auto clamped_current = std::clamp( health, 0.0f, max );
	const auto clamped_animated = std::clamp( animated_health, 0.0f, max );

	const auto proportion = clamped_animated / max;

	const float outline_offset = 1.0f;
	const float bar_gap = 3.5f;

	const auto fill_color = get_gradient_color( clamped_current, max ).to_im_color( );

	if ( orientation == healthbar_orientation::vertical )
	{
		float bar_width = thickness;
		float bar_height = size.y;

		float bar_x = position.x - bar_width - bar_gap;
		float bar_y = position.y;

		ImDrawList->AddRectFilled( ImVec2( bar_x - outline_offset, bar_y - outline_offset ), ImVec2( bar_x + bar_width + outline_offset, bar_y + bar_height + outline_offset ), ImColor( 0, 0, 0, 255 ) );

		float fill_height = bar_height * proportion;

		float fill_y = ( direction == healthbar_direction::normal )
			? bar_y + bar_height - fill_height
			: bar_y;

		ImDrawList->AddRectFilled( ImVec2( bar_x, fill_y ), ImVec2( bar_x + bar_width, fill_y + fill_height ), fill_color );
	}
	else
	{
		float bar_width = size.x;
		float bar_height = thickness;

		float bar_x = position.x;
		float bar_y = position.y;

		ImDrawList->AddRectFilled( ImVec2( bar_x - outline_offset, bar_y - outline_offset ), ImVec2( bar_x + bar_width + outline_offset, bar_y + bar_height + outline_offset ), ImColor( 0, 0, 0, 255 ) );

		float fill_width = bar_width * proportion;

		float fill_x = ( direction == healthbar_direction::normal )
			? bar_x
			: bar_x + bar_width - fill_width;

		ImDrawList->AddRectFilled( ImVec2( fill_x, bar_y ), ImVec2( fill_x + fill_width, bar_y + bar_height ), fill_color );
	}

}

bool framework_interface::c_framework::drag_component( ImVec2& position, ImVec2 size, const std::string& name )
{
	static std::unordered_map<std::string, bool> is_dragging_map;
	static std::unordered_map<std::string, ImVec2> drag_offset_map;

	ImVec2 component_min = ImVec2( position.x - size.x / 2.0f, position.y - size.y / 2.0f );
	ImVec2 component_max = ImVec2( position.x + size.x / 2.0f, position.y + size.y / 2.0f );

	ImVec2 mouse_pos = ImGui::GetMousePos( );
	bool mouse_over_component = mouse_pos.x >= component_min.x && mouse_pos.x <= component_max.x &&
								mouse_pos.y >= component_min.y && mouse_pos.y <= component_max.y;

	bool& is_dragging = is_dragging_map[ name ];
	ImVec2& drag_offset = drag_offset_map[ name ];

	if ( ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
	{
		if ( mouse_over_component || is_dragging )
		{
			if ( !is_dragging )
			{
				is_dragging = true;
				drag_offset = ImVec2( mouse_pos.x - position.x, mouse_pos.y - position.y );
			}
			
			if ( is_dragging )
			{
				position.x = mouse_pos.x - drag_offset.x;
				position.y = mouse_pos.y - drag_offset.y;
			}

			return true;
		}
	}
	else
	{
		is_dragging = false;
	}

	return false;
}

#endif
