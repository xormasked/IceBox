#ifndef WIDGETS_MANAGER_CPP
#define WIDGETS_MANAGER_CPP

#include <impl/includes.hpp>

using namespace ImGui;

int WidgetsManager::Config( int i, std::vector< c_config >& configs, bool loaded )
{
	int result = -1;
	char temp [ 64 ];

	auto& item = configs [ i ];
	auto& id = item.id;
	auto& editing = item.editing;
	auto& name = item.name;
	auto& creator = item.author;
	auto& modified = item.modified;

	struct s {
		char savedname [ 32 ];
	}; auto& obj = anim_obj( id, 0, s { } );

	BeginChild( id, { GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2, 32 + GImGui->FontSize * 2 }, ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
	{
		SetCursorPos( { 14, 14 } );
		BeginGroup( );
		{
			bool inputhovered = false;
			Text( name );

			SetCursorPosY( GetCursorPosY( ) - GImGui->Style.ItemSpacing.y + 4 );
			TextDisabled( LangManager::get( ).translate( "Created by: " ).c_str() );
			SameLine( 0, 0 );
			TextColored( GetStyleColorVec4( ImGuiCol_Scheme ), creator );
			SameLine( 0, 12 );
			TextDisabled( LangManager::get( ).translate( "Modified: " ).c_str() );
			SameLine( 0, 0 );
			TextColored( GetStyleColorVec4( ImGuiCol_Scheme ), modified );
		}
		EndGroup( );

		SetCursorPos( ImVec2 { GetWindowWidth( ), GetWindowHeight( ) / 2 } - ImVec2 { 122, 18 } );
		BeginGroup( );
		{
			ImFormatString( temp, sizeof( temp ), "##%s export", id );
			if ( CompBuilder::get( ).Button( temp, { 36, 36 }, [ & ] ( CompBuilder::ButtonEnv env ) {
				auto col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextHovered ), env.anim.hover ), GetColorU32( ImGuiCol_TextHovered, 0.6f ), env.anim.held );
				GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, env.bb.GetCenter( ) - ImVec2 { 7, 7 }, col, I_DOWNLOAD );
				} ) ) {

				PopupManager::get( ).open_popup( [ = ] {
					ImVec2 size { 300, GImGui->FontSize + 38 + GImGui->Style.ItemSpacing.y + GetFrameHeight( ) * 2 };
					SetCursorPos( GetWindowSize( ) / 2 - size / 2 );
					PushStyleVar( ImGuiStyleVar_WindowPadding, { 14, 14 } );
					BeginChild( "export", size, ImGuiChildFlags_Border );
					{
						PushItemWidth( GetWindowWidth( ) - GImGui->Style.WindowPadding.x * 2 );

						char code [ 32 ];
						ImFormatString( code, sizeof( code ), "%s", id );
						TextField( "Copy this code", code, sizeof( code ), { 0, 0 }, "", 0 );

						if ( Button( "Copy", { CalcItemWidth( ), GetFrameHeight( ) } ) ) {
							PopupManager::get( ).close_popup( );
						}

						PopItemWidth( );
					}
					EndChild( );
					PopStyleVar( );
					} );
			}

			SameLine( 0, 0 );

			ImFormatString( temp, sizeof( temp ), "##%s delete", id );
			if ( CompBuilder::get( ).Button( temp, { 36, 36 }, [ & ] ( CompBuilder::ButtonEnv env ) {
				auto col = col_anim( col_anim( GetColorU32( ImGuiCol_Scheme ), GetColorU32( ImGuiCol_Scheme, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Scheme, 0.4f ), env.anim.held );
				GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, env.bb.GetCenter( ) - ImVec2 { 7, 7 }, col, I_TRASH );
				} ) ) {
				result = 2;
			}

			SameLine( 0, 0 );

			ImFormatString( temp, sizeof( temp ), "##%s", id );
			if ( CompBuilder::get( ).Button( temp, { 36, 36 }, [ & ] ( CompBuilder::ButtonEnv env ) {
				auto col = col_anim( col_anim( GetColorU32( ImGuiCol_Button ), GetColorU32( ImGuiCol_ButtonHovered ), env.anim.hover ), GetColorU32( ImGuiCol_ButtonActive ), env.anim.held );
				GetWindowDrawList( )->AddRectFilled( env.bb.Min, env.bb.Max, col, GImGui->Style.FrameRounding );
				GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, env.bb.GetCenter( ) - ImVec2 { 7, 7 }, GetColorU32( ImGuiCol_TextButton ), loaded ? I_CHECK : I_PLAYER__PLAY );
				} ) ) {
				result = 1;
			}
		}
		EndGroup( );
	}
	EndChild( );

	return result;
}


