#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <windows.h>

using namespace boost::filesystem;
using boost::property_tree::ptree;

const path GLOB_EXEC_PATH = current_path();

struct game {
    path name;
    path directory;
};

void wait() {
    std::cout << "Press any key to continue... " << std::endl << std::flush;
    std::cin.ignore(INT_MAX, '\n');
}

void check_json() {
    if (!exists("config.json")) {
        std::cerr << "config.json not found. Creating..." << std::endl;
        ptree root;
        std::cout << "Enter DoJa directory (or drag and drop): " << std::endl;
        std::string arg_path;
        std::getline(std::cin, arg_path);
        if (arg_path[0] == '\"' && arg_path[arg_path.length() - 1] == '\"') {
            arg_path = arg_path.substr(1, arg_path.length() - 2);
        }
        path doja_path(arg_path);
        if (!is_directory(doja_path)) {
            std::cerr << "Not a directory. Exiting..." << std::endl;
            wait();
            exit(-1);
        }
        else {
            if (!exists(doja_path.append("/bin/doja.exe"))) {
                std::cerr << "DoJa directory invalid. Exiting..." << std::endl;
                wait();
                exit(-1);
            }
        }
        root.put("doja_path", arg_path);
        write_json("config.json", root);
    }
}

void import_games(const std::vector<std::string>& names) {
    path doja_path;
    ptree root;
    current_path(GLOB_EXEC_PATH);
    read_json("config.json", root);
    doja_path = root.get<std::string>("doja_path");

    for (const std::string& name : names) {
        path file(name);
        
        game game{
            file.stem(),
            file.parent_path()
        };

        std::cout << "Creating for " << game.name << "...\n";

        path project_path = doja_path / "apps" / game.name;

        create_directory(project_path);
        create_directory(project_path / "bin");
        create_directory(project_path / "sp");

        if (!exists(project_path) || !exists(project_path / "bin") || !exists(project_path / "sp")) {
            std::cerr << "Directories couldn't create. Skipping...";
            continue;
        }

        current_path(project_path / "bin");

        path jam_file = game.directory / (game.name.string() + ".jam");
        path jar_file = game.directory / (game.name.string() + ".jar");

        if (!exists(jam_file) || !exists(jar_file)) {
            std::cerr << "Missing .jam or .jar file. Skipping..." << std::endl;
            continue;
        }

        copy_file(jam_file, project_path / "bin" / (game.name.string() + ".jam"),
            copy_option::overwrite_if_exists);
        copy_file(jar_file, project_path / "bin" / (game.name.string() + ".jar"),
            copy_option::overwrite_if_exists);

        current_path(project_path / "sp");

        if (exists(game.directory / (game.name.string() + ".sp"))) {
            std::cout << "OPTIONAL: .sp found. Copying...\n";
        }
    }
}

int main() {
    
    char szf[MAX_PATH * 20] = {0};
    std::vector<std::string> files;
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "JAM Files (*.jam)\0*.jam\0";
    ofn.lpstrFile = szf;
    ofn.nMaxFile = MAX_PATH * 20;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
    
    check_json();
    std::cout << "Select all .jam files to import (.jar should be in the same directory, .sp is optional).\n";
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
