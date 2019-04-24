#include "common/args/argsparser.h"
#include "common/files/file_reader.h"
#include "common/files/file_writer.h"
#include "common/error/Error.h"
#include "common/log/log.h"
#include "common/log/logfilter.h"
#include "common/freq/freq.h"
#include "common/multithread/MultiThread.h"

#include "appcommon/ScaffInfo.h"
#include "appcommon/SmithWaterman_OO.h"

#include <string>
#include <vector>
#include <sstream>
#include <cassert>
#include <map>
#include <set>
#include <algorithm>
#include <thread>

struct ReadNameMapper
{
    private:
        std::map<std::string , unsigned int > data ;
        unsigned int next = 0 ;
    public:
        unsigned int GetId(const std::string & id ,bool new_id = true  )
        {
            auto itr = data.find(id) ;

            if( itr == data.end() )
            {
                if( new_id )
                {
                    next ++ ;
                    data[id]=next ;
                    return next ;
                }
                else
                {
                    return 0 ;
                }
            }
            return itr->second;
        }
};


struct ReadInfo
{
    unsigned int order ;
    //std::string order;
    char orientation ;
    int pos ;
    int ref_id ;
};

typedef BGIQD::stLFR::SmithWaterman_OO<ReadInfo> TheSmithWaterman;

struct MapperInfo
{
    static int ont_id ;
    int ref_id ;
    std::vector<ReadInfo>  read_infos ;

    void InitFromStr_ONT( const std::string & line
            , ReadNameMapper & maper )
    {
        ont_id ++ ;
        ref_id = ont_id ;
        std::string tmp_name ;
        std::istringstream ist(line);
        ist>>tmp_name ;
        ReadInfo tmp ;
        while( ! ist.eof() )
        {
            ist>>tmp_name>>tmp.orientation>>tmp.pos;
            tmp.order = maper.GetId(tmp_name, true );
            read_infos.push_back(tmp);
            //reads.insert(tmp.order);
        }
    }

    void InitFromStr_WithFilter( const std::string & line
            , ReadNameMapper & maper )
    {
        std::istringstream ist(line);
        ist>>ref_id ;
        ReadInfo tmp ;
        std::string tmp_name;
        while( ! ist.eof() )
        {
            ist>>tmp_name>>tmp.orientation>>tmp.pos;
            tmp.order = maper.GetId(tmp_name,false);
            if( tmp.order != 0  )
            {
                read_infos.push_back(tmp);
                //reads.insert(tmp.order);
            }
        }
    }

    static std::vector<ReadInfo> CompleteReverse( const std::vector<ReadInfo> & c1 )
    {
        auto change_oo =  [](ReadInfo & i)
        { 
            if(i.orientation == '+') 
                i.orientation = '-' ; 
            else 
                i.orientation = '+' ; 
        };
        std::vector<ReadInfo> ret ; 
        ret.resize(c1.size());
        std::reverse_copy(c1.begin(),c1.end(),ret.begin());
        std::for_each(ret.begin() ,ret.end() , change_oo);
        return ret ;
    }


    std::set<unsigned int> reads() const 
    {
        std::set<unsigned int> ret;
        for( const auto & i :read_infos )
        {
            ret.insert(i.order);
        }
        return ret ;
    }
    //std::set<unsigned int> reads ;

    static std::vector<ReadInfo> Add( const std::vector<ReadInfo> & c1
            , const std::vector<ReadInfo> & c2 , bool o1 , bool o2  ) 
    {
        std::vector<ReadInfo> ret ;
        if( o1 && o2 )
        {
            std::copy(c1.begin(),c1.end(),std::back_inserter(ret)) ;
            std::copy(c2.begin(),c2.end(),std::back_inserter(ret)) ;
        }
        else if ( o1 && !o2 )
        {
            auto c2p = CompleteReverse(c2);
            std::copy(c1.begin(),c1.end(),std::back_inserter(ret)) ;
            std::copy(c2p.begin(),c2p.end(),std::back_inserter(ret)) ;
        }
        else if ( !o1 && !o2 )
        {
            auto c1p = CompleteReverse(c1);
            auto c2p = CompleteReverse(c2);
            std::copy(c1p.begin(),c1p.end(),std::back_inserter(ret)) ;
            std::copy(c2p.begin(),c2p.end(),std::back_inserter(ret)) ;
        }
        else
        {
            auto c1p = CompleteReverse(c1);
            std::copy(c1p.begin(),c1p.end(),std::back_inserter(ret)) ;
            std::copy(c2.begin(),c2.end(),std::back_inserter(ret)) ;
        }
        return ret ;
    }