bool WidgetsManager::Checkbox( std::string label, bool* v, int* key, float* col, std::function< void( ) > options, bool warning, bool disabled ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { Checkbox( label, v, key, col, options, warning, disabled ); } );

	if ( disabled ) {
		PushItemFlag( ImGuiItemFlags_Disabled, true );
		PushStyleVar( ImGuiStyleVar_Alpha, 0.5f * GImGui->Style.Alpha );
	}

	float square_sz = 16;

	ImRect total_bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcTextSize( label.c_str( ), 0, 1 ).x + GImGui->Style.ItemSpacing.x + square_sz, square_sz } };

	if ( warning ) {
		total_bb.Min.x += 24;
		total_bb.Max.x += 24;
	}

	ImRect bb { total_bb.Min, total_bb.Min + ImVec2{ square_sz, square_sz } };
	ImVec2 options_pos { bb.Min.x + CalcItemWidth( ) - 24.f * warning, bb.Min.y };

	bool result = CompBuilder::get( ).Checkbox( label.c_str( ), v, key, col, options, warning, total_bb, bb, options_pos, [ & ] ( CompBuilder::CheckboxEnv env ) {
		ImColor col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.enabled );

		GetWindowDrawList( )->AddRect( bb.Min, bb.Max, GetColorU32( ImGuiCol_Border ), 2 );
		GetWindowDrawList( )->AddRectFilled( bb.Min, bb.Max, GetColorU32( ImGuiCol_Scheme, env.anim.enabled ), 2 );
		RenderCheckMark( GetWindowDrawList( ), bb.GetCenter( ) - ImVec2 { 4, 4 }, GetColorU32( ImGuiCol_ChildBg, env.anim.enabled ), 8 );

		GetWindowDrawList( )->AddText( { total_bb.Min.x + GImGui->Style.ItemInnerSpacing.x + square_sz, total_bb.GetCenter( ).y - GImGui->FontSize / 2 - 0.5f }, col, env.label, FindRenderedTextEnd( env.label ) );
		} );

	if ( disabled ) {
		PopStyleVar( );
		PopItemFlag( );
	}

	return result;
}

