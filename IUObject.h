#ifndef IUOBJECT_H
#define IUOBJECT_H

#include <iostream>

class IUObject
{
    public:

    virtual void set_property( std::string property , void* data ) = 0;

    virtual void* get_property ( std::string property ) = 0;

};

#endif // IUOBJECT_H
