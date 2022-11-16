#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "UObject.h"
#include "command.h"
#include "ioc.h"
#include "eventloop.h"
#include "meventloop.h"

TEST( mocktest1 , MoveAdapter )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    position xy{ 12 , 5 };

    MoveAdapter cmdMove( spaceship , xy , { -7 , 3 } );

    cmdMove.execute();

    xy = *( (position*) spaceship->get_property( "position" ) );

    EXPECT_EQ( xy.x , 5 );
    EXPECT_EQ( xy.y , 8 );
}

TEST( mocktest2 , CheckFuelAdapter )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    CheckFuelAdapter cmdFuel( spaceship );
    ExceptionHandler exceptionhandler;
    ExceptionStaus ex_status = ExceptionStaus::NOTPROCESSED;

    exceptionhandler.Setup( "CheckFuel",
                          ExceptionType::ArgumentException,
                          [](){ std::cout<<"Exception Rotation" <<  std::endl; }
                        );

    try
    {
        int fuel = 0;
        spaceship->set_property( "fuel" , &fuel );
        cmdFuel.execute();

    }
    catch ( Exception ex )
    {
        ex_status = exceptionhandler.Handler( ex.cmd_type , ex.ex_type );
    }

    EXPECT_EQ( (int)ex_status , ( int ) ExceptionStaus::PROCESSED );
}

TEST( mocktest3 , BurnFuelAdapter )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    CheckFuelAdapter cmdFuel( spaceship );
    BurnFuelAdapter cmdBurnFue( spaceship );

    int fuel = 99;
    spaceship->set_property( "fuel" , &fuel );
    cmdBurnFue.execute();
    fuel = *( (int*) spaceship->get_property( "fuel" ) );

    EXPECT_EQ( fuel , 98 );
}

TEST( mocktest4 , macroCommandMove )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    int fuel = 100;
    spaceship->set_property( "fuel" , &fuel );

    macroCommandMove cmdMove( spaceship , { 10 , 5 } , { -5 , 5 } );

    cmdMove.execute();

    position xy = *( ( position* ) spaceship->get_property( "position") );
    fuel = *( ( int* ) spaceship->get_property( "fuel" ) );

    EXPECT_EQ( xy.x , 5 );
    EXPECT_EQ( xy.y , 10 );
    EXPECT_EQ( fuel , 99 );
}
TEST( mocktest5 , ChangeVelocityAdapter )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    velocity v{ -5 , 7 };
    bool is_movable = true;

    spaceship->set_property( "velocity" , &v );
    spaceship->set_property( "is_movable" , &is_movable );

    ChangeVelocityAdapter cmdVelocity( spaceship , { 3 , -3 } );

    cmdVelocity.execute();

    v = *( ( velocity*) spaceship->get_property( "velocity" ) );

    EXPECT_EQ( v.x , -2 );
    EXPECT_EQ( v.y , 4 );

}
TEST( mocktest6 , ChangeVelocityAdapterException )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    velocity v{ -5 , 7 };
    bool is_movable = false;

    ExceptionHandler exceptionhandler;
    ExceptionStaus ex_status = ExceptionStaus::NOTPROCESSED;

    exceptionhandler.Setup( "Velocity",
                          ExceptionType::ArgumentException,
                          [](){ std::cout << "ChangeVelocityAdapterException" <<  std::endl; }
                        );

    try
    {
        spaceship->set_property( "velocity" , &v );
        spaceship->set_property( "is_movable" , &is_movable );

        ChangeVelocityAdapter cmdVelocity( spaceship , { 3 , -3 } );

        cmdVelocity.execute();
    }
    catch ( Exception ex )
    {
        ex_status = exceptionhandler.Handler( ex.cmd_type , ex.ex_type );
    }

    EXPECT_EQ( (int)ex_status , ( int ) ExceptionStaus::PROCESSED );
}
TEST( mocktest7 , RotationAdapter )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    RotationAdapter cmdRotate( spaceship , 45 );

    cmdRotate.execute();

    int angle = *( ( int* ) spaceship->get_property( "angle" ) );

    EXPECT_EQ( angle , 45 );
}

TEST( mocktest7 , RotationAdapterException )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    int angle = 0;

    ExceptionHandler exceptionhandler;
    ExceptionStaus ex_status = ExceptionStaus::NOTPROCESSED;

    exceptionhandler.Setup( "Rotation",
                          ExceptionType::ArgumentException,
                          [](){ std::cout << "RotationAdapterException" <<  std::endl; }
                        );

    try
    {
        RotationAdapter cmdRotate( spaceship , angle );

        cmdRotate.execute();
    }
    catch ( Exception ex )
    {
        ex_status = exceptionhandler.Handler( ex.cmd_type , ex.ex_type );
    }

    EXPECT_EQ( (int)ex_status , ( int ) ExceptionStaus::PROCESSED );
}