    static std::vector<ReadInfo> FilterCommon( const std::vector<ReadInfo> & base , 
            const std::set<unsigned int> & common )
    {
        int total = 0 ;
        auto check = [&]( const ReadInfo & info) -> bool 
        {
            if( common.find( info.order) != common.end() )
            {
                total++ ;
                return true ;
            }
            return false ;
        };
        std::vector<ReadInfo> ret ;
        ret.resize(base.size() );
        std::copy_if( base.begin() , base.end() , ret.begin() , check) ;
        ret.resize(total);
        return ret ;
    }
};

int MapperInfo::ont_id = 0 ;

std::set<unsigned int > SetUnion( const std::set<unsigned int> & s1 ,
        const std::set<int> & s2 )
{
    std::set<unsigned int> dest1;
    std::set_union(s1.begin(), s1.end(),
            s2.begin(), s2.end(),
            std::inserter(dest1,dest1.begin()));
    return dest1 ;
}

std::set<unsigned int > SetDiff( const std::set<unsigned int > & s1 ,
        const std::set<unsigned int> & s2 )
{
    std::set<unsigned int> dest1;
    std::set_difference(s1.begin(), s1.end(),
            s2.begin(), s2.end(),
            std::inserter(dest1,dest1.begin()));
    return dest1 ;
}

std::set<unsigned int> SetIntersection( const std::set<unsigned int> & s1 ,
        const std::set<unsigned int> & s2 )
{
    std::set<unsigned int> dest1;
    std::set_intersection(s1.begin(), s1.end(),
            s2.begin(), s2.end(),
            std::inserter(dest1,dest1.begin()));
    return dest1 ;
}

struct AppConfig 
{
    std::map<int , std::set<unsigned int> > read_on_ont;

    std::map<int , MapperInfo> read2ont ;

    std::map<int, MapperInfo> read2con ;

    std::string r2ont_f ;
    std::string r2con_f ;

    BGIQD::LOG::logger loger;
    BGIQD::FREQ::Freq<std::string> failed_reason_freq;
    BGIQD::FREQ::Freq<int> common_size_freq;
    BGIQD::FREQ::Freq<int> high_score_freq;

    std::set<unsigned int> ont_checked_reads;
    ReadNameMapper stlfr_reads;

    void  Init()
    {
        BGIQD::LOG::logfilter::singleton()
            .get("ORGC",BGIQD::LOG::loglevel::INFO, loger);
    }

    void  LoadR2ONT() 
    {
        BGIQD::LOG::timer t(loger,"LoadR2ONT");
        auto in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(r2ont_f);
        if( in == NULL )
            FATAL( " failed to open r2ont for read !! exit ..." );
        auto parseline = [this]( const std::string & line ) 
        {
            MapperInfo tmp ;
            tmp.InitFromStr_ONT(line,stlfr_reads);
            read2ont[tmp.ref_id] = tmp ;
        };
        BGIQD::FILES::FileReaderFactory::EachLine(*in,parseline);
        delete in ;
    }
    void  LoadR2CON() 
    {
        BGIQD::LOG::timer t(loger,"LoadR2CON");
        auto in = BGIQD::FILES::FileReaderFactory
            ::GenerateReaderFromFileName(r2con_f);
        if( in == NULL )
            FATAL( " failed to open r2con for read !! exit ..." );
        auto parseline = [this]( const std::string & line ) 
        {
            MapperInfo tmp ;
            tmp.InitFromStr_WithFilter(line,stlfr_reads);
            read2con[tmp.ref_id] = tmp ;
        };
        BGIQD::FILES::FileReaderFactory::EachLine(*in,parseline);
        delete in ;
    }

