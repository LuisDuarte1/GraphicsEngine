import glob
import sys
from io import SEEK_SET, TextIOWrapper
import os

PROJ_FILES = "../src"

os.chdir(PROJ_FILES)

#get all cpp files and header files
FILES = glob.glob("**", recursive=True)
FILES = [x if '.cpp' in x or '.h' in x else '' for x in FILES]

def component_class_rule(file: TextIOWrapper):
    file.seek(0, SEEK_SET) #move to the start of the file
    lines = file.readlines()
    found_component_class = False
    var_names = []
    skip = False
    for i,line in enumerate(lines):
        if "COMPONENT_CLASS_START" in line and not "#define" in line:
            found_component_class = True
        if "COMPONENT_FIELD" in line and not "#define" in line:
            if found_component_class == False:
                #this means that a component field was outside a class which must not happen
                print("Found a component outside a component class in file {}: {}".format(file.name,line),file=sys.stderr)
                exit(1)
            var_names.append(line.split('(')[1].split(',')[1].split(')')[0])
        if "void addNewComponent()" in line and found_component_class==True:
            skip = True
        if "COMPONENT_CLASS_END" in line and not "#define" in line:
            if skip:
                var_names = []
                found_component_class = False
                skip = False      
                continue
            if found_component_class == False:
                print("Found a extra COMPONENT_CLASS_END in file: {}".format(file.name), file=sys.stderr)
                exit(1)
            file.truncate(0)
            file.writelines(lines[:i])
            file.write("    void addNewComponent(){\n")
            for name in var_names:
                file.write("        "+name+".resize("+name+".size()+1);\n")
            file.write("    }\n")
            file.writelines(lines[i:])
            component_class_rule(file)
            var_names = []
            found_component_class = False
    if(found_component_class == True):
        print("Didn't close component class declaration in file: {}".format(file.name), file=sys.stderr)
        exit(1)
        




if __name__ == '__main__':
    for filename in FILES:
        if filename == '': continue
        file = open(filename, "a+")
        component_class_rule(file)
        file.close()