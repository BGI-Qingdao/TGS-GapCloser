#include "common/test/Test.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"

TEST_MODULE_INIT(FILES)

TEST(ReaderUnExisitFile)
{
    auto in = BGIQD::FILES::FileReaderFactory::GenerateReaderFromFileName("nowayexist.xyzacvbasdasdad");
    CHECK(true , (NULL ==in ) );

    if( in )
        delete in;
}