    void BuildR2ONTIndex()
    {
        BGIQD::LOG::timer t(loger,"BuildR2ONTIndex");
        for( const auto & pair : read2ont )
        {
            auto reads = pair.second.reads();
            for( const auto & read_i : reads)
            {
                read_on_ont[read_i].insert(pair.first) ;
            }
        }
    }

    BGIQD::stLFR::ScaffInfoHelper helper ;

    void LoadScaffInfos()
    {
        BGIQD::LOG::timer t(loger,"LoadScaffInfos");
        helper.LoadAllScaff(std::cin);
    }

    int min_both_read_in_contig  ;
    int min_both_read_in_ont  ;

    bool ParseAGap(BGIQD::stLFR::ContigDetail & c1
            ,const BGIQD::stLFR::ContigDetail & c2 )
    {
        // step 0 , check
        if( read2con.find( c1.contig_id ) == read2con.end() )
        {
            failed_reason_freq.Touch("contig_less_10_reads");
            return false ;
        }
        if( read2con.find( c2.contig_id ) == read2con.end() )
        {
            failed_reason_freq.Touch("contig_less_10_reads");
            return false ;
        }

        // step 1 , find all common ONT reads

        const auto &  c1info = read2con.at((c1.contig_id)) ;
        const auto &  c2info = read2con.at((c2.contig_id)) ;
        const auto  rc1 = c1info.reads() ;
        const auto  rc2 = c2info.reads() ;

        if( (int)rc1.size() < min_both_read_in_contig || (int)rc2.size() < min_both_read_in_contig )
        {
            failed_reason_freq.Touch("contig_less_10_reads");
            return false ;
        }

        std::set<unsigned int> ont_c1 ;
        std::set<unsigned int> ont_c2 ;

        std::map<int , std::set<unsigned int> > ont_share_reads;

        for( const auto & r1 : rc1 )
        {
            auto itr = read_on_ont.find(r1);
            if( itr != read_on_ont.end() )
            {
                const auto & onts = itr->second ;
                for( const auto & ont : onts  )
                {
                    ont_share_reads[ont].insert(r1);
                    ont_c1.insert( ont );
                }
            }
        }

        for( const auto & r1 : rc2 )
        {
            auto itr = read_on_ont.find(r1);
            if( itr != read_on_ont.end() )
            {
                const auto & onts = itr->second ;
                for( const auto & ont : onts  )
                {
                    ont_share_reads[ont].insert(r1);
                    ont_c2.insert( ont );
                }
            }
        }
        auto common_ont = SetIntersection(ont_c1 , ont_c2 );
        // step 2 , filter good ONT reads
        if( common_ont.empty() )
        {
            failed_reason_freq.Touch("no common");
            return false ;
        }
        // step 3 , align c1 + c2 to ONT reads 
        std::map<int , TheSmithWaterman> align_cache ;
        for( const auto & ont : common_ont )
        {
            const auto & common = ont_share_reads.at(ont) ;
            const auto & ont_reads_base = read2ont.at(ont).read_infos ;
            auto ont_common = MapperInfo::FilterCommon( ont_reads_base , common );

            common_size_freq.Touch(ont_common.size());
            if( (int)ont_common.size() <min_both_read_in_ont )
                continue ;

            const auto & c1_reads_base = read2con.at(c1info.ref_id).read_infos;
            const auto & c2_reads_base = read2con.at(c2info.ref_id).read_infos;
            auto r1_common = MapperInfo::FilterCommon(c1_reads_base,common);
            auto r2_common = MapperInfo::FilterCommon(c2_reads_base,common);

            if( r1_common.size() <4 ||  r2_common.size() <4 )
                continue ;

            std::for_each(r1_common.begin() , r1_common.end() 
                    ,[c1](ReadInfo & i){ i.ref_id = (c1.contig_id) ; });
            std::for_each(r2_common.begin() , r2_common.end() 
                    ,[c2](ReadInfo & i){ i.ref_id = (c2.contig_id) ; });

            auto l2r = MapperInfo::Add( r1_common, r2_common 
                    , c1.orientation , c2.orientation);
            auto r2l = MapperInfo::CompleteReverse( l2r );

            BGIQD::stLFR::Schemes the_schemes ;
            the_schemes.match_orientation_score = 4 ;
            the_schemes.match_order_score = 2 ;
            the_schemes.delete_score = 0 ;
            the_schemes.insert_score = 0 ;
            the_schemes.mismatch_score= 0 ;

            TheSmithWaterman tmp ;
            tmp.schemes = the_schemes ;
            tmp.ref = ont_common ;
            tmp.query = l2r ;
            tmp.CheckLoadedData() ;
            tmp.InitAfterDataLoaded();
            tmp.FillMutrix() ;
            auto path = tmp.GetResult();
            auto path_query = tmp.AlignedElementsQuery(path);
            BGIQD::FREQ::Freq<int> freq ;
            std::for_each( path_query.begin() , path_query.end() ,
                    [&freq]( ReadInfo & i ) { freq.Touch(i.ref_id) ; } );
            if( freq.data.size() < 2 )
                tmp.max_value = 0 ;

            TheSmithWaterman tmp1 ;
            tmp1.schemes = the_schemes ;
            tmp1.ref = ont_common ;
            tmp1.query = r2l ;
            tmp1.CheckLoadedData() ;
            tmp1.InitAfterDataLoaded();
            tmp1.FillMutrix() ;
            auto path1 = tmp1.GetResult();
            auto path_query1 = tmp1.AlignedElementsQuery(path1);
            BGIQD::FREQ::Freq<int> freq1 ;
            std::for_each( path_query1.begin() , path_query1.end() ,
                    [&freq1]( ReadInfo & i ) { freq1.Touch(i.ref_id) ; } );
            if( freq1.data.size() < 2 )
                tmp1.max_value = 0 ;

            if( tmp.max_value > tmp1.max_value && tmp.max_value != 0 )
            {
                align_cache[ont]=tmp;
            }
            else if( tmp1.max_value != 0 )
            {
                align_cache[ont]=tmp1;
            }
            else
            {}
        }
        // step 5 , choose the best ONT read 
        int max_value = 0;
        TheSmithWaterman max_align ;
        for(const auto & pair : align_cache )
        {
            auto path = pair.second.GetResult();
            auto path_ref = pair.second.AlignedElementsRef(path);
            auto path_query = pair.second.AlignedElementsQuery(path);
            BGIQD::FREQ::Freq<int> freq ;
            std::for_each( path_query.begin() , path_query.end() ,
                    [&freq]( ReadInfo & i ) { freq.Touch(i.ref_id) ; } );
            if( freq.data.size() < 2 )
                continue ;
            if( pair.second.max_value > max_value )
            {
                max_value = pair.second.max_value;
                max_align = pair.second ;
            }
        }
        align_cache.clear() ;
        if( max_value < 1 )
        {
            failed_reason_freq.Touch(" no max value ");
            return  false ;
        }
        high_score_freq.Touch(max_value);
        // step 6 , estimate the gap size.
        auto path       = max_align.GetResult();
        auto path_ref   = max_align.AlignedElementsRef(path);
        auto path_query = max_align.AlignedElementsQuery(path);

        auto prev = std::adjacent_find(path_query.begin() 
                , path_query.end() 
                , [](const ReadInfo & p , const ReadInfo & n)
                { return p.ref_id != n.ref_id ; } );

        if ( prev == path_query.end() )
        {
            failed_reason_freq.Touch("all aligned in same contig ");
            return  false ;
        }
        auto prev_from_query = *prev ;
        auto next_from_query = *( prev +1 ) ;
        auto prev_from_ref = *(path_ref.begin() + ( prev - path_query.begin() ) );
        auto next_from_ref = *(path_ref.begin() + ( prev - path_query.begin() +1 ) );
        // gap = ref_gap - c1_tail - c2_tail
        int ref_gap = std::abs( prev_from_ref.pos - next_from_ref.pos -1 );
        int c1_tail = 0 ;
        int c2_tail = 0 ;

        if( prev_from_query.ref_id == (int) c1.contig_id )
        {
            // c1 == prev , c2== next
            if( c1.orientation == true )
                c1_tail = c1.contig_len - prev_from_query.pos ;
            else 
                c1_tail = prev_from_query.pos ;
            if( c2.orientation == true )
                c2_tail = next_from_query.pos ;
            else 
                c2_tail = c2.contig_len - next_from_query.pos ;
        }
        else
        {
            // c2 == prev , c1 == next
            if( c1.orientation == false )
                c1_tail = next_from_query.pos ;
            else 
                c1_tail = c1.contig_len - next_from_query.pos ;
            if( c2.orientation == false )
                c2_tail = c2.contig_len - prev_from_query.pos ;
            else 
                c2_tail = prev_from_query.pos ;
        }
        int gap = ref_gap - c1_tail - c2_tail ;
        if( gap < -max_overlap )
            gap = -max_overlap ;
        c1.gap_size = gap ;
        return true ;
    }

