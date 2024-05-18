import glob, re, os, os.path

def matches(line):
    m = re.match('(?P<f>^\s*)#[uU][sS][eE]\s+(?P<s>[a-zA-Z_0-9]*)\s*[=]?\s*(?P<a>[a-zA-Z_0-9]*)', line)
    if m != None: return 0, m
    m = re.match('^\s*(?P<f>True|False)\s*[=]', line)
    if m != None: return 1, m
    m = re.match('^\s*import\s.*(?P<f>string)', line)
    if m != None: return 2, m
    m = re.match('^\s*from\s.*(?P<f>string)\s?import', line)
    if m != None: return 3, m
    m = re.match('\s.*(?P<f>string)\.', line)
    if m != None: return 4, m
    return 99, None

def convert(list, inname):
    outname = inname.replace('source', 'build').replace('.pty', '.py')
    dir,name = os.path.split(outname)
    if name[:3] in ['I3_','DB_']:
        return
    if os.path.exists(dir) == False:
        os.makedirs(dir)
    outfile = open(outname, 'wt')
    infile = open(inname, 'rt')
    lines = infile.readlines()
    infile.close()
    for line in lines:
        line = line.rstrip()
        n, m = matches(line)
        if n == 0:
            trip = m.groups()
            if trip[1] == 'INTRINSICS':
                line = '%sfrom SYS_FUNCTIONS import *\n%sfrom INTRINSICS import *' % (trip[0], trip[0])
            elif len(trip[2]) == 0:
                line = '%sfrom %s import *' % (trip[0], trip[1])
            elif trip[1] == trip[2]:
                line = '%simport %s' % (trip[0], trip[1])
            else:
                line = '%simport %s as %s' % (trip[0], trip[1], trip[2])
            if trip[1] != 'INTRINSICS' and not trip[1] in list:
                list.append(trip[1])
        elif n == 1: 
            word = m.groups()
            line = line.replace(word[0], '#%s' % (word[0]))
        elif n in (2,3,4): 
            word = m.groups()
            line = line.replace(word[0], 'DEPRECATED_STRING', 1)
        if "SYS_IDE_FUNCTIONS" in line:
            line = line.replace("SYS_IDE_FUNCTIONS", "SYS_FUNCTIONS")
        outfile.write('%s\n' % line)
    outfile.close()
    command = 'python %s -w -n %s' % (r'C:\Python36\Tools\Scripts\2to3.py', outname)
    #print(command)
    os.system(command)


def main():
    list = []
    mask = r'd:\nedbank\temp\2to3\source\*.pty'
    files = glob.glob(mask)
    for file in files:
        #print(file)
        convert(list, file)

if __name__ == '__main__':
    main()

