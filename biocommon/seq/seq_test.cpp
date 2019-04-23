#include "common/test/Test.h"
#include "biocommon/seq/seq.h"

#include <sstream>

TEST_MODULE_INIT(SeqTest)

using namespace BGIQD::SEQ;

TEST(LoadSeqTest01)
{
    seq s;
    s.AddPartSeq(std::string(300,'A'));
    std::cerr<<s.Seq(50);
    std::cerr<<s.Seq(100);
}
