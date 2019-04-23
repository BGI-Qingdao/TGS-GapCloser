#include "gzstream.h"
#include <fstream>
#include "file_writer.h"

namespace BGIQD{
namespace FILES{

    std::ostream * FileWriterFactory::GenerateWriterFromFileName( const std::string & file_name )
    {
        std::ostream * ret = NULL;
        if( file_name.rfind(".gz") == file_name.length() - 3 )
        {// gzip file
            ret = (new ogzstream(file_name.c_str()));
        }
        else
        { // treat as txt file now //TODO
            ret = (new std::ofstream(file_name.c_str()));
        }
        if (ret && ! ret->good() )
        {
            delete ret ;
            ret = NULL;
        }
        return ret;
    }


    void FileWriterFactory::ResizeBuff( std::ostream & ist, size_t size )
    {
        char * buffer = (char *) calloc( sizeof(char) , size );
        if( buffer == 0 )
        {
            std::cerr<<"Fatal buffer calloc failed . System has no more memory ."<<std::endl;
            exit(1);
        }
        (ist.rdbuf())->pubsetbuf(buffer,size);
    }

}
}
