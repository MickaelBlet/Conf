#include "Configator.hpp"

void test123(void)
{
    Conf::Configator cong({
        {"", {
            {"1",""},
            {"2",""},
        }},
        {"1", {
            {"1","1"},
            {"2","2"},
        }},
        {"2", {
            {"1","3"},
            {"2","4.122"},
        }},
    });
    return ;
}