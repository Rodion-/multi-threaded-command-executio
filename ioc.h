#ifndef IOC_H
#define IOC_H

#include <unordered_map>
#include <iostream>
#include "command.h"

template<typename T>
class Ioc
{
    using types = std::unordered_map< std::string , T*(*)( std::string argv[] ) >;
    types m_types;

    std::unordered_map< std::string , types > map_scope;

    public:

    Ioc(){}
    ~Ioc(){}

    T* Resolve( std::string type , std::string argv[] )
    {
        auto result = m_types.find( type );

        if( result != m_types.end() )
        {
            result->second( argv );
        }
    }

    void Register( std::string type , T*(*create)( std::string argv[] ) )
    {
        m_types.insert( std::make_pair( type , create ) );
    }
};


#endif // IOC_H
