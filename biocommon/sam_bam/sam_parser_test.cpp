#include "common/test/Test.h"
#include "biocommon/sam_bam/sam_parser.h"

TEST_MODULE_INIT(SAMParser)

using namespace BGIQD::SAM;
TEST(LineParser_test)
{
    LineParser l0("@adasdasd");
    CHECK(l0.IsHead() , true);
    LineParser l1("11111    256 NC_000019.10    3624769 0   72M adasffas");
    CHECK(l1.IsHead() , false);
    auto data = l1.ParseAsMatchData();
    CHECK(data.read_name,"11111");
    CHECK(data.ref_name,"NC_000019.10");
    CHECK(data.first_match_position,3624769);
    CHECK(data.flags.num,256);
    CHECK(data.detail.infos.size(),1);
    MatchInfo d = data.detail.infos[0];
    CHECK(d.start_position_on_ref , 3624769);
    CHECK(d.end_position_on_ref , 3624769+72-1);
    CHECK(d.start_position_on_read , 0 );
    CHECK(d.end_position_on_read , 71 );
    CHECK(d.type , CIGAR::M);

    LineParser l2("11111    256 NC_000019.10    3624769 0   2H70M adasffas");
    auto data1 = l2.ParseAsMatchData();
    CHECK(data1.detail.infos.size(),2);
    MatchInfo d1 = data1.detail.infos[1];
    CHECK(d1.start_position_on_ref , 3624769);
    CHECK(d1.end_position_on_ref , 3624769+70-1);
    CHECK(d1.start_position_on_read , 2 );
    CHECK(d1.end_position_on_read , 71 );
    CHECK(d1.type , CIGAR::M);
    LineParser l3("11111    256 NC_000019.10    3624769 0   3S7M2M adasffas");
    auto data2 = l3.ParseAsMatchData();
    CHECK(data2.detail.infos.size(),3);
    MatchInfo d2 = data2.detail.infos[1];
    CHECK(d2.start_position_on_ref , 3624769);
    CHECK(d2.end_position_on_ref , 3624769+7-1);
    CHECK(d2.start_position_on_read , 3 );
    CHECK(d2.end_position_on_read , 9 );
    CHECK(d2.type , CIGAR::M);
}

TEST(FLAGS_test)
{
    FLAGS_INT i;
    i.num = 2048;
    CHECK((i.flags.ox800 & 0x1), 1 );
    CHECK((i.flags.ox400 & 0x1), 0 );
    i.num = 1024+256;
    CHECK((i.flags.ox800 & 0x1), 0 );
    CHECK((i.flags.ox400 & 0x1), 1 );
    CHECK((i.flags.ox200 & 0x1), 0 );
    CHECK((i.flags.ox100 & 0x1), 1 );
    i.num = 147 ;
}
