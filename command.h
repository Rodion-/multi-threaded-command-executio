#ifndef COMMAND_H
#define COMMAND_H

#include "UObject.h"
#include "icommand.h"
#include "exception.h"
#include <iostream>
#include <memory>

class CheckFuelAdapter : public ICommand , std::enable_shared_from_this<CheckFuelAdapter>
{
    std::shared_ptr<IUObject> ob;

    int fuel;

    std::string cmdtype;

    public:

    CheckFuelAdapter( std::shared_ptr<IUObject> _ob ) : ob( _ob )
    {
        cmdtype = "CheckFuel" ;
    }
    ~CheckFuelAdapter(){}


    void execute(  void ) override
     {
          fuel = *((int*)ob->get_property("fuel"));

         if( fuel == 0 ) throw Exception{ cmdtype , ExceptionType::ArgumentException , ExceptionStaus::NOTPROCESSED };
     }
};

class BurnFuelAdapter : public ICommand
{
    std::shared_ptr<IUObject> ob;

    int fuel;

    std::string cmdtype;

    public:

    BurnFuelAdapter( std::shared_ptr<IUObject> _ob ) : ob( _ob )     {
        cmdtype = "BurnFuel" ;
    }
    ~BurnFuelAdapter(){}

     void execute(  void ) override
     {
         fuel = *( ( int* ) ob->get_property( "fuel") );

         fuel--;

         ob->set_property( "fuel" , &fuel );
     }
};

class MoveAdapter : public ICommand
{
    std::shared_ptr<IUObject> ob;

    std::string cmdtype;

    position pos;
    velocity vel;

    public:

    MoveAdapter( std::shared_ptr<IUObject> _ob , position _pos , velocity _vel) : ob( _ob ) , pos( _pos ) , vel( _vel )
    {
        cmdtype = "Move" ;
    }
    ~MoveAdapter(){}

    void execute(  void ) override
    {
        if( !pos.x && !pos.y ) throw -1;
        if( vel.x && !vel.y ) throw -1;
        if( !pos.x && !pos.y && vel.x && !vel.y ) throw -1;

        pos.x = pos.x + vel.x;
        pos.y = pos.y + vel.y;

        ob->set_property( "position" , &pos );
    }
};

class macroCommandMove : public ICommand
{
    std::shared_ptr<IUObject> ob;
    position xy;
    velocity vel;

    public:

    macroCommandMove( std::shared_ptr<IUObject> _ob , position _xy , velocity _vel )
        : ob( _ob ) , xy( _xy ) , vel( _vel )
    {

    }

    ~macroCommandMove(){}

    void execute ( void )
    {
        BurnFuelAdapter BurnFuelCommand( ob );
        CheckFuelAdapter CheckFuelCommand( ob );
        MoveAdapter MoveCommand( ob , xy , vel );

        CheckFuelCommand.execute();
        MoveCommand.execute();
        BurnFuelCommand.execute();
    }
};


class ChangeVelocityAdapter : public ICommand
{
    std::shared_ptr<IUObject> ob;

    std::string cmdtype;

    velocity vel;

    public:

    ChangeVelocityAdapter( std::shared_ptr<IUObject> _ob , velocity _vel) : ob( _ob ) , vel( _vel )
    {
        cmdtype = "Velocity" ;
    }
    ~ChangeVelocityAdapter(){}

    void execute(  void ) override
    {
        bool is_movable = *( ( bool* ) ob->get_property("is_movable") );

        if( is_movable == false )
        throw Exception{ cmdtype , ExceptionType::ArgumentException , ExceptionStaus::NOTPROCESSED };

        velocity ob_vel;

        ob_vel = *( ( velocity* ) ob->get_property( "velocity" ) );

        ob_vel.x += vel.x;
        ob_vel.y += vel.y;

        ob->set_property("velocity" , &ob_vel );
    }
};

class RotationAdapter : public ICommand
{
    std::shared_ptr<IUObject> ob;

    std::string cmdtype;

    int angle;

    public:

    RotationAdapter( std::shared_ptr<IUObject> _ob , int _angle) : ob( _ob ) , angle( _angle )
    {
        cmdtype = "Rotation" ;
    }
    ~RotationAdapter(){}

    void execute() override
    {
        if( angle == 0 ) throw Exception{ cmdtype , ExceptionType::ArgumentException , ExceptionStaus::NOTPROCESSED };
        ob->set_property( "angle" , &angle );
    }
};

class macroCommandRotate : public ICommand
{
    std::shared_ptr<IUObject> ob;
    velocity vel;
    int angle;

    public:

    macroCommandRotate( std::shared_ptr<IUObject> _ob , int _angle )
        : ob( _ob ) , angle( _angle )
    {

    }

    ~macroCommandRotate(){}

    void execute ( void )
    {
        vel = *( ( velocity* ) ob->get_property( "velocity" ) );

        vel.x *= cos( angle );
        vel.y *= sin( angle );

        BurnFuelAdapter BurnFuelCommand( ob );
        CheckFuelAdapter CheckFuelCommand( ob );
        ChangeVelocityAdapter VelocityCommand( ob , vel );
        RotationAdapter RotationCommand( ob , angle);

        CheckFuelCommand.execute();
        RotationCommand.execute();
        VelocityCommand.execute();
        BurnFuelCommand.execute();
    }
};


#endif // COMMAND_H