    int max_overlap ;


    int thread_num ;
    void ParseAllGaps()
    {
        BGIQD::LOG::timer t(loger,"ParseAllGaps");
        int succ = 0 ;
        int failed = 0 ;
        int total = 0 ;
        BGIQD::MultiThread::MultiThread multi;
        std::mutex the_mutex ;
        multi.Start(thread_num);
        for( auto & pair : helper.all_scaff ) 
        {
            auto & a_scaff = pair.second.a_scaff ;
            for( int i = 0 ; i < (int) a_scaff.size() -1 ; i ++ )
            {
                total ++ ;
                loger<<BGIQD::LOG::lstart()<<"start process gap "
                    <<total<<" ..."<<BGIQD::LOG::lend() ;
                multi.AddJob([& , i]()
                        {
                            int the_index = i ;
                            if( ParseAGap(a_scaff.at(the_index) , a_scaff.at(the_index+1)) )
                            {
                                std::lock_guard<std::mutex> locker(the_mutex);
                                succ++ ;
                            }
                            else
                            {
                                std::lock_guard<std::mutex> locker(the_mutex);
                                failed++;
                                loger<<BGIQD::LOG::lstart()<<"end process gap "
                                    <<total<<BGIQD::LOG::lend() ;
                                loger<<BGIQD::LOG::lstart()<<"total succ is  "<<succ
                                    <<" !"<<BGIQD::LOG::lend() ;
                            }
                        }
                        );
            }
        }
        multi.End();
        multi.WaitingStop();

        loger<<BGIQD::LOG::lstart()<<" Failed reason freq\n"
            <<failed_reason_freq.ToString()<<BGIQD::LOG::lend() ;
        loger<<BGIQD::LOG::lstart()<<" Common size freq\n"
            <<common_size_freq.ToString()<<BGIQD::LOG::lend() ;
        loger<<BGIQD::LOG::lstart()<<" Highest score freq\n"
            <<high_score_freq.ToString()<<BGIQD::LOG::lend() ;
    }

