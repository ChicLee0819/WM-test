#!/usr/bin/python
import configparser, pathlib
# add dummy section for ini format
config_string = '[dummy_section]\n'
with open(pathlib.Path().resolve() / 'settings.pro', 'r') as f:
    count = 0
    start_skip = False
    lines = f.readlines()
    for line in lines:
        # skip not ini style line
        if '{' in line:
            start_skip = True
            continue
        elif start_skip:
            if '}' in line:
                start_skip = False
            continue
        count += 1
        config_string += line
        # only get 5 lines, because version is at line number 4
        if count > 5:
            break

# parse ini config
config = configparser.ConfigParser()
config.read_string(config_string)
print(config.get('dummy_section', 'VERSION'))
