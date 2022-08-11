#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

struct game {
    boost::filesystem::path name;
    boost::filesystem::path directory;
};

void wait() {
    std::cout << "Press any key to continue... " << std::endl << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    if (!boost::filesystem::exists("config.json")) {
        std::cerr << "config.json not found. Creating..." << std::endl;
        boost::property_tree::ptree root;
        std::cout << "Enter DoJa directory (or drag and drop): " << std::endl;
        std::string path;
        std::getline(std::cin, path);
        boost::filesystem::path doja_path(path);
        if (!boost::filesystem::is_directory(doja_path)) {
            std::cerr << "Not a directory. Exiting..." << std::endl;
            wait();
            return -1;
        } else {
            if (!boost::filesystem::exists(doja_path.append("/bin/doja.exe"))) {
                std::cerr << "DoJa directory invalid. Exiting..." << std::endl;
                wait();
                return -1;
            }
        }
        root.put("doja_path", path);
        boost::property_tree::write_json("config.json", root);
    }

    std::cout << "Enter game .jam or .jar from the same directory (or drag and drop): " << std::endl;
    boost::filesystem::path arg_path;
    std::string arg;
    std::getline(std::cin, arg);
    arg_path = boost::filesystem::path(arg);
    if (arg_path.extension() != ".jam" && arg_path.extension() != ".jar") {
        std::cerr << "File isn't of .jam or .jar extension. Exiting..." << std::endl;
        wait();
        return -1;
    } else {
        boost::property_tree::ptree root;
        boost::property_tree::read_json("config.json", root);
        std::string doja_path = root.get<std::string>("doja_path");
        std::cout << "Creating a project with jam/jar combo..." << std::endl;
        game game{
                arg_path.stem(),
                arg_path.parent_path()
        };
        boost::filesystem::path path(doja_path + "/apps/");
        boost::filesystem::current_path(path);
        boost::filesystem::create_directory(path / game.name);
        boost::filesystem::create_directory(path / game.name / "bin");
        boost::filesystem::current_path(path / game.name / "bin");
        copy_file(game.directory / game.name.string().append(".jam"),
                  boost::filesystem::current_path() / game.name.string().append(".jam"),
                  boost::filesystem::copy_option::overwrite_if_exists);
        copy_file(game.directory / game.name.string().append(".jar"),
                  boost::filesystem::current_path() / game.name.string().append(".jar"),
                  boost::filesystem::copy_option::overwrite_if_exists);
        std::cout << "Project created." << std::endl;
    }
    wait();
}
