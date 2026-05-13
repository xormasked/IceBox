#ifndef COLOR_HPP
#define COLOR_HPP

class c_color {
public:
	float r { }, g { }, b { }, a { };

public:
	c_color( ) : r( 0.00f ), g( 0.00f ), b( 0.00f ), a( 0.00f ) { };
	c_color( float r, float g, float b, float a = 1.00f ) : r( r ), g( g ), b( b ), a( a ) { };
	c_color( int r, int g, int b, int a = 255 ) : r( r / 255.00f ), g( g / 255.00f ), b( b / 255.00f ), a( a / 255.00f ) { };

public:
	float* to_array( ) { return reinterpret_cast< float* >( this ); };
	ImColor to_im_color( ) const { return ImColor( r, g, b, a ); }

public:
	static ImColor green_to_red( float factor );
};

#endif // !COLOR_HPP
