#ifndef COMPONENT_BUILDER_CPP
#define COMPONENT_BUILDER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

void CompBuilder::Empty( const char* label, ImRect total_bb, ImRect bb, std::function< void( const EmptyEnv& ) > code ) {
	ImGuiWindow* window = GetCurrentWindow( );
	auto id = window->GetID( label );

	ItemSize( total_bb );
	ItemAdd( total_bb, id );

	bool hovered, held;
	bool pressed = ButtonBehavior( bb, id, &hovered, &held );

	code( EmptyEnv {
		id,
		hovered,
		held,
		pressed,
		LangManager::get( ).translate( label ).c_str( )
		} );
}

bool CompBuilder::Button( const char* label, ImVec2 size, std::function< void( const ButtonEnv& ) > code ) {
	ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + CalcItemSize( size, CalcItemWidth( ), GetFrameHeight( ) ) };
	bool pressed;

	Empty( label, bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
		}; auto& obj = anim_obj( label, 4431423, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || env.held );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );

		code( ButtonEnv {
			bb,
			env.hovered,
			env.held,
			env.label,
			ButtonEnv::Anim {
				obj.hover,
				obj.held,
				obj.anim
			}
			} );

		pressed = env.pressed;
		} );

	return pressed;
}

template < typename T >
bool CompBuilder::Slider( const char* label, T* v, T min, T max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code ) {
	bool result = false;

	Empty( label, total_bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float val_anim;
			char buf [ 64 ];
		}; auto& obj = anim_obj( label, 032, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || env.held );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );

		obj.val_anim = ImClamp( ImLerp( obj.val_anim, ( ImClamp( *v, min, max ) - min * 1.f ) / ( max - min ) * bb.GetWidth( ), GetIO( ).DeltaTime * 17 ), 0.00f, ( ImClamp( *v, min, max ) - min * 1.f ) / ( max - min ) * bb.GetWidth( ) );

		if ( env.held ) {
			*v = ImClamp( T( min + ( GetIO( ).MousePos.x - bb.Min.x ) / bb.GetWidth( ) * ( max - min ) ), min, max );
			result = true;
		}

		ImFormatString( obj.buf, sizeof( obj.buf ), format, *v );

		code( SliderEnv {
			env.hovered,
			env.held,
			obj.buf,
			env.label,
			SliderEnv::Anim {
				obj.hover,
				obj.held,
				obj.anim,
				obj.val_anim,
			}
			} );
		} );

	return result;
}

bool CompBuilder::SliderInt( const char* label, int* v, int min, int max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code ) {
	return Slider( label, v, min, max, format, total_bb, bb, code );
}

bool CompBuilder::SliderFloat( const char* label, float* v, float min, float max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code ) {
	return Slider( label, v, min, max, format, total_bb, bb, code );
}

