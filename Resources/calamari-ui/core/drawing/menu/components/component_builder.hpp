#pragma once

class CompBuilder {
public:
	struct EmptyEnv {
		ImGuiID& id;
		bool hovered;
		bool held;
		bool pressed;
		const char* label;
	};

	void Empty( const char* label, ImRect total_bb, ImRect bb, std::function< void( const EmptyEnv& ) > code );

	struct ButtonEnv {
		ImRect bb;

		bool hovered;
		bool held;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float anim;
		} anim;
	};

	bool Button( const char* label, ImVec2 size, std::function< void( const ButtonEnv& ) > code );

	struct SliderEnv {
		bool hovered;
		bool held;
		char* buf;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float anim;
			float val_anim;
		} anim;
	};

	template < typename T >
	bool Slider( const char* label, T* v, T min, T max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code );
	bool SliderInt( const char* label, int* v, int min, int max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code );
	bool SliderFloat( const char* label, float* v, float min, float max, const char* format, ImRect total_bb, ImRect bb, std::function< void( SliderEnv ) > code );

	struct CheckboxEnv {
		bool hovered;
		bool held;
		bool pressed;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float enabled;
			float anim;
		} anim;
	};

	bool Checkbox( const char* label, bool* v, int* key, float* col, std::function< void( ) > options, bool warning, ImRect total_bb, ImRect bb, ImVec2 options_pos, std::function< void( CheckboxEnv ) > code );

	struct ComboEnv {
		bool hovered;
		bool held;
		bool pressed;
		bool& open;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float open;
			float anim;
		} anim;
	};

	bool Combo( const char* label, ImRect total_bb, ImRect bb, std::function< void( ComboEnv ) > code );

	struct SelectableEnv {
		bool hovered;
		bool held;
		bool pressed;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float selected;
			float anim;
		} anim;
	};

	bool Selectable( const char* label, bool selected, ImRect bb, std::function< void( const SelectableEnv& ) > code );

	struct ColorEditEnv {
		bool hovered;
		bool held;
		bool pressed;
		bool& open;
		const char* label;

		struct Anim {
			float hover;
			float held;
			float open;
			float anim;
		} anim;
	};

	bool ColorEdit( const char* label, ImRect total_bb, ImRect bb, float col [ 4 ], std::function< void( ColorEditEnv ) > code );

	struct BinderEnv {
		ImRect total_bb;
		ImRect bb;

		bool hovered;
		bool held;
		bool pressed;
		bool active;
		const char* label;

		std::vector< const char* > keys;

		struct Anim {
			float hover;
			float held;
			float active;
			float anim;
		} anim;
	};

	bool Binder( const char* label, int* key, std::function< void( const BinderEnv& ) > code );

	struct OpenButtonEnv {
		ImRect bb;
		bool hovered;
		bool held;
		bool pressed;
		bool& open;

		struct Anim {
			float hover;
			float held;
			float anim;
			float open;
		} anim;
	};

	bool OpenButton( const char* str_id, ImVec2 size, std::function< void( OpenButtonEnv ) > code, bool def = false );

	static CompBuilder& get( ) {
		static CompBuilder s { };
		return s;
	}
};