template < typename T >
bool WidgetsManager::Slider( std::string label, T* v, T min, T max, const char* format ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { Slider( label, v, min, max, format ); } );

	ImRect total_bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcItemWidth( ), GImGui->FontSize + GImGui->Style.ItemInnerSpacing.y + 5 } };
	ImRect bb { total_bb.Max - ImVec2{ total_bb.GetWidth( ), 5 }, total_bb.Max };
	return CompBuilder::get( ).Slider( label.c_str( ), v, min, max, format,
		total_bb,
		bb,
		[ & ] ( const CompBuilder::SliderEnv& env ) {
			ImColor col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.held );

			GetWindowDrawList( )->AddRect( bb.Min, bb.Max, GetColorU32( ImGuiCol_Border ), 2 );
			GetWindowDrawList( )->AddRectFilled( bb.Min, bb.Min + ImVec2 { env.anim.val_anim, bb.GetHeight( ) }, GetColorU32( ImGuiCol_Scheme ), 2 );
			GetWindowDrawList( )->AddCircleFilled( { bb.Min.x + env.anim.val_anim, bb.GetCenter( ).y }, 5.5f + env.anim.anim - 2.f * env.anim.held, GetColorU32( ImGuiCol_Text ), 36 );

			GetWindowDrawList( )->AddText( total_bb.Min, GetColorU32( ImGuiCol_Text ), env.label, FindRenderedTextEnd( env.label ) );

			auto pos = GetCurrentWindow( )->DC.CursorPos;
			auto pos_prev = GetCurrentWindow( )->DC.CursorPosPrevLine;
			GetCurrentWindow( )->DC.CursorPos = ImVec2 { total_bb.Max.x - CalcTextSize( env.buf ).x, total_bb.Min.y };
			char temp [ 64 ];
			ImFormatString( temp, sizeof( temp ), "##%s input", label.c_str( ) );
			PushStyleColor( ImGuiCol_FrameBg, GetColorU32( ImGuiCol_FrameBg, 0.f ) );
			PushStyleVar( ImGuiStyleVar_FramePadding, { 0, 0 } );
			PushItemFlag( ImGuiItemFlags_NoNav, true );
			if ( InputText( temp, env.buf, sizeof( env.buf ) ) ) {
				ImGuiDataType data_type = std::is_same< T, int >::value ? ImGuiDataType_S32 : ImGuiDataType_Float;
				DataTypeApplyFromText( env.buf, data_type, v, format );
			}
			PopItemFlag( );
			PopStyleVar( );
			PopStyleColor( );
			GetCurrentWindow( )->DC.CursorPos = pos;
			GetCurrentWindow( )->DC.CursorPosPrevLine = pos_prev;
		} );
}

bool WidgetsManager::SliderInt( std::string label, int* v, int min, int max, const char* format ) {
	return Slider( label, v, min, max, format );
}

bool WidgetsManager::SliderFloat( std::string label, float* v, float min, float max, const char* format ) {
	return Slider( label, v, min, max, format );
}

bool WidgetsManager::ComboEx( std::string label, const char* preview_value, std::function< void( CompBuilder::ComboEnv env ) > code ) {
	ImRect total_bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcItemWidth( ), GetFrameHeight( ) + GImGui->FontSize + GImGui->Style.ItemInnerSpacing.y } };
	ImRect bb { total_bb.Max - ImVec2{ CalcItemWidth( ), GetFrameHeight( ) }, total_bb.Max };

	CompBuilder::get( ).Combo( label.c_str( ), total_bb, bb, [ & ] ( const CompBuilder::ComboEnv& env ) {
		ImColor col = col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextDisabled, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Scheme ), env.anim.open );

		GetWindowDrawList( )->AddText( total_bb.Min, GetColorU32( ImGuiCol_Text ), env.label, FindRenderedTextEnd( env.label ) );

		GetWindowDrawList( )->AddRect( bb.Min, bb.Max, GetColorU32( ImGuiCol_Border ), GImGui->Style.FrameRounding, env.open ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersAll );
		GetWindowDrawList( )->AddText( bb.Min + GImGui->Style.FramePadding, GetColorU32( ImGuiCol_Text ), LangManager::get( ).translate( preview_value ).c_str( ) );
		GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, { bb.Max.x - GImGui->Style.FramePadding.x - 14, bb.GetCenter( ).y - 7.5f }, col, I_SELECTOR );

		if ( env.anim.open > 0.05f ) {
			SetNextWindowPos( { bb.Min.x, bb.Max.y } );
			PushStyleVar( ImGuiStyleVar_Alpha, env.anim.open );
			PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, 0 } );
			PushStyleVar( ImGuiStyleVar_WindowRounding, GImGui->Style.FrameRounding );
			PushStyleVar( ImGuiStyleVar_WindowPadding, { 0, 0 } );
			PushStyleColor( ImGuiCol_WindowBg, GetColorU32( ImGuiCol_ChildBg ) );
			char temp [ 32 ]; // changed here 20.06
			ImFormatString( temp, sizeof( temp ), "%s popup", label.c_str( ) ); // changed here 20.06
			Begin( temp, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground ); // changed here 20.06
			{
				SetWindowSize( { bb.GetWidth( ), ( ( ImMin( GetCurrentWindow( )->ContentSize.y, GetFrameHeight( ) * 5 ) ) + GImGui->Style.WindowRounding ) * env.anim.open } );

				if ( env.anim.open > 0.9f )
					GetCurrentWindow( )->PopupId = GetCurrentWindow( )->ID;
				else
					GetCurrentWindow( )->PopupId = 0;

				BringWindowToDisplayFront( GetCurrentWindow( ) );
				BringWindowToFocusFront( GetCurrentWindow( ) );

				GetWindowDrawList( )->AddRectFilled( GetWindowPos( ), GetWindowPos( ) + GetWindowSize( ), GetColorU32( ImGuiCol_WindowBg ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );
				GetWindowDrawList( )->AddRect( GetWindowPos( ) - ImVec2 { 0, 1 }, GetWindowPos( ) + GetWindowSize( ), GetColorU32( ImGuiCol_Border ), GImGui->Style.WindowRounding, ImDrawFlags_RoundCornersBottom );

				if ( !IsWindowHovered( ImGuiHoveredFlags_AllowWhenBlockedByActiveItem ) && IsMouseClicked( 0 ) && !env.hovered ) {
					env.open = false;
				}

				code( env );
			}
			End( );
			PopStyleColor( );
			PopStyleVar( 4 );
		}
		} );

	return false;
}