bool CompBuilder::Checkbox( const char* label, bool* v, int* key, float* col, std::function< void( ) > options, bool warning, ImRect total_bb, ImRect bb, ImVec2 options_pos, std::function< void( CheckboxEnv ) > code ) {
	bool pressed = false;

	Empty( label, total_bb, total_bb, [ & ] ( const EmptyEnv& env ) {
		ImGuiLastItemData data = GImGui->LastItemData;

		float h, s1, v1;
		ImColor warning_col;
		ColorConvertRGBtoHSV( GetStyleColorVec4( ImGuiCol_Scheme ).x, GetStyleColorVec4( ImGuiCol_Scheme ).y, GetStyleColorVec4( ImGuiCol_Scheme ).z, h, s1, v1 );
		ColorConvertHSVtoRGB( 0.f, s1, v1, warning_col.Value.x, warning_col.Value.y, warning_col.Value.z );
		warning_col.Value.w = GImGui->Style.Alpha;

		if ( env.pressed ) {
			*v = !*v; // changed here 20.06
		}

		struct s {
			float anim;
			float hover;
			float held;
			float enabled;
			float key_size;
		}; auto& obj = anim_obj( label, 443143223, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || *v );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.enabled = anim( obj.enabled, 0.f, 1.f, *v );

		code( CheckboxEnv {
			env.hovered,
			env.held,
			env.pressed,
			env.label,
			CheckboxEnv::Anim {
				obj.hover,
				obj.held,
				obj.enabled,
				obj.anim,
			}
			} );

		if ( warning ) {
			GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, { total_bb.Min.x - 24, total_bb.GetCenter( ).y - 7 }, warning_col, I_ALERT__TRIANGLE );
		}

		PushItemFlag( ImGuiItemFlags_NoNav, true );

		if ( options ) {
			GetCurrentWindow( )->DC.CursorPos = ImVec2 { options_pos.x - 14 - ( obj.key_size + 42 ) * ( bool ) key - 24.f * ( bool ) col, bb.GetCenter( ).y - 7 };
			char temp [ 64 ];
			ImFormatString( temp, sizeof( temp ), "##%s options", label );

			OpenButton( temp, { 14, 14 }, [ & ] ( OpenButtonEnv env ) {
				ImColor color = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextHovered ), env.anim.hover ), GetColorU32( ImGuiCol_Scheme ), env.anim.open );
				GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, env.bb.Min, color, I_SETTINGS );

				if ( env.anim.open > 0.05f ) {
					PushStyleVar( ImGuiStyleVar_Alpha, env.anim.open );
					PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 12 } );
					PushStyleVar( ImGuiStyleVar_WindowRounding, GImGui->Style.FrameRounding );
					PushStyleVar( ImGuiStyleVar_WindowPadding, { 14, 14 } );
					PushStyleVar( ImGuiStyleVar_WindowBorderSize, 1 );
					PushStyleColor( ImGuiCol_WindowBg, GetColorU32( ImGuiCol_FrameBg ) );
					char str [ 64 ];
					ImFormatString( str, sizeof( str ), "%s options", label );
					Begin( str, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove );
					{
						SetWindowPos( { env.bb.Max.x - GetWindowWidth( ), env.bb.Max.y + 20 - int( 10 * env.anim.open ) } );
						SetWindowSize( { 200, GetCurrentWindow( )->ContentSize.y + GImGui->Style.WindowPadding.y * 2 } );

						BringWindowToDisplayFront( GetCurrentWindow( ) );
						BringWindowToFocusFront( GetCurrentWindow( ) );

						bool is_other_popup = false;
						int max_index = 0;
						int windows_count = 0;
						for ( auto& window : GImGui->Windows ) {
							if ( strstr( window->Name, "options" ) && window->WasActive ) {
								max_index = ImMax( max_index, ( int ) window->BeginOrderWithinContext );
								windows_count++;
							}
						}

						if ( !IsWindowHovered( ) && ( GImGui->HoveredWindow && !strstr( GImGui->HoveredWindow->Name, "popup" ) ) && ( GImGui->HoveredWindow && ( !strstr( GImGui->HoveredWindow->Name, "options" ) || GImGui->HoveredWindow->ID != GetCurrentWindow( )->ID ) ) && ( GetCurrentWindow( )->BeginOrderWithinContext == max_index ) && IsMouseClicked( 0 ) && !env.hovered ) {
							env.open = false;
						}

						PushStyleColor( ImGuiCol_FrameBg, GetColorU32( ImGuiCol_FrameBgHovered ) );
						PushItemWidth( GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 );
						PushItemFlag( ImGuiItemFlags_NoNav, true );

						options( );

						PopItemFlag( );
						PopItemWidth( );
						PopStyleColor( );
					}
					End( );
					PopStyleColor( );
					PopStyleVar( 5 );
				}
				} );
		}

		if ( col ) {
			GetCurrentWindow( )->DC.CursorPos = ImVec2 { options_pos.x - 14 - ( obj.key_size + 42 ) * ( bool ) key, bb.GetCenter( ).y - 7 };
			char color_label [ 64 ];
			ImFormatString( color_label, sizeof( color_label ), "##%s color", label );
			WidgetsManager::get( ).ColorEdit( color_label, col );
		}

		if ( key ) {
			ImFont* key_font = fonts->get( HASH_STR( "Geist-SemiBold.ttf" ), 12.00f );
			if ( key_font )
				obj.key_size = ImLerp( obj.key_size, key_font->CalcTextSizeA( 12, FLT_MAX, -1, key_name_for_vk( *key ) ).x, GetIO( ).DeltaTime * 24 );

			ImVec2 pos = GetCurrentWindow( )->DC.CursorPos;
			ImVec2 pos_prev = GetCurrentWindow( )->DC.CursorPosPrevLine;
			GetCurrentWindow( )->DC.CursorPos = ImVec2 { options_pos.x - 32 - ( int ) obj.key_size, bb.GetCenter( ).y - 12 };
			char bind_label [ 64 ];
			ImFormatString( bind_label, sizeof( bind_label ), "##%s bind", label );
			WidgetsManager::get( ).Binder( bind_label, key );
			GetCurrentWindow( )->DC.CursorPosPrevLine = pos_prev;
			GetCurrentWindow( )->DC.CursorPos = pos;
		}

		PopItemFlag( );

		GImGui->LastItemData = data;

		pressed = env.pressed;
		} );

	return pressed;
}

