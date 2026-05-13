#ifndef OPTION_HPP
#define OPTION_HPP

namespace option_interface {

	enum class c_keybind_mode : u8 {
		hold,
		toggle,
		always
	};

	class c_keybind {
	public:
		int key { };
		bool was_down { };
		c_keybind_mode mode = c_keybind_mode::hold;

	public:
		bool is_active( ) const;
		bool did_press( bool inputs = true );
	};

	class c_option {
	public:
		bool state { };
		bool previous_state { };

		c_keybind keybind;
		c_color color = c_color( 255, 255, 255 );

	public:
		bool is_enabled( ) const { return state; }
		bool was_enabled( ) const { return previous_state && !state; }
		void update( ) { previous_state = state; }
	};
}

#endif // !OPTION_HPP
