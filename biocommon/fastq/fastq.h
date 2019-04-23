#ifndef __BIOCOMMON_FASTQ_FASTQ_H__
#define __BIOCOMMON_FASTQ_FASTQ_H__

#include <string>
#include <cassert>
#include <vector>
#include <iostream>

#include "biocommon/seq/seq.h"
#include "common/flags/flags.h"

namespace BGIQD {
    namespace FASTQ {

        struct NormalHead
        {
            std::string head;

            void Init( const std::string & line )
            {
                head = line ;
            }
            std::string Head() const { return head ; }

            void Reset() { head.clear() ;} 
        };

        struct Id_Desc_Head
        {
            std::string Id;

            std::string Desc;

            void Init( const std::string & line ) ;

            std::string Head() const { return "@" + Id +Desc ; }

            void Reset() { Id.clear() ; Desc.clear() ;} 
        };
        struct stLFRHeader
        {
            enum ReadType
            {
                Unknow = 0 ,
                readName_barcodeStr =  1 ,
                readName_barcodeStr_index =  2 ,
                readName_barcodeStr_index_barcodeNum = 3 ,
            } type ;
            stLFRHeader() {
                Reset();
            }

            stLFRHeader(const stLFRHeader & a) {
                type = a.type ;
                readIndex  = a.readIndex ;
                barcode_num = a.barcode_num ;
                barcode_str = a.barcode_str ;
                readName = a.readName ;
            }

            stLFRHeader & operator = ( const stLFRHeader & a ) {
                if( &a != this )
                {
                    type = a.type ;
                    readIndex  = a.readIndex ;
                    barcode_num = a.barcode_num ;
                    barcode_str = a.barcode_str ;
                    readName = a.readName ;
                }
                return *this ;
            }

            int readIndex ; //  1/2/3

            int barcode_num;

            std::string barcode_str ;

            std::string readName;

            void Init( const std::string & line ) ;

            std::string Head() const ;

            void Reset()
            {
                type = Unknow ;
                readName ="";
                barcode_str = "" ;
                readIndex = 0 ;
                barcode_num = 0 ;
            }
        };

        template<class T>
            struct Fastq
            {
                typedef T Header;

                FLAGS_INT ;

                ADD_A_FLAG(1,UnSet);
                ADD_A_FLAG(2,Set_head);
                ADD_A_FLAG(3,Set_seq);
                ADD_A_FLAG(4,Set_3);
                ADD_A_FLAG(5,Set_quality);

                void Reset() 
                {
                    head.Reset();
                    seq.Reset();
                    quality.Reset();
                    flags = 0 ;
                    Set_UnSet();
                }

                Header head;

                BGIQD::SEQ::seq seq;

                BGIQD::SEQ::seq quality;

                void AddHead(const std::string & line)
                {
                    Clean_UnSet();
                    Set_Set_head();
                    head.Init(line);
                }

                void AddSeq(const std::string & line )
                {
                    if( Is_UnSet() || ! Is_Set_head() )
                    {
                        assert(0);
                    }
                    Set_Set_seq();
                    seq.AddPartSeq(line);
                }

                void Add3(const std::string & )
                {
                    if( Is_UnSet()
                            || ! Is_Set_head() 
                            || ! Is_Set_seq() )
                    {
                        assert(0);
                    }
                    Set_Set_3();
                }

                void AddQuality( const std::string & line )
                {

                    if( Is_UnSet() || ! Is_Set_3() )
                    {
                        assert(0);
                    }
                    Set_Set_quality();
                    quality.AddPartSeq(line);
                }

                bool Is_Setted() const { 
                    return  (!Is_UnSet() )
                        && Is_Set_head() 
                        && Is_Set_seq()
                        && Is_Set_3() 
                        && Is_Set_quality();
                }

                bool QualityFilled() const 
                {
                    assert(quality.Len() <= seq.Len() );
                    return quality.Len() == seq.Len() ;
                }

            };

        template<class T > 
            struct FastqReader
            {
                typedef T Fastq;

                static bool IsHead(const std::string & line)
                {
                    return ( ! line.empty()) && line[0] == '@' ;
                }
                static bool Is_3(const std::string & line)
                {
                    return ( ! line.empty()) && line[0] == '+' ;
                }

                static void LoadAllFastq( 
                        std::istream & ist 
                        , std::vector<Fastq> & buffer )
                {
                    std::string line ;
                    Fastq fq;
                    fq.Reset();
                    long index = 0 ;
                    while ( ! std::getline(ist,line).eof() )
                    {
                        index ++ ;
                        if( index % 4 == 1 )
                        {
                            assert(IsHead(line) && "fastq format invalid!!!");
                            if(fq.Is_Setted())
                            {
                                buffer.push_back(fq);
                            }
                            fq.Reset();
                            fq.AddHead(line);
                        }
                        else if ( index % 4 == 2 )
                        {
                            fq.AddSeq(line);
                        }
                        else if ( index % 4 == 3 )
                        {
                            assert( Is_3(line) && "fastq format invalid!!!" );
                            fq.Add3(line);
                        }
                        else 
                        {
                            fq.AddQuality(line);
                        }
                    }
                    if(fq.Is_Setted())
                    {
                        buffer.push_back(fq);
                    }
                    fq.Reset();
                }

                static bool LoadNextFastq(std::istream & ist , Fastq & fq)
                {
                    std::string line ;
                    int index = 0;
                    fq.Reset();
                    while ( ! std::getline(ist,line).eof() )
                    {
                        index ++ ;
                        if( index % 4 == 1 )
                        {
                            assert(IsHead(line) && "fastq format invalid!!!");
                            fq.AddHead(line);
                        }
                        else if ( index % 4 == 2 )
                        {
                            fq.AddSeq(line);
                        }
                        else if ( index % 4 == 3 )
                        {
                            assert( Is_3(line) && "fastq format invalid!!!" );
                            fq.Add3(line);
                        }
                        else 
                        {
                            fq.AddQuality(line);
                        }
                        if( index == 4 )
                            break ;
                    }
                    return fq.Is_Setted();
                }
            };
    }
}

#endif //__BIOCOMMON_FASTQ_FASTQ_H__
