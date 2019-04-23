#ifndef __COMMON_ARGS_ARGS_PARSER_H__
#define __COMMON_ARGS_ARGS_PARSER_H__

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include <getopt.h>

namespace BGIQD{
    namespace ARGS{

        struct args_union
        {
            enum type 
            {
                is_bool = 0,
                is_string = 1,
                is_int = 2,
                is_long = 3,
                is_float = 4,
                is_vector_string = 5,
            };

            union data {
                std::string *s;
                std::vector<std::string> *vs;
                bool b;
                int i;
                long l;
                float f;
            };

            bool optional;

            bool setted ;

            std::string explain;

            std::string name ;

            std::string default_value ;

            type t;

            data d;

            static std::string get_type( type t)
            {
                if( t == is_bool )
                    return "[ no arg ]";
                if( t == is_int)
                    return "[ int arg ]";
                if( t == is_long)
                    return "[ long arg ]";
                if( t == is_float)
                    return "[ float arg ]";
                if( t == is_string)
                    return "[ string arg ]";
                if( t == is_vector_string)
                    return "[ vector_string arg ] ";
                return "";
            }

            std::string args_to_string() 
            {
                if( t == is_bool )
                    return d.b ? "true" : "false";
                if( t == is_int)
                    return std::to_string(d.i);
                if( t == is_long)
                    return std::to_string(d.l);
                if( t == is_float)
                    return std::to_string(d.f);
                if( t == is_string)
                    return to_string();
                if( t == is_vector_string)
                {
                    std::string ret ;
                    for(const auto & i : to_vector_string())
                    {
                        ret += "\t";
                        ret += i;
                    }
                    return ret ;
                }
                return "";
            }

            args_union(type ty
                    ,const  std::string & n
                    ,bool o
                    ,const std::string & df
                    ,const std::string & exp) 
                : optional(o)
                , setted(false)
                , explain(exp)
                , name(n)
                , default_value(df)
                , t(ty) 

            {
                if(! optional )
                {
                    d.s = NULL;
                }
                if ( t != is_string && t != is_vector_string )
                    d.l = 0 ;
                if ( t == is_float )
                    d.f = 0.0f ;
                if ( t == is_string )
                    d.s = new std::string("");
                if ( t == is_vector_string )
                    d.vs = new std::vector<std::string>();
            }

            void set_value( const  char * value , bool df )
            {
                if( t == BGIQD::ARGS::args_union::is_bool  )
                {
                    d.b = ! df ;
                }
                if( t == BGIQD::ARGS::args_union::is_string)
                {
                    d.s = new std::string(value);
                }
                if( t == BGIQD::ARGS::args_union::is_int)
                {
                    d.i = std::stoi(std::string(value));
                }
                if( t == BGIQD::ARGS::args_union::is_long)
                {
                    d.l = std::stol(std::string(value));
                }
                if( t == BGIQD::ARGS::args_union::is_float)
                {
                    d.f = std::stod(std::string(value));
                }
                if( t == BGIQD::ARGS::args_union::is_vector_string)
                {
                    if( d.vs == NULL )
                    {
                        d.vs = new std::vector<std::string>() ;
                    }
                    (*(d.vs)).push_back(std::string(value));
                }
            }

            std::string to_string() const 
            {
                assert( t == is_string && d.s );
                return *d.s;
            }

            std::vector<std::string> to_vector_string() const 
            {
                assert( t == is_vector_string && d.vs );
                return *d.vs;
            }

            bool to_bool() const { assert(t == is_bool); return d.b ; }

            int to_int() const { assert( t == is_int) ; return d.i ; }

            long to_long() const { assert(t == is_long) ;return d.l ; }

            float to_float() const { assert( t== is_float) ; return d.f ; }

            ~args_union()
            {
                if( t == is_string && d.s )
                    delete d.s ;
                if ( t== is_vector_string && d.vs )
                    delete d.vs ;
            }
        };


        template<class T>
            struct args_traits
            {
                args_union::type type() ;
            };

        template<>
            struct args_traits<int>
            {
                args_union::type type() { return args_union::type::is_int ; }
            };

        template<>
            struct args_traits<std::string>
            {
                args_union::type type() { return args_union::type::is_string; }
            };

        template<>
            struct args_traits<long>
            {
                args_union::type type() { return args_union::type::is_int ; }
            };


        template<>
            struct args_traits<float>
            {
                args_union::type type() { return args_union::type::is_float; }
            };

        template<>
            struct args_traits<std::vector<std::string> >
            {
                args_union::type type() { return args_union::type::is_vector_string; }
            };

        template<>
            struct args_traits< bool >
            {
                args_union::type type() { return args_union::type::is_bool; }
            };


        static std::map<int,args_union*>  infos;

