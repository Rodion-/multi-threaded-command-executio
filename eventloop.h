#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "UObject.h"
#include "command.h"
#include "thread"
#include "queue"
#include "mutex"

class EventLoop
{
    std::queue<ICommand*> pool;
    std::thread mthread;
    std::mutex mtx;

    bool soft_stop;
    bool hard_stop;

    public:

    EventLoop() : soft_stop(false) , hard_stop(false) {}
    ~EventLoop(){ mthread.join(); }


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
        try {
        do
        {
            if( hard_stop == true ) break;
            else if( !pool.empty() )
            {
                std::unique_lock<std::mutex> guard( mtx );

                ICommand* cmd = pool.front();
                pool.pop();
                guard.unlock();

                cmd->execute();
            } else if( soft_stop == false )
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            } else if( soft_stop == true ) break;

        }while( true );

        } catch ( int i ) {
            std::cout<<"exception"<<std::endl;
//            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
    }

    void run ( void )
    {
        if( !mthread.joinable() )
            mthread = std::thread( &EventLoop::pop , this );
    }
};




#endif // EVENTLOOP_H
