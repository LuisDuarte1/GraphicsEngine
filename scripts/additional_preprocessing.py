from __future__ import barry_as_FLUFL
import glob
import sys
from io import SEEK_SET, TextIOWrapper
import os
import re


def do_nothing():pass

PROJ_FILES = "../src"

os.chdir(PROJ_FILES)

#get all cpp files and header files
FILES = glob.glob("**", recursive=True)
FILES = [x if '.cpp' in x or '.h' in x else '' for x in FILES]

components = {}


def component_class_rule(file: TextIOWrapper):
    file.seek(0, SEEK_SET) #move to the start of the file
    lines = file.readlines()
    found_component_class = False
    component_name = ""
    var_names = []
    skip = False
    check_for_component_changes = False
    function_var_names = {}
    component_changes_line = -1
    for i,line in enumerate(lines):
        if "COMPONENT_CLASS_START" in line and not "#define" in line:
            found_component_class = True
            component_name = re.search("COMPONENT_CLASS_START\((.*)\)", line).group(1)
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
    if component_name not in components.keys():
        components[component_name] = var_names
    

def fillCommand(file: TextIOWrapper, lines, line: int) -> int:
    print(f"Filling in file {file.name}:{line}")
    fill_match = re.search("Fill\s*(\S*);\s*Format:\s*(.*)", lines[line])
    identation =  " " * (len(lines[line]) - len(lines[line].lstrip()))
    if not fill_match:
        print(f"Invalid fill command syntax in file {file.name}:{line+1}")
        exit(1)
    component_name = fill_match.group(1)
    if component_name not in components.keys():
        print(f"Invalid component name {component_name} in file {file.name}:{line+1}")
        exit(1)
    format = fill_match.group(2)
    if '{name}' not in format:
        print(f"Invalid fill format in file {file.name}:{line+1}")
        exit(1)
    
    #means that it's empty so we can just fill it
    if re.search("\s*//End Command\s*", lines[line+1]):
        file.truncate(0)
        file.writelines(lines[:line+1])
        for i in components[component_name]:
            file.write(identation+format.replace("{name}", i)+"\n")
        file.writelines(lines[line+1:])
    #if not we need to do a asymetrical check for deletions or additions
    else:
        fill_var_names = []
        end = 0
        for i,l in enumerate(lines[line+1:]):
            if re.search("\s*//End Command\s*", l):
                end = i + line+1
                break
            format_re = re.search("\s*"+format.replace("(","\(").replace(")", "\)").replace("{name}", "(\S*)")+"\s*", l)
            if not format_re:
                print(f"Something not equal to the format is in the Fill command region in file {file.name}:{line}")
                exit(1)
            fill_var_names.append((i + line+1, format_re.group(1)))
        
        deletions = [i[0] if i[1] not in components[component_name] else do_nothing() for i in fill_var_names]
        deletions = list(filter(lambda x: x is not None, deletions))
        if len(deletions) > 0:
            file.truncate(0)
            file.writelines(lines[:line+1])
            for e in range(line+1,end):
                if e in deletions:
                    continue
                file.write(lines[e])
            file.writelines(lines[end:])
            file.seek(0, SEEK_SET) #move to the start of the file
            lines = file.readlines()

        additions = [i if i not in list(map(lambda x: x[1], fill_var_names)) else do_nothing() for i in components[component_name]]
        additions = list(filter(lambda x: x is not None, additions))
        if len(additions) > 0:
            file.truncate(0)
            file.writelines(lines[:line+1])
            for i in additions:
                file.write(identation+format.replace("{name}", i)+"\n")
            file.writelines(lines[line+1:])
    return line+1+len(components[component_name])            




COMMANDS = {"Fill":fillCommand}


def comment_command_rule(file):
    file.seek(0, SEEK_SET) #move to the start of the file
    lines = file.readlines()
    skip = -1
    i = -1
    while i < len(lines)-1:
        i += 1 
        while i < skip:
            i += 1
            continue

        begin_command_math = re.search("\s*//\s*Command:\s*(\S*)\s", lines[i])
        if not begin_command_math:
            continue
        command = begin_command_math.group(1)
        command_func = COMMANDS.get(command)
        if not command_func:
            print("Invalid command name {} in file {}:{}".format(command,file.name, i+1), file=sys.stderr)
            exit(1)
        skip = command_func(file, lines, i)
        if skip > i:
            file.seek(0, SEEK_SET) #move to the start of the file
            lines = file.readlines()





if __name__ == '__main__':
    for filename in FILES:
        if filename == '': continue
        file = open(filename, "a+")
        component_class_rule(file)
        file.close()
    #now after we find every component we open all files again to search for comments that use commands to auto generate lines of code according to format
    for filename in FILES:
        if filename == '': continue
        file = open(filename, "a+")
        comment_command_rule(file)
        file.close()
