#include  "common/test/Test.h"
                          
BGIQD::LOG::logger Test::log ;

static void print_usage()
{
    std::cout<<"test    [command]"<<std::endl;
    std::cout<<"            all -- run all test module."<<std::endl<<std::endl;
    std::cout<<"            help -- print this usage"<<std::endl<<std::endl;
    std::cout<<"            command can be :"<<std::endl;

    for(auto & a : Test::the_map())
    {
        std::cout<<"              "<<a.first<<std::endl;
    }
}

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        print_usage();
        return 0;
    }
    std::string command(argv[1]);
    if(command == "--help" || command == "-h" || command == "help")
        print_usage();
    else if ( command == "all" || command == "--all" )
    {
        Test::RunAllTest();
    }
    else
    {
        Test::RunTest(command);
    }
    return 0;
}
