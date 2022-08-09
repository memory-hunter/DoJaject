#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

void wait() {
    std::cout << "Press any key to continue... " << std::flush;
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
        std::cout << path << std::endl;
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

    if (std::string(argv[1]).find(".jam") || std::string(argv[1]).find(".jar")) {
        std::cerr << "File isn't of .jam or .jar extension. Exiting..." << std::endl;
        wait();
        return -1;
    } else {
        boost::property_tree::ptree root;
        boost::property_tree::read_json("config.json", root);
        std::string doja_path = root.get<std::string>("doja_path");
        std::cout << "Creating a project with jam/jar combo..." << std::endl;
        boost::filesystem::path path(doja_path + "/apps");
        std::string name = std::string(argv[1]);
        name.erase(std::remove(name.begin(), name.end(), '.'), name.end());
        if (!boost::filesystem::create_directory(path / name / "bin")) {
            std::cerr << "Could not create directory. Exiting..." << std::endl;
            wait();
            return -1;
        }
        boost::filesystem::path project_path(path / name / "bin");
        copy_file(argv[1], project_path / argv[1]);
        copy_file(name + ".jam", project_path / (name + ".jam"));
        copy_file(name + ".jar", project_path / (name + ".jar"));
        std::cout << "Project created." << std::endl;
    }

    wait();
}
