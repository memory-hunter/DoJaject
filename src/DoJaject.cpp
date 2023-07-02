#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <windows.h>

const boost::filesystem::path GLOB_EXEC_PATH = boost::filesystem::current_path();

struct game {
    boost::filesystem::path name;
    boost::filesystem::path directory;
};

void wait() {
    std::cout << "Press any key to continue... " << std::endl << std::flush;
    std::cin.ignore(INT_MAX, '\n');
}

void check_json() {
    if (!boost::filesystem::exists("config.json")) {
        std::cerr << "config.json not found. Creating..." << std::endl;
        boost::property_tree::ptree root;
        std::cout << "Enter DoJa directory (or drag and drop): " << std::endl;
        std::string path;
        std::getline(std::cin, path);
        if (path[0] == '\"' && path[path.length() - 1] == '\"') {
            path = path.substr(1, path.length() - 2);
        }
        boost::filesystem::path doja_path(path);
        if (!boost::filesystem::is_directory(doja_path)) {
            std::cerr << "Not a directory. Exiting..." << std::endl;
            wait();
            exit(-1);
        }
        else {
            if (!boost::filesystem::exists(doja_path.append("/bin/doja.exe"))) {
                std::cerr << "DoJa directory invalid. Exiting..." << std::endl;
                wait();
                exit(-1);
            }
        }
        root.put("doja_path", path);
        boost::property_tree::write_json("config.json", root);
    }
}

void import_games(const std::vector<std::string>& names) {
    boost::filesystem::path doja_path;
    boost::property_tree::ptree root;
    boost::filesystem::current_path(GLOB_EXEC_PATH);
    std::cout << boost::filesystem::current_path().string();
    boost::property_tree::read_json("config.json", root);
    doja_path = root.get<std::string>("doja_path");

    for (const std::string& name : names) {
        boost::filesystem::path file(name);
        
        game game{
            file.stem(),
            file.parent_path()
        };

        std::cout << "Creating for " << game.name << "...\n";

        boost::filesystem::path project_path = doja_path / "apps" / game.name;

        boost::filesystem::create_directory(project_path);
        boost::filesystem::create_directory(project_path / "bin");
        boost::filesystem::create_directory(project_path / "sp");

        boost::filesystem::current_path(project_path / "bin");

        boost::filesystem::path jam_file = game.directory / (game.name.string() + ".jam");
        boost::filesystem::path jar_file = game.directory / (game.name.string() + ".jar");

        boost::filesystem::copy_file(jam_file, project_path / "bin" / (game.name.string() + ".jam"),
            boost::filesystem::copy_option::overwrite_if_exists);
        boost::filesystem::copy_file(jar_file, project_path / "bin" / (game.name.string() + ".jar"),
            boost::filesystem::copy_option::overwrite_if_exists);

        boost::filesystem::current_path(project_path / "sp");

        if (boost::filesystem::exists(game.directory / (game.name.string() + ".sp"))) {
            std::cout << "OPTIONAL: .sp found. Copying...\n";
        }
    }
}

int main() {
    
    char szf[MAX_PATH] = {0};
    std::vector<std::string> files;
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "JAM Files (*.jam)\0*.jam\0";
    ofn.lpstrFile = szf;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
    
    check_json();
    if (GetOpenFileNameA(&ofn)) {
        char* p = szf;
        std::string dir = p;
        p += dir.length() + 1;
        
        while (*p) {
            std::string fp = dir + "\\" + p;
            if (*p == '\0') {
                files.push_back(fp);
                p += strlen(p + 1) + 1;
            }
            else {
                files.push_back(fp);
                p += strlen(p) + 1;
            }
        }
        delete[] p;
    }
    import_games(files);
    wait();
}
