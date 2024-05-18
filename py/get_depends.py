import sys, glob, os, os.path, re

path = os.path.abspath(r'..\router3')

files = []
file_uses = {}
file_group = {}
groups = []
group_files = {}
scripts = []
script_runner = {}
script_groups = {}
script_lines = []

def update_script_lines(script, depends):
    for i, line in enumerate(script_lines):
        if line[13:22] == script:
            b = line.find("['intrsys'")
            if b <= 0: break
            e = line.find("']", b)
            newline = '{0}{1}{2}'.format(line[:b], repr(depends), line[e+2:])
            script_lines[i] = newline
            break

def extract_usage(lines, file, group):
    if file in files:
        print ('{0}{1} - file is seen in a previous group {2}'.format(file, group, file_group[file]))
        return
    file_group[file] = group
    files.append(file)
    file_uses[file] = []
    if not group in group_files:
        groups.append(group)
        group_files[group] = []
    group_files[group].append(file)
    for line in lines:
        result = re.match('\s*(from|import)\s+(?P<data>[A-Z0-9_]+)',line)
        if result != None:
            data = result.group('data')
            if not data[:3] in ('DB_', 'I3_', 'XSD_'):
                file_uses[file].append(data)

def extract_scripts(lines):
    #script_lines = lines
    for line in lines:
        script_lines.append(line)
        if not line[:13] == "script_cntl['":
            continue
        script = line[13:22]
        scripts.append(script)
        rest = line[49:]
        result = re.match("(?P<data>[^']+)", rest)
        if result != None:
            runner = result.group('data')
            script_runner[script] = runner

def get_depends(depends, seen, file):
    seen.append(file)
    if not file in file_group:
        return
    group = file_group[file]
    if not group in depends:
        depends.append(group)
    for uses_file in file_uses[file]:
        if not uses_file in seen:
            if uses_file in files:
                get_depends(depends, seen, uses_file)
            else:
                print('{0} uses file {1} is missing'.format(file, uses_file))

def expand_script_groups():
    for script in scripts:
        file = script_runner[script]
        depends = ['intrsys']
        seen = []
        get_depends(depends, seen, file)
        script_groups[script] = depends
        update_script_lines(script, depends)

def main():
    print (os.curdir, os.getcwd(), path)
    filelist = glob.glob(r'%s\*\*.py' % (path))
    for fullname in filelist:
        pathname, filename = os.path.split(fullname)
        basename, _ = os.path.splitext(filename)
        _, groupname = os.path.split(pathname)
        #print (fullname, pathname, filename, groupname)
        ifile = open(fullname, 'rt')
        lines = ifile.readlines()
        if basename == 'SCRIPT_QUEUES':
            script_filename = fullname
            extract_scripts(lines)
        else:
            extract_usage(lines, basename, groupname)
        ifile.close()
    expand_script_groups()
    os.rename(script_filename, script_filename.replace('.py','.py.bak'))
    ofile = open(script_filename, 'wt')
    ofile.writelines(script_lines)
    ofile.close()
    return

if __name__ == '__main__':
    main()