bool CompBuilder::Combo( const char* label, ImRect total_bb, ImRect bb, std::function< void( ComboEnv ) > code ) {
	Empty( label, total_bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float open_anim;
			bool open;
		}; auto& obj = anim_obj( label, 21231, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || obj.open );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.open_anim = anim( obj.open_anim, 0.f, 1.f, obj.open );

		if ( env.pressed ) {
			obj.open = !obj.open;
		}

		code( ComboEnv {
			env.hovered,
			env.held,
			env.pressed,
			obj.open,
			env.label,
			ComboEnv::Anim {
				obj.hover,
				obj.held,
				obj.open_anim,
				obj.anim
			}
			} );
		} );

	return false;
}

bool CompBuilder::Selectable( const char* label, bool selected, ImRect bb, std::function< void( const SelectableEnv& ) > code ) {
	bool pressed = false;

	Empty( label, bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float selected;
		}; auto& obj = anim_obj( label, 231, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || selected );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.selected = anim( obj.selected, 0.f, 1.f, selected );

		code( SelectableEnv {
			env.hovered,
			env.held,
			env.pressed,
			env.label,
			SelectableEnv::Anim {
				obj.hover,
				obj.held,
				obj.selected,
				obj.anim
			}
			} );

		pressed = env.pressed;
		} );

	return pressed;
}

bool CompBuilder::ColorEdit( const char* label, ImRect total_bb, ImRect bb, float col [ 4 ], std::function< void( ColorEditEnv ) > code ) {
	Empty( label, total_bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float open_anim;
			bool open;
		}; auto& obj = anim_obj( label, 23321, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || obj.open );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.open_anim = anim( obj.open_anim, 0.f, 1.f, obj.open );

		if ( env.pressed ) {
			obj.open = !obj.open;
		}

		code( ColorEditEnv {
			env.hovered,
			env.held,
			env.pressed,
			obj.open,
			env.label,
			ColorEditEnv::Anim {
				obj.hover,
				obj.held,
				obj.open_anim,
				obj.anim
			}
			} );
		} );

	return false;
}

