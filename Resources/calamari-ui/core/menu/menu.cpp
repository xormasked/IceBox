#ifndef MENU_CPP
#define MENU_CPP

#include <impl/includes.hpp>
#include <impl/hook_bridge.hpp>
#include <fstream>

auto menu_interface::c_interface::initialize( ) -> void
{
	size = ImVec2 { 660, 675 };

	if (!GImGui)
		return;

	GImGui->IO.IniFilename = "";

	try {
		StyleManager::get( ).Styles( );
		StyleManager::get( ).Colors( );
	} catch (const std::exception& e) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[StyleManager Exception] " << e.what() << " | Type: " << typeid(e).name() << std::endl;
		log.close();
		return;
	} catch (...) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[StyleManager Unknown] Caught unknown exception type" << std::endl;
		log.close();
		return;
	}

	try {
		LangManager::get( ).initialize( );
	} catch (const std::exception& e) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[LangManager Exception] " << e.what() << " | Type: " << typeid(e).name() << std::endl;
		log.close();
		return;
	} catch (...) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[LangManager Unknown] Caught unknown exception type" << std::endl;
		log.close();
		return;
	}

	if (!fonts)
		return;

	auto config = ImFontConfig( );

	config.OversampleH = 1;
	config.OversampleV = 1;
	// Embedded fonts live in static storage (fonts_binary::*). Default ImGui ownership frees FontData on atlas teardown → invalid heap pointer in debug CRT.
	config.FontDataOwnedByAtlas = false;

	try {
		fonts->add( HASH_STR( "Karla-SemiBold.ttf" ) , fonts_binary::karla_semibold , sizeof( fonts_binary::karla_semibold ) , { 16.0f, 25.00f } , &config , ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );
		fonts->add( HASH_STR( "Geist-SemiBold.ttf" ) , fonts_binary::geist_semi_bold , sizeof( fonts_binary::geist_semi_bold ) , { 14.00f, 12.00f } , &config , ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );

		ImWchar icons_ranges [ ] = { 0x1 + 1000, 0x1 + 5881, 0 };
		fonts->add( HASH_STR( "Glyphter.tff" ) , fonts_binary::glyphter , sizeof( fonts_binary::glyphter ) , { 14.00f, 12.00f } , &config , icons_ranges );
	} catch (const std::exception& e) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[Fonts Exception] " << e.what() << " | Type: " << typeid(e).name() << std::endl;
		log.close();
		return;
	} catch (...) {
		std::ofstream log("menu_error.log", std::ios::app);
		log << "[Fonts Unknown] Caught unknown exception type" << std::endl;
		log.close();
		return;
	}

	if (!vars)
		return;

	static ChildManager & cm = ChildManager::get( );
	static auto & widgets = WidgetsManager::get( );

	TabsManager::get( ).add_page( 0 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Ragebot" ) );
				{
					widgets.Checkbox( HASH_STR( "Ragebot" ), &vars->combat.rage_bot.on , nullptr , nullptr , [ & ] ( )
						{
							widgets.Checkbox( HASH_STR( "Vis check" ) , &vars->combat.rage_bot.vischeck , nullptr , nullptr , nullptr );
							widgets.Checkbox( HASH_STR( "Pen check" ), &vars->combat.rage_bot.pencheck, nullptr, nullptr, nullptr );
						} );
					widgets.Checkbox( HASH_STR( "Ghosting" ), &vars->visual.jitter_peek, &vars->visual.jitter_peek_vk, nullptr, [ & ] ( )
						{
							widgets.SliderInt( HASH_STR( "Delay ( MS )" ), &vars->visual.jitter_peek_delay_ms, 0.0, 160.0 );
						} );
				}
				cm.endchild( );
			}
			EndGroup( );

			SameLine( );

			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Silent Aim" ) );
				{
					widgets.Checkbox( HASH_STR( "PSilent" ), &vars->aim.silent_aim.state, nullptr, nullptr, nullptr );
					widgets.Checkbox( HASH_STR( "Enable FOV" ), &vars->aim.fov_circle.state , nullptr ,
						vars->aim.fov_circle.color.to_array( ) , [ & ] ( )
						{
							widgets.Checkbox( HASH_STR( "Filled circle" ), &vars->aim.fov_circle_filled , nullptr , nullptr , nullptr );
							widgets.SliderFloat( HASH_STR( "Circle opacity" ) , &vars->aim.fov_circle_opacity , 0.f , 1.f );
							if ( vars->aim.silent_aim.is_enabled( ) ) {
								widgets.SliderFloat( HASH_STR( "FOV Size" ) , &vars->aim.silent_aim_fov , 0.1f , 90.0f );
							}
						} );
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	TabsManager::get( ).add_page( 0 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Weapon" ) );
				{
					widgets.Checkbox( HASH_STR( "No Spread" ), &vars->misc.no_weapon_spread, nullptr, nullptr, nullptr );
					widgets.Checkbox( HASH_STR( "No Recoil" ), &vars->misc.no_recoil, nullptr, nullptr, nullptr );
				}
				cm.endchild( );

				cm.beginchild( HASH_STR( "Bullet Settings" ) );
				{
				}
				cm.endchild( );
			}
			EndGroup( );

			SameLine( );

			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Animation" ) );
				{
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	TabsManager::get( ).add_page( 1 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Visuals" ) );
				{
					widgets.Checkbox( HASH_STR( "Aspect ratio" ) , &vars->misc.aspect_ratio_changer.state , nullptr , nullptr , [ & ] ( )
						{
							widgets.SliderFloat( HASH_STR( "Ratio" ) , &vars->misc.aspect_ratio , 0.3f , 2.00f );
						} );

					widgets.Checkbox( HASH_STR( "Skeleton ESP" ) , &vars->esp.skeleton.enabled , nullptr ,
						vars->esp.skeleton.color.to_array( ) , [ & ] ( )
						{
							widgets.SliderFloat( HASH_STR( "Thickness" ) , &vars->esp.skeleton.thickness , 0.5f , 5.0f );
						} );

					widgets.Checkbox( HASH_STR( "Box ESP" ), &vars->esp.box.enabled, nullptr,
						vars->esp.box.color.to_array( ), [ & ] ( )
						{
							widgets.SliderFloat( HASH_STR( "Thickness" ), &vars->esp.box.thickness, 0.5f, 2.0f );
						} );


				}
				cm.endchild( );
			}
			EndGroup( );

			SameLine( );

			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Visual Options" ) );
				{
					widgets.Checkbox( HASH_STR( "Third Person" ), &vars->visual.third_person, &vars->visual.third_person_vk, nullptr, nullptr );
					widgets.Checkbox( HASH_STR( "FOV Editor" ), &vars->visual.fov_enabled, nullptr, nullptr, [ & ] ( )
						{
							widgets.SliderFloat( HASH_STR( "Eye FOV" ), &vars->visual.eye_fov_degrees, 70.0f, 120.0f );
							widgets.SliderFloat( HASH_STR( "Viewmodel FOV" ), &vars->visual.viewmodel_fov_degrees, 1.0f, 120.0f );
						} );
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	auto misc_stub_page = [ & ] ( const char * title )
		{
			BeginGroup( );
			{
				cm.beginchild( title );
				{
				}
				cm.endchild( );
			}
			EndGroup( );
		};

	TabsManager::get( ).add_page( 3, [ & ] {
		BeginGroup( );
		{
			cm.beginchild( HASH_STR( "General" ) );
			{
				widgets.Checkbox( HASH_STR( "Run & Shoot" ), &vars->misc.run_and_shoot, nullptr, nullptr, nullptr );
				widgets.Checkbox( HASH_STR( "Long Melee" ), &vars->misc.long_melee.state, nullptr, nullptr, nullptr );
				widgets.Checkbox( HASH_STR( "Self Revive" ), &vars->misc.instant_revive.state, &vars->misc.instant_revive.keybind.key, nullptr, nullptr );
				widgets.Checkbox( HASH_STR( "Unlock All" ), &vars->visual.unlock_all_mid_hook, nullptr, nullptr, nullptr );

			}
			cm.endchild( );
		}
		EndGroup( );
	} );
	TabsManager::get( ).add_page( 3 , [ & ] { misc_stub_page( HASH_STR( "Viewmodel" ) ); } );
	TabsManager::get( ).add_page( 3 , [ & ] { misc_stub_page( HASH_STR( "Overlays" ) ); } );
	TabsManager::get( ).add_page( 3 , [ & ] { misc_stub_page( HASH_STR( "Skybox" ) ); } );
	TabsManager::get( ).add_page( 3 , [ & ] { misc_stub_page( HASH_STR( "Rust+" ) ); } );

	TabsManager::get( ).add_page( 2 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "World Modulation" ) );
				{
					widgets.Checkbox( HASH_STR( "Enable" ) , &vars->world.modulation.enabled );

					widgets.Separator( );

					auto channel = [ & ] ( const char * name , HookVars::world_channel & ch )
						{
							widgets.Checkbox( name , &ch.edit , nullptr , nullptr , [ & ] ( )
								{
									widgets.ColorEdit( std::string( name ) + "##wm_rgb" ,
										reinterpret_cast< float* >( &ch.rgb ) );
								} );
						};

					channel( HASH_STR( "Light" ) , vars->world.modulation.lighting );
					channel( HASH_STR( "Reflection" ) , vars->world.modulation.reflection );
					channel( HASH_STR( "Highlight" ) , vars->world.modulation.highlight );
					channel( HASH_STR( "Top / bottom" ) , vars->world.modulation.top_bottom );
					channel( HASH_STR( "Global illumination" ) , vars->world.modulation.global_illumination );
					channel( HASH_STR( "Magic" ) , vars->world.modulation.magic );
					channel( HASH_STR( "Top / bottom fog" ) , vars->world.modulation.top_bottom_fog );
					channel( HASH_STR( "Sky" ) , vars->world.modulation.sky );
					channel( HASH_STR( "Highlight 2" ) , vars->world.modulation.highlight_secondary );
				}
				cm.endchild( );
			}
			EndGroup( );

			SameLine( );

			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "World Edit" ) );
				{
					widgets.Checkbox( HASH_STR( "Enable" ) , &vars->world.edit.enabled );
					widgets.SliderFloat( HASH_STR( "Clarity" ) , &vars->world.edit.clarity , 0.5f , 3.0f );
					widgets.SliderFloat( HASH_STR( "Saturation" ) , &vars->world.edit.saturation , 0.0f , 3.0f );
					widgets.SliderFloat( HASH_STR( "Brightness" ) , &vars->world.edit.brightness , 0.0f , 3.0f );
					widgets.SliderFloat( HASH_STR( "Illumination" ) , &vars->world.edit.illumination , 0.0f , 16.0f );
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	TabsManager::get( ).add_page( 2 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Player Glow" ) );
				{
					static std::vector< std::string > glow_styles = {
						std::string( HASH_STR( "Solid" ) ),
						std::string( HASH_STR( "Outline" ) ),
					};

					widgets.Checkbox( HASH_STR( "Player Glow" ) , &vars->world.modulation.glow.enabled , nullptr , nullptr , [ & ] ( )
						{
							widgets.Combo( HASH_STR( "Glow style" ) , &vars->world.modulation.glow.style , glow_styles );
							widgets.ColorEdit( HASH_STR( "Glow color" ) , reinterpret_cast< float* >( &vars->world.modulation.glow.rgb ) );
						} );
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	TabsManager::get( ).add_page( 4 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Injection" ) );
				{
					if ( widgets.Button( HASH_STR( "Uninject" ) , { CalcItemWidth( ) , 0 } ) )
						hook_bridge::request_uninject( );
				}
				cm.endchild( );
			}
			EndGroup( );
		} );

	TabsManager::get( ).add_page( 4 , [ & ] ( )
		{
			BeginGroup( );
			{
				cm.beginchild( HASH_STR( "Configs" ) );
				{
				}
				cm.endchild( );
			}
			EndGroup( );
		} );
}

