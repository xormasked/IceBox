#ifndef INTERFACE_HPP
#define INTERFACE_HPP

namespace menu_interface {

	class c_interface {
	private:
		ImVec2 size;

	public:
		c_interface( ) { };
		~c_interface( ) = default;

		void draw( );
		void initialize( );
	};

} inline const auto menu = std::make_shared<menu_interface::c_interface>( );

namespace definitions
{
	static std::vector<std::string> visuals_cateogries
	{
		HASH_STR( "Players" ),
		HASH_STR( "Teammates" ),
		HASH_STR( "Wounded" ),
		HASH_STR( "Scientists" ),
		HASH_STR( "Sleepers" ),
		HASH_STR( "Safezone" ),
	};

	static std::vector<multi_select_item> elements
	{
		HASH_STR( "Boxes" ),
		HASH_STR( "Skeletons" ),
		HASH_STR( "Username" ),
		HASH_STR( "Distance" ),
		HASH_STR( "Held Item" ),
		HASH_STR( "Playtime" ),
		HASH_STR( "Team ID" ),
		HASH_STR( "View Direction" )
	};

	static std::vector<std::string> aim_categories
	{
		HASH_STR( "Rifles" ),
		HASH_STR( "LMGs" ),
		HASH_STR( "SMGs" ),
		HASH_STR( "Shotguns" ),
		HASH_STR( "Snipers" ),
		HASH_STR( "Pistols" ),
		HASH_STR( "Bows" ),
		HASH_STR( "Blowpipes" ),
		HASH_STR( "Launchers" ),
		HASH_STR( "Specials" ),
		HASH_STR( "Global" ),
	};

	static std::vector<std::string> aim_hitboxes 
	{
		HASH_STR( "Head" ),
		HASH_STR( "Torso" ),
		HASH_STR( "Closest" ),
		HASH_STR( "Randomized" )
	};

	static std::vector<std::string> override_hitboxes
	{
		HASH_STR( "Head" ),
		HASH_STR( "Torso" ),
		HASH_STR( "Randomized" )
	};

	static std::vector<std::string> override_patrol_hitboxes
	{
		HASH_STR( "Tail Rotor" ),
		HASH_STR( "Main Rotor" ),
		HASH_STR( "Body" ),
		HASH_STR( "Smart Override" )
	};

	static std::vector<multi_select_item> target_categories
	{
		HASH_STR( "NPCs" ),
		HASH_STR( "Wounded" ),
		HASH_STR( "Sleepers" ),
		HASH_STR( "Friendlies" ),
		HASH_STR( "Safezones" ),
	};

	static std::vector<multi_select_item> bone_names
	{
		HASH_STR( "Head" ),
		HASH_STR( "Neck" ),
		HASH_STR( "Pelvis" ),
		HASH_STR( "Spine 1" ),
		HASH_STR( "Spine 4" ),
		HASH_STR( "L Upper Arm" ),
		HASH_STR( "R Upper Arm" ),
		HASH_STR( "L Forearm" ),
		HASH_STR( "R Forearm" ),
		HASH_STR( "L Hand" ),
		HASH_STR( "R Hand" ),
		HASH_STR( "L Hip" ),
		HASH_STR( "R Hip" ),
		HASH_STR( "L Knee" ),
		HASH_STR( "R Knee" ),
		HASH_STR( "L Foot" ),
		HASH_STR( "R Foot" )
	};

	static std::vector<multi_select_item> hotbar_categories
	{
		HASH_STR( "Belt" ),
		HASH_STR( "Clothing" )
	};

	static std::vector<std::string> recoil_modes
	{
		HASH_STR( "Basic" ),
		HASH_STR( "Advanced" ),
	};

	static std::vector<std::string> eoka_modes
	{
		HASH_STR( "Always Hit" ),
		HASH_STR( "Never Hit" ),
	};

	static std::vector<multi_select_item> item_categories 
	{
		HASH_STR( "Weapon" ),
		HASH_STR( "Construction" ),
		HASH_STR( "Items" ),
		HASH_STR( "Resources" ),
		HASH_STR( "Attire" ),
		HASH_STR( "Tool" ),
		HASH_STR( "Medical" ),
		HASH_STR( "Food" ),
		HASH_STR( "Ammunition" ),
		HASH_STR( "Traps" ),
		HASH_STR( "Misc" ),
		HASH_STR( "All" ),
		HASH_STR( "Common" ),
		HASH_STR( "Component" ),
		HASH_STR( "Search" ),
		HASH_STR( "Favourite" ),
		HASH_STR( "Electrical" ),
		HASH_STR( "Fun" )
	};
}

using namespace definitions;

#endif 