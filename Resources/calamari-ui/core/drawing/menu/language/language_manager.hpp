#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class LangManager {
    int lang = 0;

    struct Lang {
        std::string label;
        std::string font;
        std::unordered_map<std::string, const char8_t*> dict { };
    };

    std::vector<Lang> languages;

public:
    void add_language( std::string label, std::string font, std::unordered_map<std::string, const char8_t*> dict ) {
        languages.push_back( Lang { label, font, dict } );
    }

    int& get_lang( ) {
        return lang;
    }

    std::string get_lang_name( ) {
        return languages [ lang ].label;
    }

    std::vector<Lang>& get_langs( ) {
        return languages;
    }

    ImFont* get_font( int sz ) {
        return fonts->get( languages [ lang ].font, ( float ) sz );
    }

    void set_lang( int i ) {
        lang = i;
    }

    std::string translate( std::string str ) {
        if ( !str.data( ) ) return "";

        if ( lang == 0 )
            return str;

        auto it = languages [ lang ].dict.find( str );
        if ( it == languages [ lang ].dict.end( ) ) return str;

        return reinterpret_cast< const char* >( it->second );
    }

    void initialize( ) {
        add_language( HASH_STR( "English" ), HASH_STR( "Geist-SemiBold.ttf" ), { } );
    }

    static LangManager& get( ) {
        static LangManager s;
        return s;
    }
};