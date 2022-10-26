#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "UObject.h"
#include "command.h"
#include "ioc.h"

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


//std::shared_ptr<UObjectMock> spaceship = std::make_shared<UObjectMock>();

//TEST( mocktest10 , IoC )
//{
//    Ioc<ICommand> ioc;

//    testing::Mock::AllowLeak( &spaceship );

//    spaceship->DelegateToFake();

//    ioc.Register( "move" , []( std::string argv[] )->ICommand*
//        {
//            position xy{ std::stoi( argv[ 0 ] ) , std::stoi( argv[ 1 ] ) };
//            velocity vel{ std::stoi( argv[ 2 ] ) , std::stoi( argv[ 3 ] ) };
//            return new MoveAdapter( spaceship , xy , vel );
//        }
//    );

//    std::string argv[ 10 ];
//    argv[0]="1";
//    argv[1]="2";
//    argv[2]="3";
//    argv[3]="4";

//    ioc.Resolve( "move" , argv )->execute();

//    position xy = *( (position*) spaceship->get_property( "position" ) );

//    EXPECT_EQ( xy.x , 4 );
//    EXPECT_EQ( xy.y , 6 );
//}

