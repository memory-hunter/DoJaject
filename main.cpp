#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

void wait() {
    std::cout << "Press any key to continue... " << std::endl << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: DoJaject.exe <command>" << std::endl;
        wait();
        return -1;
    }

    if (!boost::filesystem::exists("config.json")) {
        std::cerr << "config.json not found. Creating..." << std::endl;
        boost::property_tree::ptree root;
        std::cout << "Enter DoJa directory (or drag and drop): " << std::endl;
        std::string path;
        std::getline(std::cin, path);
        path.erase(std::remove(path.begin(), path.end(), '\"'), path.end());
        if (!boost::filesystem::is_directory(path)) {
            std::cerr << "Not a directory. Exiting..." << std::endl;
            wait();
            return -1;
        } else {
            if (!boost::filesystem::exists(path + "/bin/doja.exe")) {
                std::cerr << "DoJa directory invalid. Exiting..." << std::endl;
                wait();
                return -1;
            }
        }
        root.put("doja_path", path);
        boost::property_tree::write_json("config.json", root);
    }

    std::string name = std::string(argv[1]);

    if (name.find(".jar") == std::string::npos && name.find(".jam") == std::string::npos) {
        std::cerr << "File isn't of .jam or .jar extension. Exiting..." << std::endl;
        wait();
        return -1;
    } else {
        boost::property_tree::ptree root;
        boost::property_tree::read_json("config.json", root);
        std::string doja_path = root.get<std::string>("doja_path");
        std::cout << "Creating a project with jam/jar combo..." << std::endl;
        std::string game = name.substr(name.find_last_of('/') + 1, name.length());
        game = game.substr(0, game.find_first_of('.'));
        boost::filesystem::path game_path = boost::filesystem::path(game);
        boost::filesystem::path path(doja_path + "/apps/");
        boost::filesystem::current_path(path);
        if (create_directory(game_path)) {
            std::cerr << "Could not create directory. Exiting..." << std::endl;
            wait();
            return -1;
        }
        boost::filesystem::current_path(path / game_path);
        name = name.substr(0, name.find_last_of('.'));
        copy_file(name + ".jar", boost::filesystem::current_path() / game_path.string().append(".jar"), boost::filesystem::copy_option::overwrite_if_exists);
        copy_file(name + ".jam", boost::filesystem::current_path() / game_path.string().append(".jam"), boost::filesystem::copy_option::overwrite_if_exists);
        std::cout << "Project created." << std::endl;
    }

    wait();
}