bool CompBuilder::Binder( const char* label, int* key, std::function< void( const BinderEnv& ) > code ) {
	const int vk_limit = static_cast<int>( key_name_list.size( ) );
	if ( vk_limit > 0 && ( *key < 0 || *key >= vk_limit ) )
		*key = 0;

	struct s {
		float anim;
		float hover;
		float held;
		float active;
		float val_anim;
	}; auto& obj = anim_obj( label, 21131, s { } );

	ImFont* bind_font = fonts->get( HASH_STR( "Geist-SemiBold.ttf" ), 12.00f );
	if ( bind_font )
		obj.val_anim = ImLerp( obj.val_anim, bind_font->CalcTextSizeA( 12, FLT_MAX, -1, key_name_for_vk( *key ) ).x, GetIO( ).DeltaTime * 24 );

	ImRect total_bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcTextSize( label, 0, 1 ).x > 0 ? CalcItemWidth( ) : 32.f + ( int ) obj.val_anim, 24 } };
	ImRect bb { total_bb.Max - ImVec2{ 32.f + ( int ) obj.val_anim, 24 }, total_bb.Max };

	Empty( label, total_bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float active;
			bool is_active;
		}; auto& obj = anim_obj( label, 21131, s { } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || IsItemActive( ) );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.active = anim( obj.active, 0.f, 1.f, obj.is_active );

		ImGuiID id = GImGui->LastItemData.ID;
		auto& io = GetIO( );
		ImGuiWindow* window = GetCurrentWindow( );

		const bool hovered = ItemHoverable( bb, id, 0 );

		const bool SHOULD_EDIT = hovered && io.MouseClicked [ 0 ];
		static bool can_bind_m1 = false;

		if ( SHOULD_EDIT ) {
			if ( !obj.is_active ) {
				memset( io.MouseDown, 0, sizeof( io.MouseDown ) );
				//memset( io.KeysDown, 0, sizeof( io.KeysDown ) );
				*key = 58;
				can_bind_m1 = false;
				FocusWindow( window );
			}

			obj.is_active = true;
		}
		else if ( !hovered && io.MouseClicked [ 0 ] && obj.is_active ) {
			obj.is_active = false;
			*key = 0;
		}

		bool value_changed = false;
		int k = *key;

		if ( obj.is_active ) {
			if ( !( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) ) can_bind_m1 = true;

			for ( int i = 0; i < 256 && i < static_cast<int>( key_name_list.size( ) ); ++i ) {
				if ( GetAsyncKeyState( i ) & 0x8000 && ( i != VK_LBUTTON || can_bind_m1 ) && strlen( key_name_for_vk( i ) ) > 0 ) {
					value_changed = true;
					k = i;
					obj.is_active = false;
					break;
				}
			}

			*key = k;
		}

		code( BinderEnv {
			total_bb,
			bb,

			env.hovered,
			env.held,
			env.pressed,
			obj.is_active,
			env.label,

			key_name_list,

			BinderEnv::Anim {
				obj.hover,
				obj.held,
				obj.active,
				obj.anim
			}
			} );
		} );

	return false;
}

bool CompBuilder::OpenButton( const char* str_id, ImVec2 size, std::function< void( OpenButtonEnv ) > code, bool def ) {
	ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + size };
	bool pressed;

	Empty( str_id, bb, bb, [ & ] ( const EmptyEnv& env ) {
		struct s {
			float anim;
			float hover;
			float held;
			float open_anim;
			bool open;
		}; auto& obj = anim_obj( str_id, 4431423123, s { 0, 0, 0, 0, def } );

		obj.anim = anim( obj.anim, 0.f, 1.f, env.hovered || env.held );
		obj.hover = anim( obj.hover, 0.f, 1.f, env.hovered );
		obj.held = anim( obj.held, 0.f, 1.f, env.held );
		obj.open_anim = anim( obj.open_anim, 0.f, 1.f, obj.open );

		if ( env.pressed ) {
			obj.open = !obj.open;
		}

		code( OpenButtonEnv {
			bb,
			env.hovered,
			env.held,
			env.pressed,
			obj.open,
			OpenButtonEnv::Anim {
				obj.hover,
				obj.held,
				obj.anim,
				obj.open_anim
			}
			} );

		pressed = env.pressed;
		} );

	return pressed;
}

#endif // !COMPONENT_BUILDER_CPP
