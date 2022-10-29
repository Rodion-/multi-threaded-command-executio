#include "test.h"

#include <thread>


void write_to_queue( void )
{

}

int main ( int argc , char** argv )
{
    #if G_TEST_RUN
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
    #endif




    return 0;
}
