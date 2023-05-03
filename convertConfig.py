import toml
import xmltodict

FILEPATH = r".\NewVegasReloaded.dll.config"

def convert_xml():
    toml_file = {}

    with open(FILEPATH) as fd:
        xml_string = f"<Config>{fd.read()}</Config>"

        xml_file = xmltodict.parse(xml_string)["Config"]

        for title, main_section in xml_file.items():
            toml_file[title] = {}
            print(toml.dumps(toml_file))

            for section, category_contents in main_section.items():

                # go through the Shaders and save options
                toml_file[title][section] = {}

                for subsection, settings in category_contents.items():
                    toml_file[title][section][subsection] = {}
                    
                    if settings is None:
                        print("no values for " + subsection)
                        continue

                    for key, value in settings.items():
                        print (value)
                        value_type = value["@Type"]
                        value = value["@Value"]

                        if value_type == '0':
                            value = ( value == '1')
                        elif value_type == '1':
                            value = int(value)
                        elif value_type == '2':
                            value = float(value)
                        print (value)

                        toml_file[title][section][subsection][key] = value

        print(toml.dumps(toml_file))

    with open(FILEPATH.replace("config", "converted.toml"), "w") as fd:
        fd.write(toml.dumps(toml_file))    

if __name__ == "__main__":
    convert_xml()