inline std::vector< const char* > key_name_list = {
	"...", // 0
	"M1", // 1
	"M2",  // 2
	"",  // 3
	"M3",    // 4
	"M4",  // 5
	"M5",  // 6
	"",  // 7
	"BACKSPACE",    // 8
	"TAB",    // 9
	"",    // 10
	"",    // 11
	"",    // 12
	"ENTER",    // 13
	"",    // 14
	"",    // 15
	"",    // 16
	"",    // 17
	"",    // 18
	"PB",    // 19
	"CAPS",    // 20
	"",    // 21
	"",    // 22
	"",    // 23
	"",    // 24
	"",    // 25
	"",    // 26
	"ESC",    // 27
	"",    // 28
	"",    // 29
	"",    // 30
	"",    // 31
	"SPACE",    // 32
	"PU",    // 33
	"PD",    // 34
	"END",    // 35
	"HOME",    // 36
	"LEFT",    // 37
	"UP",    // 38
	"RIGHT",    // 39
	"DOWN",    // 40
	"",    // 41
	"",    // 42
	"",    // 43
	"",    // 44
	"INS",    // 45
	"DEL",    // 46
	"",    // 47
	"0",    // 48
	"1",    // 49
	"2",    // 50
	"3",    // 51
	"4",    // 52
	"5",    // 53
	"6",    // 54
	"7",    // 55
	"8",    // 56
	"9",    // 57
	"...",    // 58
	"",    // 59
	"",    // 60
	"",    // 61
	"",    // 62
	"",    // 63
	"",    // 64
	"A",    // 65
	"B",    // 66
	"C",    // 67
	"D",    // 68
	"E",    // 69
	"F",    // 70
	"G",    // 71
	"H",    // 72
	"I",    // 73
	"J",    // 74
	"K",    // 75
	"L",    // 76
	"M",    // 77
	"N",    // 78
	"O",    // 79
	"P",    // 80
	"Q",    // 81
	"R",    // 82
	"S",    // 83
	"T",    // 84
	"U",    // 85
	"V",    // 86
	"W",    // 87
	"X",    // 88
	"Y",    // 89
	"Z",    // 90
	"WIN",    // 91
	"",    // 92
	"",    // 93
	"",    // 94
	"",    // 95
	"",    // 96
	"",    // 97
	"",    // 98
	"",    // 99
	"",    // 100
	"",    // 101
	"",    // 102
	"",    // 103
	"",    // 104
	"",    // 105
	"",    // 106
	"",    // 107
	"",    // 108
	"",    // 109
	"",    // 110
	"",    // 111
	"F1",    // 112
	"F2",    // 113
	"F3",    // 114
	"F4",    // 115
	"F5",    // 116
	"F6",    // 117
	"F7",    // 118
	"F8",    // 119
	"F9",    // 120
	"F10",    // 121
	"F11",    // 122
	"F12",    // 123
	"",    // 124
	"",    // 125
	"",    // 126
	"",    // 127
	"",    // 128
	"",    // 129
	"",    // 130
	"",    // 131
	"",    // 132
	"",    // 133
	"",    // 134
	"",    // 135
	"",    // 136
	"",    // 137
	"",    // 138
	"",    // 139
	"",    // 140
	"",    // 141
	"",    // 142
	"",    // 143
	"",    // 144
	"SL",    // 145
	"",    // 146
	"",    // 147
	"`",    // 148
	"",    // 149
	"",    // 150
	"",    // 151
	"",    // 152
	"",    // 153
	"",    // 154
	"",    // 155
	"",    // 156
	"",    // 157
	"",    // 158
	"",    // 159
	"LSHIFT",    // 160
	"RSHIFT",    // 161
	"LCTRL",    // 162
	"RCTRL",    // 163
	"LALT",    // 164
	"RALT",    // 165
	"",    // 166
	"",    // 167
	"",    // 168
	"",    // 169
	"",    // 170
	"",    // 171
	"",    // 172
	"",    // 173
	"",    // 174
	"",    // 175
	"",    // 176
	"",    // 177
	"",    // 178
	"",    // 179
	"",    // 180
	"",    // 181
	"",    // 182
	"",    // 183
	"",    // 184
	"",    // 185
	"",    // 186
	"=",    // 187
	",",    // 188
	"-",    // 189
	".",    // 190
	"/",    // 191
	"",    // 192
	"",    // 193
	"",    // 194
	"",    // 195
	"",    // 196
	"",    // 197
	"",    // 198
	"",    // 199
	"",    // 200
	"",    // 201
	"",    // 202
	"",    // 203
	"",    // 204
	"",    // 205
	"",    // 206
	"",    // 207
	"",    // 208
	"",    // 209
	"",    // 210
	"",    // 211
	"",    // 212
	"",    // 213
	"",    // 214
	"",    // 215
	"",    // 216
	"",    // 217
	"",    // 218
	"[",    // 219
	"\\",    // 220
	"]",    // 221
	"",    // 222
	"",    // 223
	"",    // 224
	"",    // 225
	"",    // 226
	"",    // 227
	"",    // 228
	"",    // 229
	"",    // 230
	"",    // 231
	"",    // 232
	"",    // 233
	"",    // 234
	"",    // 235
	"",    // 236
	"",    // 237
	"",    // 238
	"",    // 239
	"",    // 240
	"",    // 241
	"",    // 242
	"",    // 243
	"",    // 244
	"",    // 245
	"",    // 246
	"",    // 247
	"",    // 248
	"",    // 249
	"",    // 250
	"",    // 251
	"",    // 252
	"",    // 253
	"",    // 254
	"",    // 255
	"",    // 256
};

inline const char* key_name_for_vk( int vk )
{
	const int n = static_cast<int>( key_name_list.size( ) );
	if ( n <= 0 )
		return "...";
	int idx = vk;
	if ( idx < 0 )
		idx = 0;
	else if ( idx >= n )
		idx = n - 1;
	return key_name_list[ static_cast<size_t>( idx ) ];
}