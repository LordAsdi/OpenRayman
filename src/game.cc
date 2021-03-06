#include <game.h>
#include <platform/file.h>
#include <platform/message_box.h>
#include <json.hpp>
#include <iostream>
#include <fstream>

namespace openrayman
{
    game::game(const backend_specifics& backend_specifics, const std::string& name) :
        m_valid(false)
    {
        m_location = backend_specifics.storage_path() + "/games/" + name;
        if(!file::exists(m_location))
            m_location = backend_specifics.data_path() + "/games/" + name;
        if(!file::exists(m_location))
        {
            // Show a special message if we can't find "rayman2".
            if(name == "rayman2")
                message_box::display("Error!", "Rayman 2: The Great Escape could not be found in the user or data directory."
                    "\nRefer to the the file \"README.md\" or use the command line option \"--help\" for further instructions.", true);
            else
                message_box::display("Error!", "The game \"" + name + "\" could not be found in the user or data directory."
                    "\nThis could be the result of a broken dependency.", true);
            return;
        }
        if(file::exists(m_location + "/manifest.json"))
        {
            nlohmann::json manifest_json;
            std::ifstream manifest_stream(file::fix_string(m_location + "/manifest.json"));
            if(manifest_stream.is_open())
            {
                manifest_stream >> manifest_json;
                if(manifest_json.count("info") <= 0
                    || manifest_json.count("dependencies") <= 0)
                {
                    message_box::display("Error!", "The manifest of the game \"" + name + "\" lacks one or more required properties.", true);
                    return;
                }
                if(manifest_json["info"].count("name") > 0)
                    m_info.name = manifest_json["info"]["name"];
                else
                    m_info.name = name;
                if(manifest_json["info"].count("description") > 0)
                    m_info.description = manifest_json["info"]["description"];
                std::vector<std::string> dependencies = manifest_json["dependencies"];
                for(std::string& dependency : dependencies)
                {
                    game* g = new game(backend_specifics, dependency);
                    if(!g->valid())
					{
						delete g;
						return;
					}
                    m_dependencies.push_back(std::unique_ptr<game>(g));
                }
                m_valid = true;
            }
        }
        else
        {
            // Show a special message if we can't find "rayman2".
            if(name == "rayman2")
                message_box::display("Error!", "Rayman 2: The Great Escape has no valid manifest."
                    "\nThis is needed for OpenRayman to run successfully."
                    "\nAn error might have occured during extraction."
                    "\nTry extracting the game again, and make sure you have a valid Rayman 2: The Great Escape installation.", true);
            else
                message_box::display("Error!", "The game \"" + name + "\" has no valid manifest.", true);
        }
    }

    bool game::has_file(const std::string& file, bool only_in_this) const
    {
        if(file::exists(m_location + "/" + file))
            return true;
        if(!only_in_this)
        {
            for(const std::unique_ptr<game>& game : m_dependencies)
            {
                if(game->has_file(file, false))
                    return true;
            }
        }
        return false;
    }

    const std::string game::find_file(const std::string& file, bool show_error_msg) const
    {
        if(has_file(file, true))
            return m_location + "/" + file;
        for(const std::unique_ptr<game>& game : m_dependencies)
        {
            if(game->has_file(file, false))
                return game->find_file(file, show_error_msg);
        }
        if(show_error_msg)
            message_box::display("Error!", "The file \"" + file + "\" could not be found in the game \"" + m_info.name + "\"."
                "\nor any of its dependencies."
                "\nThis could be the result of a broken dependency.", true);
        return "";
    }
}