    void PrintScaffInfos() 
    {
        BGIQD::LOG::timer t(loger,"PrintScaffInfos");
        helper.PrintAllScaff(std::cout);
    }

} config ;

int main(int argc , char **argv)
{
    START_PARSE_ARGS
        DEFINE_ARG_REQUIRED(std::string , r2ont, "read 2 ont data ");
        DEFINE_ARG_REQUIRED(std::string , r2con, "read 2 contig data ");
        DEFINE_ARG_REQUIRED(int , thread, " thread num ");
        DEFINE_ARG_OPTIONAL(int , min_reads_from_contig , "min reads num from a contig that can start a align ","4");
        DEFINE_ARG_OPTIONAL(int , min_reads_from_ont, "min reads num from a ont that can start a align ","10");
        DEFINE_ARG_OPTIONAL(int , max_overlap, "max overlap that we can support(make sure positive num)","3000");
    END_PARSE_ARGS

    config.min_both_read_in_ont = min_reads_from_ont.to_int() ;
    config.min_both_read_in_contig = min_reads_from_contig.to_int() ;
    config.thread_num = thread.to_int() ;
    config.max_overlap = max_overlap.to_int();
    config.r2con_f = r2con.to_string() ;
    config.r2ont_f = r2ont.to_string() ;

    config.Init() ;
    BGIQD::LOG::timer t(config.loger , "Main ");
    config.LoadR2ONT() ;
    config.BuildR2ONTIndex() ;
    config.LoadR2CON();
    config.LoadScaffInfos();
    config.ParseAllGaps() ;
    config.PrintScaffInfos();
    return 0;
}
