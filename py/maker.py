with open(r'C:\vlab\python\jtools\src\configs\JPortal.run.xml', 'rt') as ifile:
    jlines = ifile.readlines()
for line in jlines:
    fields = line.strip().split()
    if fields[0] == '<option' and fields[1][5:] == '"PROGRAM_PARAMETERS"':
        switches = line[45:-5]
print (switches)
with open(r'C:\vlab\python\jtools\src\configs\Crackle.run.xml', 'rt') as ifile:
    clines = ifile.readlines()
for line in clines:
    fields = line.strip().split()
    if fields[0] == '<option' and fields[1][5:] == '"PROGRAM_PARAMETERS"':
        switches = line[41:-5]
print (switches)

