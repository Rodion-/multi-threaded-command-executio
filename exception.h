#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "icommand.h"
#include <unordered_map>
#include <memory>

enum class ExceptionType
{
    ArgumentException = 0,
    ConnectionTimeout,
    TYPE2Exception,
    TYPE3Exception,
};


enum class ExceptionStaus
{
    NOTPROCESSED = 0,
    PROCESSED,
};

typedef struct Exception
{
    std::string cmd_type;
    ExceptionType ex_type;
    ExceptionStaus ex_status;
}Exception;

class ExceptionHandler
{
    using ExHdlrArgScd = std::unordered_map< ExceptionType , void(*)(void)  > ;

    using ExHdlr = std::unordered_map< std::string , std::unique_ptr<ExHdlrArgScd> >;

    ExHdlr handler;

    public:

    ExceptionHandler()
    {

    }
    ~ExceptionHandler(){}

    void Setup( std::string cmd_type ,
                ExceptionType ex ,
                void(*hdlr)(void) )
    {

        auto argscd = handler.find( cmd_type );
        if (argscd == handler.end())
        {
            std::unique_ptr<ExHdlrArgScd> ArgScd = std::make_unique<ExHdlrArgScd>();

            ArgScd->insert( { ex , hdlr } );
            handler.insert( { cmd_type , std::move( ArgScd ) } );
        }
    }

    ExceptionStaus  Handler( std::string cmd_type , ExceptionType ex )
    {
        auto argscd = handler.find( cmd_type/*cmd->type()*/ );
        if (argscd != handler.end())
        {
            auto hdlr = argscd->second->find( ex );

            if( hdlr != argscd->second->end() )
            {
                hdlr->second();
                return ExceptionStaus::PROCESSED;
            }
        }
        return ExceptionStaus::NOTPROCESSED;
    }
};

#endif // EXCEPTION_H
