#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <Windows.h>
#include "excpt.h"

namespace Memory {

	inline uint64_t ImageBase = ( uint64_t ) GetModuleHandle( NULL );

	inline bool IsValidPtr( uintptr_t ptr ) {
		__try
		{
			volatile auto result = *( uintptr_t* ) ptr;
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
		return true;
	}

	__forceinline bool valid_pointer( const void* ptr ) {
		return ptr && IsValidPtr( reinterpret_cast< uintptr_t >( ptr ) );
	}

	inline uint64_t NtCurrentPeb( )
	{
		return __readgsqword( 0x60 );
	}

	template<typename T>
	T Read( uintptr_t address );

	template<typename T>
	void Write( uintptr_t address, T data );

	uintptr_t GetAddr( uintptr_t base, std::vector<uintptr_t> offsets );

	template<typename T>
	T ReadPtr( uintptr_t base, std::vector<uintptr_t> offsets );

	template<typename T>
	void WritePtr( uintptr_t base, std::vector<uintptr_t> offsets, T data );

	template<typename T>
	T Read( uintptr_t address ) {
		if ( IsBadReadPtr( ( void* ) address, sizeof( T ) ) ) {
			if constexpr ( std::is_pointer_v<T> ) {
				return nullptr;
			}
			else {
				return T{};
			}
		}
		return *reinterpret_cast< T* >( address );
	}



	template<typename T>
	void Write( uintptr_t address, T data ) {
		if ( IsBadReadPtr( ( void* ) address, sizeof( T ) ) ) return;
		if ( IsBadWritePtr( ( LPVOID ) address, sizeof( T ) ) ) return;
		*( T* ) address = data;
	}

	inline uintptr_t GetAddr( uintptr_t base, std::vector<uintptr_t> offsets ) {
		for ( int i = 0; i < offsets.size( ); i++ ) {
			base = Read<uintptr_t>( base );
			if ( !base ) return 0;
			base += offsets[ i ];
			if ( !base ) return 0;
		}
		return base;
	}


	template<typename T>
	T ReadPtr( uintptr_t base, std::vector<uintptr_t> offsets ) {
		for ( int i = 0; i < offsets.size( ); i++ ) {
			base = Read<uintptr_t>( base );
			base += offsets[ i ];
		}
		return Read<T>( base );
	}

	template<typename T>
	void WritePtr( uintptr_t base, std::vector<uintptr_t> offsets, T data ) {
		base = GetAddr( base, offsets );
		if ( IsBadWritePtr( ( LPVOID ) base, sizeof( T ) ) ) return;
		if ( IsBadReadPtr( ( void* ) base, sizeof( T ) ) ) return;
		Write<T>( base, data );
	}

	template<typename R, typename T, typename... Args>
	__forceinline R call_virtual( T* self, std::size_t index, Args... args ) {
		if ( !self || !valid_pointer( self ) ) {
			if constexpr ( !std::is_void_v<R> ) {
				return R{};
			}
			else {
				return;
			}
		}

		void** vtable = *reinterpret_cast< void*** >( self );
		if ( !vtable || !valid_pointer( vtable ) || !valid_pointer( vtable[ index ] ) ) {
			if constexpr ( !std::is_void_v<R> ) {
				return R{};
			}
			else {
				return;
			}
		}

		return reinterpret_cast< R( __fastcall* )( T*, Args... ) >( vtable[ index ] )( self, args... );
	}

}