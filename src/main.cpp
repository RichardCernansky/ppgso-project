
#include "window.cpp"

const unsigned int SIZE = 1024;

int main()
{
    auto window = ProjectWindow{SIZE};
    while (window.pollEvents()){}

    return EXIT_SUCCESS;
}
