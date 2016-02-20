#include <util/util.h>
#include <fstream>
#include <sstream>
#include <util/error.h>

namespace util {

std::string read_file_contents(const std::string& filename)
{
    std::ifstream file(filename);

    if (file.good()) {
        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }
    DIE("File not found: %s.\n", filename);
    return "";
}

} // namespace util
