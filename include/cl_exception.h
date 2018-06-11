#ifndef __clUtilLib__cl_exception__
#define __clUtilLib__cl_exception__

#include <iostream>
#include <exception>
#include <sstream>
#include "cl_external_libraries.h"

namespace CL{

    class exception : public std::exception{
        cl_int error_;
        std::stringstream ss;
        mutable std::string message;
        const char *UNKNOW_NERROR  = "UNKNOWN ERROR" ;
    private:
        void stream_changed() const{
            message = ss.str().append("--------------- END ---------------\n");
        }
        std::string error_name(cl_int error) const;
    public:
        exception(cl_int error,
                  const char file[],
                  const char command[],
                  int line_nr):std::exception(),error_(error){
            ss << "------------ EXCEPTION ------------" << std::endl;
            ss << "ERROR CODE: " << error << " \""<< error_name() <<"\""<< std::endl;
            ss << "FILE " << file << std::endl;
            ss << "LINE " << line_nr << std::endl;
            ss << "FUNCTION \"" << command << "\"" << std::endl;
            stream_changed();
        }
        
        exception(const CL::exception &e){
            ss << e.ss.rdbuf();
            stream_changed();
        }
      
        virtual ~exception() throw(){
            
        }
        cl_int error_code() const;
        bool is_known() const;
        std::string error_name() const;
        
        void addAdditionalInfo(const std::string &msg,const std::string &key = "ADDITIONAL INFO"){
            ss << key << " : " << msg << std::endl;
            stream_changed();
        }
        
        virtual const char* what() const throw (){
            return message.c_str();
        }
        
    };
}
#endif /* defined(__clUtilLib__cl_exception__) */