bool WidgetsManager::Combo( std::string label, int* v, std::vector<std::string> items ) {
	if ( items.empty( ) )
		return false;

	const int max_idx = static_cast<int>( items.size( ) ) - 1;
	if ( *v < 0 || *v > max_idx )
		*v = 0;

	SearchManager::get( ).additem( label, [ = ] ( ) { Combo( label, v, items ); } );

	ComboEx( label, items [ static_cast<size_t>( *v ) ].c_str( ), [ & ] ( CompBuilder::ComboEnv env ) {
		for ( int i = 0; i < items.size( ); ++i ) {
			if ( Selectable( items [ i ], *v == i ) ) {
				*v = i;
				env.open = !env.open;
			}
		}
		} );

	return false;
}

bool WidgetsManager::MultiCombo( std::string label, std::vector< multi_select_item >* items ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { MultiCombo( label, items ); } );

	auto& style = GetStyle( );

	std::string buf;

	buf.clear( );
	for ( size_t i = 0; i < items->size( ); ++i ) {
		if ( items->at( i ).selected ) {
			buf += LangManager::get( ).translate( items->at( i ).label );
			buf += ", ";
		}
	}

	if ( !buf.empty( ) ) {
		buf.resize( buf.size( ) - 2 );
	}

	if ( CalcTextSize( buf.c_str( ) ).x > 145 - style.FramePadding.x * 3 - 10 ) {
		for ( int i = 0; i < buf.size( ) - 1; ++i ) {
			if ( CalcTextSize( buf.substr( 0, i + 1 ).c_str( ) ).x > 145 - style.FramePadding.x - 10 ) {
				buf.resize( i );
				if ( buf [ buf.size( ) - 1 ] == ',' ) {
					buf.resize( buf.size( ) - 1 );
				}
				buf.append( ".." );
			}
		}
	}

	ComboEx( label, buf.c_str( ), [ & ] ( CompBuilder::ComboEnv env ) {
		for ( int i = 0; i < items->size( ); ++i ) {
			if ( Selectable( items->at( i ).label, items->at( i ) ) ) {
				items->at( i ).selected = !items->at( i );
			}
		}
		} );

	return false;
}

