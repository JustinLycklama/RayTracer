#include "scene_lua.hpp"

int main(int argc, char** argv)
{
    std::string filename = "scene.lua";
    if (argc >= 2) {
        filename = argv[1];
    }

    std::cerr << "Running with file: " << filename << std::endl;

    if (!run_lua(filename)) {
        std::cerr << "Could not open " << filename << std::endl;
        return 1;
    }
}