TEST( mocktest9 , macroCommandRotate )
{
    std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

    spaceship->DelegateToFake();

    int fuel = 100;
    int angle = 0;
    bool is_movable = true;
    velocity v{ 5 , 7 };

    spaceship->set_property( "fuel" , &fuel );
    spaceship->set_property( "velocity" , &v );
    spaceship->set_property( "is_movable" , &is_movable );

    macroCommandRotate cmdRotate( spaceship , 45 );

    cmdRotate.execute();

    angle = *( ( int* ) spaceship->get_property( "angle" ) );
    v = *( ( velocity* ) spaceship->get_property( "velocity") );

    EXPECT_EQ( angle , 45 );
    EXPECT_EQ( v.x , 5 + 2 );
    EXPECT_EQ( v.y , 7 + 5 );
}

TEST( mocktest11 , multiThread_eventLoopSoftStop )
{
    mEventLoop eloop( std::thread::hardware_concurrency() );


    std::thread t1( [&eloop]()
                     {
                        int i = 0;
                        while( i < 20 )
                        {
                            eloop.push( new testCmd( ++i ) );
                        }
                        eloop.push( new SoftStopCommand( eloop.get_softf() ) );
                     }
                    );

    std::thread t2( [&eloop]()
                     {
                        int i = 20;
                        while( i < 40 )
                        {
                            eloop.push( new testCmd( ++i ) );
                        }
                        eloop.push( new exceptionCmd() );
                     }
                    );

    std::thread t3( [&eloop]()
                     {
                        int i = 40;
                        while( i < 60 )
                        {
                            eloop.push( new testCmd( ++i ) );
                        }
                     }
                    );
    t1.join();
    t2.join();
    t3.join();

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    EXPECT_EQ( eloop.is_empty() , true );
}

TEST( mocktest12 , multiThread_eventLoopHardStop )
{
    mEventLoop eloop( std::thread::hardware_concurrency() );


    std::thread t1( [&eloop]()
                     {
                        int i = 0;
                        while( i < 20 )
                        {
                            eloop.push( new testCmd( ++i ) );
                        }
                     }
                    );

    std::thread t2( [&eloop]()
                     {
                        int i = 20;
                        while( i < 40 )
                        {
                            eloop.push( new testCmd( ++i ) );
                        }
                        eloop.push( new exceptionCmd() );
                     }
                    );

    std::thread t3( [&eloop]()
                     {
                        int i = 40;
                        while( i < 60 )
                        {
                            eloop.push( new testCmd( ++i ) );
                            if( i == 50 ) eloop.push( new HardStopCommand( eloop.get_hardf() ) );
                        }
                     }
                    );
    t1.join();
    t2.join();
    t3.join();

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    EXPECT_EQ( eloop.is_empty() , false );
}

#include "eventloop2.h"

TEST( mocktest13 , multiThread_eventLoop2HardStop )
{
    mEventLoop2 eloop2( 3 );

    try
    {


    std::thread t1( [&eloop2]()
                     {
                        int i = 100;
                        while( i < 120 )
                        {
                            eloop2.push( new testCmd( ++i ) );
                        }
                     }
                    );


    std::thread t2( [&eloop2]()
                     {
                        int i = 120;
                        while( i < 40 )
                        {
                            eloop2.push( new testCmd( ++i ) );
                        }
//                        eloop2.push( new exceptionCmd() );
                     }
                    );

    std::thread t3( [&eloop2]()
                     {
                        int i = 140;
                        while( i < 160 )
                        {
                            eloop2.push( new testCmd( ++i ) );
                            if( i == 150 ) eloop2.push( new HardStopCommand( eloop2.get_hardf() ) );
//                            if( i == 150 ) eloop2.push( new SoftStopCommand( eloop2.get_softf() ) );
                        }
                     }
                    );
    t1.join();
    t2.join();
    t3.join();

//    std::this_thread::sleep_for( std::chrono::milliseconds( 2000 ) );

    bool res[ 10 ] = { false };

    for( int i = 0; i < 3; i++ )
    {
        res[ i ] = eloop2.thread_status( i );
    }

    EXPECT_EQ( eloop2.is_empty() , false );

    EXPECT_EQ( res[ 0 ] , true );
    EXPECT_EQ( res[ 1 ] , true );
    EXPECT_EQ( res[ 2 ] , true );
    }
    catch (std::exception& e) {
      std::cout << "[exception caught: ++++++++++++++++" << e.what() << "]\n";
    }
}