bool WidgetsManager::Binder( std::string label, int* key ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { Binder( label, key ); } );

	CompBuilder::get( ).Binder( label.c_str( ), key, [ & ] ( const CompBuilder::BinderEnv& env ) {
		GetWindowDrawList( )->AddText( { env.total_bb.Min.x, env.total_bb.GetCenter( ).y - GImGui->FontSize / 2 }, GetColorU32( ImGuiCol_Text ), env.label, FindRenderedTextEnd( env.label ) );

		GetWindowDrawList( )->AddRectFilled( env.bb.Min, env.bb.Max, GetColorU32( ImGuiCol_FrameBg ), 2 );
		ImFont* bind_font = fonts->get( HASH_STR("Geist-SemiBold.ttf" ), 12.00f );
		if ( bind_font )
			GetWindowDrawList( )->AddText( bind_font, 12, env.bb.Min + ImVec2 { 6, env.bb.GetHeight( ) / 2 - 6 }, GetColorU32( ImGuiCol_Text ), key_name_for_vk( *key ) );
		ImFont* icon_font = fonts->get( HASH_STR( "Glyphter.tff" ), 12.00f );
		if ( icon_font )
			GetWindowDrawList( )->AddText( icon_font, 12, env.bb.Min + ImVec2 { env.bb.GetWidth( ) - 18, env.bb.GetHeight( ) / 2 - 6 }, GetColorU32( ImGuiCol_Scheme ), I_KEYBOARD );
		} );

	return false;
}

bool WidgetsManager::TextField( std::string label, char* buf, size_t buf_size, ImVec2 size, const char* hint, const char* icon ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { TextField( label, buf, buf_size, size, hint, icon ); } );

	char str_id [ 64 ];
	ImFormatString( str_id, sizeof( str_id ), "##%s", label.c_str( ) );

	ImVec2 pos = GetCursorPos( );

	bool value_changed = false;

	if ( CalcTextSize( label.c_str( ), 0, 1 ).x > 0 ) {
		PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0, GImGui->Style.ItemInnerSpacing.y } );
		TextEx( label.c_str( ), FindRenderedTextEnd( label.c_str( ) ) );
		PopStyleVar( );
	}

	if ( icon ) {
		ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + CalcItemSize( size, CalcItemWidth( ), GetFrameHeight( ) ) };
		GetWindowDrawList( )->AddRectFilled( bb.Min, bb.Max, GetColorU32( ImGuiCol_FrameBg ), GImGui->Style.FrameRounding );

		if ( GImGui->Style.FrameBorderSize != 0 ) {
			GetWindowDrawList( )->AddRect( bb.Min, bb.Max, GetColorU32( ImGuiCol_Border ), GImGui->Style.FrameRounding );
		}

		GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, bb.Min + GImGui->Style.FramePadding - ImVec2 { 0, 1 }, GetColorU32( ImGuiCol_TextDisabled ), icon );

		PushStyleColor( ImGuiCol_FrameBg, GetColorU32( ImGuiCol_FrameBg, 0 ) );
		PushStyleColor( ImGuiCol_Border, GetColorU32( ImGuiCol_Border, 0 ) );
		SetCursorPosX( pos.x + 24 );
		SetCursorPosY( pos.y - 1 );
		value_changed = InputTextEx( str_id, LangManager::get( ).translate( hint ).c_str( ), buf, buf_size, size - ImVec2 { 24, 0 }, 0 );
		PopStyleColor( 2 );
	}
	else {
		value_changed = InputTextEx( str_id, LangManager::get( ).translate( hint ).c_str( ), buf, buf_size, size, 0 );
	}

	return value_changed;
}

bool WidgetsManager::Button( std::string label, ImVec2 size ) {
	return CompBuilder::get( ).Button( label.c_str( ), size, [ & ] ( const CompBuilder::ButtonEnv& env ) {
		auto col = col_anim( col_anim( GetColorU32( ImGuiCol_Button ), GetColorU32( ImGuiCol_ButtonHovered ), env.anim.hover ), GetColorU32( ImGuiCol_ButtonActive ), env.anim.held );

		GetWindowDrawList( )->AddRectFilled( env.bb.Min, env.bb.Max, col, GImGui->Style.FrameRounding );
		GetWindowDrawList( )->AddText( env.bb.GetCenter( ) - CalcTextSize( env.label, 0, 1 ) / 2, GetColorU32( ImGuiCol_TextButton ), env.label, FindRenderedTextEnd( env.label ) );
		} );
}

