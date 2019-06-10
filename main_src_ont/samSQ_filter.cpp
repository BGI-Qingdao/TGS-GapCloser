#include <iostream>
#include <set>
#include <string>

int main()
{
    std::string line ;
    std::set<std::string> sqs ;

    while( ! std::getline( std::cin , line ).eof() )
    {
        if( line.size() > 3 && line[0] == '@' && line[1] == 'S' && line[2] == 'Q' )
        {
            if( sqs.find(line) == sqs.end () )
            {
                sqs.insert(line) ;
                std::cout<<line<<'\n';
            }
            else ;
        }
        else
            std::cout<<line<<'\n';
    }
    return 0;
}
