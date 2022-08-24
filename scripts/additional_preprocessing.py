from __future__ import barry_as_FLUFL
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
    check_for_component_changes = False
    function_var_names = {}
    component_changes_line = -1
    for i,line in enumerate(lines):
        if "COMPONENT_CLASS_START" in line and not "#define" in line:
            found_component_class = True
        if "COMPONENT_FIELD" in line and not "#define" in line:
            if found_component_class == False:
                #this means that a component field was outside a class which must not happen
                print("Found a component outside a component class in file {}: {}".format(file.name,line),file=sys.stderr)
                exit(1)
            var_names.append(line.split('(')[1].split(',')[1].split(')')[0].replace(" ", ""))
        if "void addNewComponent()" in line and found_component_class==True:
            skip = True
            check_for_component_changes = True
            function_var_names[i] = [] #reset function_var_names if there are multiple component classes just in case
            component_changes_line = i
            continue
        if check_for_component_changes:
            if line == "    }\n": 
                check_for_component_changes=False
                continue
            #FIXME: this is not very dynamic if there are multiple functions, for now it's not needed
            function_var_names[component_changes_line].append(line.split(".resize")[0].replace(" ",""))


        if "COMPONENT_CLASS_END" in line and not "#define" in line:
            if skip:
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
            found_component_class = False
    if(found_component_class == True):
        print("Didn't close component class declaration in file: {}".format(file.name), file=sys.stderr)
        exit(1)
    if(component_changes_line == -1): return
    #check for differences
    #FIXME: this is not very dynamic if there are multiple functions, for now it's not needed
    for line in list(function_var_names.keys()):
        #if there are differences they must be or to add or to remove from the function
        #this can be calculated by simple set math, because substraction is asymetrical in sets
        differences_to_add = list(set(var_names) - set(function_var_names[line]))
        if(len(differences_to_add) != 0):
            file.truncate(0)
            file.writelines(lines[:line+1])
            for dif in differences_to_add:
                file.write("        "+dif+".resize("+dif+".size()+1);\n")
            file.writelines(lines[line+1:])
            lines = file.readlines() #reload lines

        #to remove
        differences_to_remove = list(set(function_var_names[line]) - set(var_names))
        if(len(differences_to_remove) != 0):
            file.truncate(0)
            file.writelines(lines[:line+1])
            line_count = line+1
            for l in lines[line+1:]:
                if l == "    }\n": break
                name = lines[line_count].split(".resize")[0].replace(" ","")
                if name in differences_to_remove:
                    line_count += 1
                    continue
                file.write(lines[line_count])
                line_count += 1

            file.writelines(lines[line_count:])





if __name__ == '__main__':
    for filename in FILES:
        if filename == '': continue
        file = open(filename, "a+")
        component_class_rule(file)
        file.close()