bool WidgetsManager::WarningButton( std::string label, ImVec2 size ) {
	bool result = false;

	if ( Button( label, size ) ) {
		PopupManager::get( ).open_popup( [ & ] ( ) {
			float h, s1, v1;
			ImColor warning_col;
			ColorConvertRGBtoHSV( GetStyleColorVec4( ImGuiCol_Scheme ).x, GetStyleColorVec4( ImGuiCol_Scheme ).y, GetStyleColorVec4( ImGuiCol_Scheme ).z, h, s1, v1 );
			ColorConvertHSVtoRGB( 0.f, s1, v1, warning_col.Value.x, warning_col.Value.y, warning_col.Value.z );
			warning_col.Value.w = GImGui->Style.Alpha;

			SetCursorPos( GetWindowSize( ) / 2 - ImVec2 { 300, 150 } / 2 );
			BeginChild( "popup window", { 300, 150 } );
			{
				GetWindowDrawList( )->AddText(fonts->get( HASH_STR( "Glyphter.tff" ), 14.00f ), 14, GetWindowPos( ) + ImVec2 { 20, 20 }, ImColor { warning_col.Value.x, warning_col.Value.y, warning_col.Value.z, GImGui->Style.Alpha }, I_ALERT__TRIANGLE );
				GetWindowDrawList( )->AddText( GetWindowPos( ) + ImVec2 { 44, 20 }, GetColorU32( ImGuiCol_Text ), LangManager::get( ).translate( "Are you sure u want to enable it?" ).c_str( ) );
				GetWindowDrawList( )->AddText( GetWindowPos( ) + ImVec2 { 20, 46 }, GetColorU32( ImGuiCol_TextDisabled ), LangManager::get( ).translate( "This function is " ).c_str( ) );
				GetWindowDrawList( )->AddText( GetWindowPos( ) + ImVec2 { 20 + CalcTextSize( LangManager::get( ).translate( "This function is " ).c_str( ) ).x, 46 }, GetColorU32( ImGuiCol_Text ), LangManager::get( ).translate( "dangerous!" ).c_str( ) );

				SetCursorPos( { 20, GetWindowHeight( ) - 20 - GetFrameHeight( ) } );
				if ( WidgetsManager::get( ).Button( "YES", { 100, GetFrameHeight( ) } ) ) {
					result = true;
					PopupManager::get( ).close_popup( );
				}

				SameLine( 0, 10 );

				PushStyleColor( ImGuiCol_Button, GetColorU32( ImGuiCol_FrameBg ) );
				PushStyleColor( ImGuiCol_ButtonHovered, GetColorU32( ImGuiCol_FrameBgHovered ) );
				PushStyleColor( ImGuiCol_ButtonActive, GetColorU32( ImGuiCol_FrameBgActive ) );
				PushStyleColor( ImGuiCol_TextButton, GetColorU32( ImGuiCol_Text ) );
				if ( WidgetsManager::get( ).Button( "NO", { GetWindowWidth( ) - 150, GetFrameHeight( ) } ) ) {
					PopupManager::get( ).close_popup( );
				}
				PopStyleColor( 4 );
			}
			EndChild( );
			} );
	}

	return result;
}

