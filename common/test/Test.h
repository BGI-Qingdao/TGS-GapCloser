#ifndef __LFR_TEST_TEST_H__
#define __LFR_TEST_TEST_H__

#include "common/test/Check.h"
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include "common/log/log.h"
#include "common/log/logfilter.h"
struct Test
{
    typedef std::function<void()> testFunc ;

    typedef std::vector<testFunc> TestVec;

    typedef std::map<std::string , TestVec *> TestMap;

    static TestMap & the_map()
    {
        static TestMap themap;
        BGIQD::LOG::logfilter::singleton().get("TEST_main",BGIQD::LOG::loglevel::DEBUG,Test::log);
        return themap;
    }

    static void TRun(const std::string &name ,TestVec * v)
    {
        the_map().emplace(std::make_pair(name,v));
    }

    static void RunAllTest()
    {
        for(auto & a: the_map())
        {
            RunVec(a.first,*a.second);
        }
    }
    static void RunTest(std::string module_name)
    {
        auto it = the_map().find(module_name);
        if(it != the_map().end() )
        {
            RunVec(it->first, *(it->second));
        }
    }
    private:
        static void RunVec(std::string name ,const TestVec & v)
        {
            log<<BGIQD::LOG::lstart()
                <<"---------------- Start test module "<<name<<" ---------------"
                <<BGIQD::LOG::lend();
            for(auto  i = v.begin() ; i!=v.end() ; i++)
            {
                (*i)();
            }
            log<<BGIQD::LOG::lstart()
                <<"---------------- End   test module "<<name<<" ---------------"
                <<BGIQD::LOG::lend();
        }
        static BGIQD::LOG::logger  log;
};


#define TEST_MODULE_INIT(name) \
    static BGIQD::LOG::logger test_logger;\
    static Test::TestVec & get_module()\
    {\
        static Test::TestVec * thevec = nullptr;\
        if(thevec == nullptr)\
        {\
            BGIQD::LOG::logfilter::singleton().get("TEST_"#name,BGIQD::LOG::loglevel::DEBUG,test_logger);\
            thevec = new Test::TestVec();\
            Test::TRun(#name,thevec);\
        }\
        return *thevec;\
    }

#define TEST(name) \
    void name(); \
    namespace{\
        struct test_##name{\
            test_##name(){\
                get_module().push_back([](){\
                    BGIQD::LOG::timer t(test_logger,#name);\
                    name();\
                    });\
            }\
        };\
        static test_##name tmp_##name;\
    }\
    void name()


#endif //__LFR_TEST_TEST_H_
