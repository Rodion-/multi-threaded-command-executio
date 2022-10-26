#ifndef UOBJECT_H
#define UOBJECT_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IUObject.h"

typedef struct position
{
    int x;
    int y;
} position;

typedef struct velocity
{
    int x;
    int y;
} velocity;


class UObject : public IUObject
{

    position coordinates;
    velocity v;
    int fuel;

    int angle;


    public:

    UObject(){}
    ~UObject(){}

    void set_property ( std::string property , void* data )
    {

    }

    void* get_property ( std::string property )
    {

    }

};

class UObjectMock : public IUObject
{
    position coordinates;
    velocity v;
    int fuel;
    bool is_movable;
    int angle;

    public:

    MOCK_METHOD(void*, get_property, (std::string) );

    MOCK_METHOD(void, set_property, (std::string , void* ) );

    void DelegateToFake() {
        ON_CALL(*this, set_property).WillByDefault([this](std::string property , void* data ) {

            int temp = 0;

            if( property == "position") coordinates = *( ( position* ) data );
            else if( property == "fuel") fuel = *( ( int* ) data ) ;
            else if( property == "velocity" ) v = *( ( velocity* ) data );
            else if( property == "is_movable" ) is_movable = *( ( bool* ) data );
            else if( property == "angle" ) angle = *( ( int* ) data ) ;
            else temp = 20;

        });

        ON_CALL(*this, get_property).WillByDefault([this](std::string property )->void* {

            int temp = 0;

            if( property == "position") return &coordinates;
            else if( property == "fuel") return &fuel;
            else if( property == "velocity" ) return &v;
            else if( property == "is_movable" ) return &is_movable;
            else if( property == "angle" ) return &angle;

            else temp = 40;

        });
    }
};

#endif // UOBJECT_H