bool WidgetsManager::ColorEdit( std::string label, float col [ 4 ] ) {
	SearchManager::get( ).additem( label, [ = ] ( ) { ColorEdit( label, col ); } );

	float square_sz = 14;

	ImRect total_bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + ImVec2{ CalcTextSize( label.c_str( ), 0, 1 ).x > 0 ? CalcItemWidth( ) : square_sz, square_sz } };
	ImRect bb { total_bb.Max - ImVec2{ square_sz, square_sz }, total_bb.Max };

	bool value_changed = false;

	CompBuilder::get( ).ColorEdit( label.c_str( ), total_bb, bb, col, [ & ] ( CompBuilder::ColorEditEnv env ) {
		GetWindowDrawList( )->AddText( { total_bb.Min.x, total_bb.GetCenter( ).y - GImGui->FontSize / 2 }, col_anim( col_anim( GetColorU32( ImGuiCol_TextDisabled ), GetColorU32( ImGuiCol_TextHovered ), env.anim.hover ), GetColorU32( ImGuiCol_Text ), env.anim.open ), env.label, FindRenderedTextEnd( env.label ) );

		GetWindowDrawList( )->AddCircleFilled( bb.GetCenter( ), square_sz / 2, ImColor { col [ 0 ], col [ 1 ], col [ 2 ], GImGui->Style.Alpha }, 36 );

		if ( env.anim.open > 0.05f ) {
			SetNextWindowPos( { bb.Min.x, bb.Max.y + 5 } );
			PushStyleVar( ImGuiStyleVar_Alpha, env.anim.open );
			PushStyleVar( ImGuiStyleVar_ItemSpacing, { 10, 10 } );
			PushStyleVar( ImGuiStyleVar_WindowRounding, GImGui->Style.FrameRounding );
			PushStyleVar( ImGuiStyleVar_WindowPadding, { 10, 10 } );
			PushStyleColor( ImGuiCol_WindowBg, GetColorU32( ImGuiCol_FrameBg ) );
			char temp [ 64 ];
			ImFormatString( temp, sizeof( temp ), "%s popup", label.c_str( ) );
			Begin( temp, 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize );
			{
				BringWindowToDisplayFront( GetCurrentWindow( ) );
				BringWindowToFocusFront( GetCurrentWindow( ) );

				if ( env.anim.open > 0.9f )
					GetCurrentWindow( )->PopupId = GetCurrentWindow( )->ID;
				else
					GetCurrentWindow( )->PopupId = 0;

				if ( !IsWindowHovered( ) && IsMouseClicked( 0 ) && !env.hovered ) {
					env.open = false;
				}

				ColorPickerManager::get( ).draw( env.label, col );
			}
			End( );
			PopStyleColor( );
			PopStyleVar( 4 );
		}
		} );

	return value_changed;
}

bool WidgetsManager::Selectable( std::string label, bool selected, ImVec2 size ) {
	ImRect bb { GetCurrentWindow( )->DC.CursorPos, GetCurrentWindow( )->DC.CursorPos + CalcItemSize( size, GetWindowWidth( ), GetFrameHeight( ) ) };
	return CompBuilder::get( ).Selectable( label.c_str( ), selected, bb, [ & ] ( const CompBuilder::SelectableEnv& env ) {
		ImColor col = col_anim( col_anim( GetColorU32( ImGuiCol_Text ), GetColorU32( ImGuiCol_Text, 0.6f ), env.anim.hover ), GetColorU32( ImGuiCol_Scheme ), env.anim.selected );

		GetWindowDrawList( )->AddRectFilled( bb.Min, bb.Max, GetColorU32( ImGuiCol_FrameBg, env.anim.selected ) );
		GetWindowDrawList( )->AddText( { bb.Min.x + GImGui->Style.FramePadding.x, bb.GetCenter( ).y - GImGui->FontSize / 2 }, col, env.label, FindRenderedTextEnd( env.label ) );
		} );
}

void WidgetsManager::Separator( ) {
	GetWindowDrawList( )->AddRectFilled( { GetWindowPos( ).x, GetCurrentWindow( )->DC.CursorPos.y }, { GetWindowPos( ).x + GetWindowWidth( ), GetCurrentWindow( )->DC.CursorPos.y + 1 }, GetColorU32( ImGuiCol_Separator ) );
	Dummy( { GetWindowWidth( ), 1 } );
}

#endif // !WIDGETS_MANAGER_CPP
