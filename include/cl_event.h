#ifndef __clLib__cl_event__
#define __clLib__cl_event__

#include <iostream>
#include "cl_external_libraries.h"

namespace CL{
    class event {
        cl_event event_;
        
        struct event_callback_t{
            std::string event_name;
            std::ostream *stream;
        };
        event_callback_t user_data; //Callback info
        static void CL_CALLBACK callback(cl_event, cl_int, void *);
    protected:
        static std::string status_to_string(cl_int status) ;
        static std::string command_to_string(cl_command_type type) ;
    public:
        event();
        event(cl_event e);
        event(CL::event &e);
        event(CL::event &&e);
        ~event();
        
        void operator=(const CL::event &c);
        
        //info
        cl_command_type command_type() const;
        std::string command_type_name() const;
        cl_bool is_valid() const;

        cl_int event_status() const;
        std::string event_status_name() const;
        
        //Actions
        void enable_callback(const std::string &event_name,std::ostream *stream = &std::cout);
        void wait();
        
        cl_event handle() const;
    };
}

#endif /* defined(__clLib__cl_event__) */