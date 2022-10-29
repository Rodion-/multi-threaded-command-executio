#ifndef MEVENTLOOP_H
#define MEVENTLOOP_H

#include "UObject.h"
#include "command.h"
#include "thread"
#include "queue"
#include "mutex"

class mEventLoop
{
    std::queue<ICommand*> pool;

    std::vector<std::thread> threadpool;
    std::mutex mtx;

    bool soft_stop;
    bool hard_stop;

    public:

    mEventLoop( int n ) : soft_stop(false) , hard_stop(false)
    {
        if( n == 0 ) n = 1;

        for( int i = 0; i < n; i++ )
        {
            threadpool.emplace_back( std::thread( &mEventLoop::pop , this ) );
        }
    }
    ~mEventLoop()
    {
        for( std::thread& t : threadpool )
            t.join();
    }


    bool* get_hardf( void )
    {
        return &hard_stop;
    }
    bool* get_softf( void )
    {
        return &soft_stop;
    }
    void push( ICommand* cmd )
    {
        pool.push( cmd );
    }
    void pop( void )
    {
        try
        {
            do
            {
                if( hard_stop == true ) break;
                if( !pool.empty() )
                {
                    std::unique_lock<std::mutex> guard( mtx );

                    ICommand* cmd = pool.front();
                    pool.pop();

                    guard.unlock();

                    cmd->execute();


                } else
                    if( soft_stop == false )
                    {
//                        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
                    } else
                        if( soft_stop == true ) break;

            } while( true );

        }
        catch ( int i )
        {
            std::cout<<"exception"<<std::endl;
            this->pop();
        }
    }
};


#endif // MEVENTLOOP_H