auto menu_interface::c_interface::draw( ) -> void
{
	static bool init = false;
	if ( !init )
	{
		for ( int i = 0; i < TabsManager::get( ).get_tabs( ).size( ); ++i )
		{
			if ( TabsManager::get( ).get_tabs( ) [ i ].pages.empty( ) ) continue;

			TabsManager::get( ).set_tab( i );
			for ( int j = 0; j < TabsManager::get( ).get_tabs( ) [ i ].pages.size( ); ++j )
			{
				TabsManager::get( ).set_subtab( j );
				TabsManager::get( ).get_tabs( ) [ i ].pages [ j ]( );
			}

			TabsManager::get( ).set_subtab( 0 );
		}

		TabsManager::get( ).set_tab( 0 );

		init = true;
	}

	SetNextWindowSize( size );
	Begin( HASH_STR( "GUI" ) , 0 , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus );
	{
		BeginChild( HASH_STR( "navbar" ) , { 0, 56 } , 0 , ImGuiWindowFlags_NoBackground );
		{
			// Safely get texture (handle standalone mode without monoauth)
			ID3D11Texture2D* texture = nullptr;
			if (monoauth && monoauth->session && monoauth->session->brand) {
				texture = monoauth->session->brand->get_texture( );
			}
			if ( texture )
				GetWindowDrawList( )->AddImage( ( ImTextureID ) texture , GetWindowPos( ) + ImVec2 { GetWindowHeight( ) / 2 - 13, GetWindowHeight( ) / 2 - 13 } , GetWindowPos( ) + ImVec2 { GetWindowHeight( ) / 2 + 13, GetWindowHeight( ) / 2 + 13 } );

			SetCursorPosX( 56 );
			PushStyleColor( ImGuiCol_FrameBg , GetColorU32( ImGuiCol_FrameBg , 0.f ) );
			PushStyleVar( ImGuiStyleVar_FrameBorderSize , 0 );
			PushStyleVar( ImGuiStyleVar_FramePadding , { 16, GetWindowHeight( ) / 2 - GImGui->FontSize / 2 } );
			PushItemFlag( ImGuiItemFlags_NoNav , true );
			GetWindowDrawList( )->AddRectFilled( GetCurrentWindow( )->DC.CursorPos , GetCurrentWindow( )->DC.CursorPos + ImVec2 { 1, GetWindowHeight( ) } , GetColorU32( ImGuiCol_Border ) );
			static bool ac = false;
			if ( WidgetsManager::get( ).TextField( HASH_STR( "##search" ) , SearchManager::get( ).search_buf , sizeof( SearchManager::get( ).search_buf ) , { 200, 0 } , HASH_STR( "Search..." ) , I_SEARCH ) )
			{
				SearchManager::get( ).update( );
			}

			if ( ac && !ImGui::IsItemActive( ) && SearchManager::get( ).search_buf [ 0 ] == '\0' )
			{
				ImGui::SetKeyboardFocusHere( -1 );
			}

			ac = IsItemActive( );

			PopItemFlag( );
			PopStyleVar( 2 );
			PopStyleColor( );

			static float tabs_size = 0;
			SetCursorPos( { GetWindowWidth( ) - tabs_size - 21, 0 } );
			float p1 = GetCurrentWindow( )->DC.CursorPos.x;
			BeginChild( HASH_STR( "tabs" ) , { 0, GetWindowHeight( ) } , ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX );
			TabsManager::get( ).render_tabs( 13 , true );
			EndChild( );
			SameLine( 0 , 0 );
			tabs_size = GetCurrentWindow( )->DC.CursorPos.x - p1;
		}
		EndChild( );

		SetCursorPos( { 0, 56 } );
		PushStyleColor( ImGuiCol_ChildBg , ImColor { 0, 0, 0 }.Value );
		PushStyleColor( ImGuiCol_Border , ImColor { 255, 231, 231, 20 }.Value );
		BeginChild( HASH_STR( "main" ) , { 0, -40 } , ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_Border , ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
		{
			PopStyleColor( 2 );

			if ( strlen( SearchManager::get( ).search_buf ) == 0 && SearchManager::get( ).get_anim( ) < 0.05f )
			{
				TabsManager::get( ).render_subtabs( 8 );

				auto window = GetCurrentWindow( );

				PushStyleVar( ImGuiStyleVar_Alpha , GImGui->Style.Alpha * TabsManager::get( ).get_anim( ) * GImGui->Style.Alpha );
				PushStyleVar( ImGuiStyleVar_WindowPadding , { 14, 14 } );
				BeginChild( HASH_STR( "content" ) , { 0, 0 } , ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
				{
					TabsManager::get( ).draw_page( window );
				}
				EndChild( );
				PopStyleVar( 2 );
			}
			else
			{
				PushStyleVar( ImGuiStyleVar_WindowPadding , { 14, 14 } );
				BeginChild( HASH_STR( "content2" ) , { 0, 0 } , ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
				{
					ChildManager::get( ).smoothscroll( );
					SearchManager::get( ).draw( );
				}
				EndChild( );
				PopStyleVar( );
			}
		}
		EndChild( );

		SetCursorPosY( GetCursorPosY( ) - GImGui->Style.ItemSpacing.y );
		BeginChild( HASH_STR( "footer" ) , { 0, 0 } , 0 , ImGuiWindowFlags_NoBackground );
		{
			SetCursorPos( { GetWindowHeight( ) / 2 - 12, GetWindowHeight( ) / 2 - 13 } );
			CompBuilder::get( ).OpenButton( HASH_STR( "lang selector" ) , { 32 + fonts->get( HASH_STR( "Geist-SemiBold.ttf" ), 12.00f )->CalcTextSizeA( 12, FLT_MAX, -1, LangManager::get( ).get_lang_name( ).c_str( ) ).x, 24 } , [ & ] ( CompBuilder::OpenButtonEnv env )
				{
					GetWindowDrawList( )->AddRectFilled( env.bb.Min , env.bb.Max , GetColorU32( ImGuiCol_ChildBg ) , GImGui->Style.FrameRounding );
					GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Glyphter.tff" ) , 12.00f ) , 12 , { env.bb.Min.x + 6, env.bb.GetCenter( ).y - 6 } , GetColorU32( ImGuiCol_Scheme ) , I_LANGUAGE__KATAKANA );
					GetWindowDrawList( )->AddText( fonts->get( HASH_STR( "Geist-SemiBold.ttf" ) , 12.00f ) , 12 , { env.bb.Min.x + 26, env.bb.GetCenter( ).y - 6 } , GetColorU32( ImGuiCol_Text ) , LangManager::get( ).get_lang_name( ).c_str( ) );

					if ( env.pressed )
					{
						env.open = !env.open;
					}

					if ( env.hovered ) env.open = true;

					if ( env.anim.open > 0.05f )
					{
						PushStyleVar( ImGuiStyleVar_Alpha , env.anim.open );
						PushStyleVar( ImGuiStyleVar_ItemSpacing , { 0, 0 } );
						PushStyleVar( ImGuiStyleVar_WindowRounding , GImGui->Style.FrameRounding );
						PushStyleVar( ImGuiStyleVar_WindowPadding , { 0, 8 } );
						PushStyleVar( ImGuiStyleVar_FramePadding , { 10, 8 } );
						PushStyleVar( ImGuiStyleVar_WindowBorderSize , 1 );
						PushStyleColor( ImGuiCol_WindowBg , GetColorU32( ImGuiCol_FrameBg ) );
						Begin( HASH_STR( "lang selector" ) , 0 , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove );
						{
							SetWindowPos( { env.bb.Min.x, env.bb.GetCenter( ).y - GetWindowHeight( ) / 2 } );
							SetWindowSize( { 100, ( GetCurrentWindow( )->ContentSize.y + GImGui->Style.WindowPadding.y * 2 ) * env.anim.open } );

							BringWindowToDisplayFront( GetCurrentWindow( ) );
							BringWindowToFocusFront( GetCurrentWindow( ) );

							if ( !IsWindowHovered( ImGuiHoveredFlags_AllowWhenBlockedByActiveItem ) && !env.hovered )
							{
								env.open = false;
							}

							PushStyleColor( ImGuiCol_FrameBg , GetColorU32( ImGuiCol_FrameBgHovered ) );

							for ( int i = 0; i < LangManager::get( ).get_langs( ).size( ); ++i )
							{
								if ( WidgetsManager::get( ).Selectable( LangManager::get( ).get_langs( ) [ i ].label , LangManager::get( ).get_lang( ) == i ) )
								{
									LangManager::get( ).set_lang( i );
								}
							}

							PopStyleColor( );
						}
						End( );
						PopStyleColor( );
						PopStyleVar( 6 );
					}
				} );

			//std::string key_text = HASH_STR( "Menu: " ) + std::string( key_name_list [ vars->ui.menu_key ] );
			//ImVec2 key_text_size = ImGui::CalcTextSize( key_text.c_str( ) );

			//GetWindowDrawList( )->AddText( GetWindowPos( ) + ImVec2 { GetWindowWidth( ) - 15 - key_text_size.x, GetWindowHeight( ) / 2 - GImGui->FontSize / 2 - 1 }, GetColorU32( ImGuiCol_Text ), key_text.c_str( ) );

		}
		EndChild( );

		PopupManager::get( ).handle( );
	}
	End( );
}

#endif // !MENU_CPP