        static int arg_index = 0 ;

        const int arg_max = 10 ;

        static struct option long_options[arg_max];

    }//ARGS
}//BGIQD


#define __PRINT_USAGE \
    std::cerr<<"Usage : "<<argv[0]<< " args "<<std::endl;\
    for( const auto &i : BGIQD::ARGS::infos )\
{\
    std::cerr<<"\t\t"<<"--"<<i.second->name;\
    if( i.second->optional){\
        std::cerr<<"\t"<<"[optional]";}\
    else{std::cerr<<"\t"<<"[required]";}\
    std::cerr<<"\t"<<BGIQD::ARGS::args_union::get_type(i.second->t);\
    std::cerr<<"\t"<<i.second->explain;\
    if( i.second->optional ){\
        std::cerr<<"\t [ default= "<<i.second->default_value<<" ]";\
    }\
    std::cerr<<std::endl;\
}

#define __CONSTRUCT_LONG_OPTIONS\
    int max = 0 ;\
    for( const auto &i : BGIQD::ARGS::infos )\
{\
    int id = i.first;\
    auto & item = *(i.second);\
    BGIQD::ARGS::long_options[id].name = item.name.c_str();\
    if(item.t == BGIQD::ARGS::args_union::is_bool )\
    {\
        BGIQD::ARGS::long_options[id].has_arg = 0;\
    }else{\
        BGIQD::ARGS::long_options[id].has_arg = 1;\
    }\
    BGIQD::ARGS::long_options[id].flag = 0 ;\
    BGIQD::ARGS::long_options[id].val = id ;\
    max = id ;\
}\
    for(int i = max + 1 ; i < BGIQD::ARGS::arg_max ; i++ )\
{\
    BGIQD::ARGS::long_options[i].name = 0;\
    BGIQD::ARGS::long_options[i].has_arg = 0;\
    BGIQD::ARGS::long_options[i].flag = 0;\
    BGIQD::ARGS::long_options[i].val = 0;\
}

#define __PARSE_ARGS \
    int __curr_flag , __out = 0;\
    while( ( __curr_flag = getopt_long_only( argc , argv,"",BGIQD::ARGS::long_options, &__out  ) ) != EOF )\
{\
    auto itr = BGIQD::ARGS::infos.find( __curr_flag ) ;\
    if ( itr == BGIQD::ARGS::infos.end() )\
    {    continue; }\
    itr->second->setted = true ;\
    itr->second->set_value(optarg, false);\
}\

#define __PRINT_ARGS \
    std::cerr<<argv[0];\
    for( const auto &i : BGIQD::ARGS::infos ){\
        if( i.second->setted) {\
            std::cerr<<" --"<<i.second->name<<" "<<i.second->args_to_string();\
        }\
    }\
    std::cerr<<std::endl;

#define __CHECK_ARGS \
    bool pass = true ;\
    for( const auto &i : BGIQD::ARGS::infos )\
{\
    if( ! i.second->optional &&  ! i.second->setted ){\
        std::cerr<<"ERROR:  unset nacessary args -- "<<i.second->name<<std::endl;\
        pass =false ;\
    }\
    if( i.second->optional && (! i.second->setted) ){\
        i.second->set_value(i.second->default_value.c_str(),true); \
    }\
}\
if( ! pass ){\
    __PRINT_USAGE\
    return 0;\
} else {\
    __PRINT_ARGS \
}

#define __DEFINE_ARG_DETAIL( typen , name , optional , d ,exp ) \
    BGIQD::ARGS::args_union name(BGIQD::ARGS::args_traits<typen>().type(),#name, optional,d,exp);\
    BGIQD::ARGS::infos[BGIQD::ARGS::arg_index]=&name;\
    BGIQD::ARGS::arg_index ++ \

#define START_PARSE_ARGS \
    BGIQD::ARGS::infos.clear();\
    BGIQD::ARGS::arg_index = 0 ;\


#define DEFINE_ARG_REQUIRED( typen , name , exp ) \
    __DEFINE_ARG_DETAIL( typen , name , false,"", exp)

#define DEFINE_ARG_OPTIONAL(typen , name ,  exp , df)\
    __DEFINE_ARG_DETAIL( typen , name , true , df , exp);\

#define __CHECK_HELP \
    if( argc < 2\
            || std::string(argv[1]) == "-h" \
            || std::string(argv[1]) == "--help"\
            || std::string(argv[1]) == "help"\
            ){\
        __PRINT_USAGE\
        return 0 ;\
    }

#define END_PARSE_ARGS \
    __CHECK_HELP\
    __CONSTRUCT_LONG_OPTIONS\
    __PARSE_ARGS\
    __CHECK_ARGS

#endif //__COMMON_ARGS_ARGS_PARSER_H__
