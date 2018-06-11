#include "cl_exception.h"

std::string CL::exception::error_name(cl_int error) const{
#define CHECK_AND_RET(E_NAME) case E_NAME: return #E_NAME; break
    switch (error) {
            CHECK_AND_RET(CL_SUCCESS                                  );
            CHECK_AND_RET(CL_DEVICE_NOT_FOUND                         );
            CHECK_AND_RET(CL_DEVICE_NOT_AVAILABLE                     );
            CHECK_AND_RET(CL_COMPILER_NOT_AVAILABLE                   );
            CHECK_AND_RET(CL_MEM_OBJECT_ALLOCATION_FAILURE            );
            CHECK_AND_RET(CL_OUT_OF_RESOURCES                         );
            CHECK_AND_RET(CL_OUT_OF_HOST_MEMORY                       );
            CHECK_AND_RET(CL_PROFILING_INFO_NOT_AVAILABLE             );
            CHECK_AND_RET(CL_MEM_COPY_OVERLAP                         );
            CHECK_AND_RET(CL_IMAGE_FORMAT_MISMATCH                    );
            CHECK_AND_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED               );
            CHECK_AND_RET(CL_BUILD_PROGRAM_FAILURE                    );
            CHECK_AND_RET(CL_MAP_FAILURE                              );
            CHECK_AND_RET(CL_MISALIGNED_SUB_BUFFER_OFFSET             );
            CHECK_AND_RET(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
            CHECK_AND_RET(CL_COMPILE_PROGRAM_FAILURE                  );
            CHECK_AND_RET(CL_LINKER_NOT_AVAILABLE                     );
            CHECK_AND_RET(CL_LINK_PROGRAM_FAILURE                     );
            CHECK_AND_RET(CL_DEVICE_PARTITION_FAILED                  );
            CHECK_AND_RET(CL_KERNEL_ARG_INFO_NOT_AVAILABLE            );
            
            CHECK_AND_RET(CL_INVALID_VALUE                            );
            CHECK_AND_RET(CL_INVALID_DEVICE_TYPE                      );
            CHECK_AND_RET(CL_INVALID_PLATFORM                         );
            CHECK_AND_RET(CL_INVALID_DEVICE                           );
            CHECK_AND_RET(CL_INVALID_CONTEXT                          );
            CHECK_AND_RET(CL_INVALID_QUEUE_PROPERTIES                 );
            CHECK_AND_RET(CL_INVALID_COMMAND_QUEUE                    );
            CHECK_AND_RET(CL_INVALID_HOST_PTR                         );
            CHECK_AND_RET(CL_INVALID_MEM_OBJECT                       );
            CHECK_AND_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR          );
            CHECK_AND_RET(CL_INVALID_IMAGE_SIZE                       );
            CHECK_AND_RET(CL_INVALID_SAMPLER                          );
            CHECK_AND_RET(CL_INVALID_BINARY                           );
            CHECK_AND_RET(CL_INVALID_BUILD_OPTIONS                    );
            CHECK_AND_RET(CL_INVALID_PROGRAM                          );
            CHECK_AND_RET(CL_INVALID_PROGRAM_EXECUTABLE               );
            CHECK_AND_RET(CL_INVALID_KERNEL_NAME                      );
            CHECK_AND_RET(CL_INVALID_KERNEL_DEFINITION                );
            CHECK_AND_RET(CL_INVALID_KERNEL                           );
            CHECK_AND_RET(CL_INVALID_ARG_INDEX                        );
            CHECK_AND_RET(CL_INVALID_ARG_VALUE                        );
            CHECK_AND_RET(CL_INVALID_ARG_SIZE                         );
            CHECK_AND_RET(CL_INVALID_KERNEL_ARGS                      );
            CHECK_AND_RET(CL_INVALID_WORK_DIMENSION                   );
            CHECK_AND_RET(CL_INVALID_WORK_GROUP_SIZE                  );
            CHECK_AND_RET(CL_INVALID_WORK_ITEM_SIZE                   );
            CHECK_AND_RET(CL_INVALID_GLOBAL_OFFSET                    );
            CHECK_AND_RET(CL_INVALID_EVENT_WAIT_LIST                  );
            CHECK_AND_RET(CL_INVALID_EVENT                            );
            CHECK_AND_RET(CL_INVALID_OPERATION                        );
            CHECK_AND_RET(CL_INVALID_GL_OBJECT                        );
            CHECK_AND_RET(CL_INVALID_BUFFER_SIZE                      );
            CHECK_AND_RET(CL_INVALID_MIP_LEVEL                        );
            CHECK_AND_RET(CL_INVALID_GLOBAL_WORK_SIZE                 );
            CHECK_AND_RET(CL_INVALID_PROPERTY                         );
            CHECK_AND_RET(CL_INVALID_IMAGE_DESCRIPTOR                 );
            CHECK_AND_RET(CL_INVALID_COMPILER_OPTIONS                 );
            CHECK_AND_RET(CL_INVALID_LINKER_OPTIONS                   );
            CHECK_AND_RET(CL_INVALID_DEVICE_PARTITION_COUNT           );
        default:
            return UNKNOW_NERROR;
            break;
    }
}

cl_int CL::exception::error_code() const{
    return error_;
}

std::string CL::exception::error_name() const{
    return error_name(error_);
}

bool CL::exception::is_known() const{
    return error_name(error_).compare(UNKNOW_NERROR);
}

