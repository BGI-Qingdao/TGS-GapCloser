#ifndef __COMMON_FILES_FILE_WRITER_H__
#define __COMMON_FILES_FILE_WRITER_H__

#include <ostream>
namespace BGIQD{
namespace FILES{

class FileWriterFactory{
    public:
        static std::ostream * GenerateWriterFromFileName( const std::string & file_name );
        static void ResizeBuff(std::ostream &, size_t size);

};

} //FILES
} //BGIQD

#endif //__COMMON_FILES_FILE_WRITER_H__
