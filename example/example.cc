#include <iostream>
#include "searchMP4.h"
#include <string>

int main(int argc, char** argv)
{
    HobotDMS::SearchMP4 search_mp4;
    std::string s1 = "/home/haoming/";
    std::string s2 = "/home/wente";

    search_mp4.showSearchPath();
    search_mp4.addSearchPath(s1);
    search_mp4.addSearchPath(s2);
    search_mp4.showSearchPath();
    std::cout << "1111111" << std::endl;

    std::cout << "erase: " << search_mp4.delSearchPath(s2) << std::endl;
    //std::cout << "erase: " << search_mp4.delSearchPath(s1) << std::endl;
    search_mp4.showSearchPath();

    search_mp4.getMP4Path(1, 2);
    std::cout << "hello world" << std::endl;
    return 0;
}
