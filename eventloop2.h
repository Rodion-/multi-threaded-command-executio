#ifndef EVENTLOOP2_H
#define EVENTLOOP2_H


#include "UObject.h"
#include "command.h"
#include "thread"
#include "queue"
#include "mutex"
#include <future>
#include <exception>

class mEventLoop2
{
    std::queue<ICommand*> pool;

    std::vector<std::thread> threadpool;
    std::mutex mtx;

    bool soft_stop;
    bool hard_stop;

    std::vector< std::future<bool> > pool_future;

    public:

    mEventLoop2( int n ) : soft_stop(false) , hard_stop(false)
    {
        if( n == 0 ) n = 1;

        for( int i = 0; i < n; i++ )
        {
            std::promise<bool> p;
            pool_future.push_back( p.get_future() );

            threadpool.push_back( std::thread( &mEventLoop2::pop , this , std::move( p ) ) );
        }
    }
    ~mEventLoop2()
    {
        for( std::thread& t : threadpool )
            t.join();
    }

    bool is_empty( void )
    {
       return pool.empty();
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

    bool thread_status( int n )
    {
        auto res = pool_future[ n ].get();
        return res;
    }
    void pop( std::promise<bool> p )
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

                    delete cmd;

                } else
                {
                    if( soft_stop == true ) break;
                }
            } while( true );

            p.set_value( true );

        }
        catch (std::exception&)
        {
            std::cout<<"exception>>>>>>>>>>>>"<<std::endl;
            p.set_exception(std::current_exception());
        }

        catch( ... )
        {
            std::cout<<"exception----"<<std::endl;
//            this->pop();
        }
    }
};


#endif // EVENTLOOP2_H
