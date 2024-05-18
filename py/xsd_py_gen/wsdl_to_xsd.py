import glob,os,os.path




def wsdl_xsd(fromdir, todir):
    for root, dirs, files in os.walk(fromdir):
        for file in files:
            fromfile = f'{root}\\{file}'
            _, ext = os.path.splitext(file)
            if ext == '.wsdl' and not '\\csharp\\' in root:
                with open(fromfile, 'rt') as ifile:
                    lines = ifile.readlines()
                olines = []
                olines.append('<?xml version="1.0" encoding="iso-8859-1"?>\n')
                olines.append('<xsd:schema elementFormDefault="qualified" xmlns:xsd="http://www.w3.org/2001/XMLSchema">\n')
                LOOK, DOIT = 0, 1
                state = LOOK
                for line in lines:
                    if '</wsdl:types>' in line: break
                    if '<wsdl:types>' in line:
                        state = DOIT
                        continue
                    if state == LOOK:
                        continue
                    if '<s:schema' in line:
                        continue
                    olines.append(line.replace('s:', 'xsd:'))
                with open(f'{todir}\\{file.replace(".wsdl",".xsd")}','wt') as ofile:
                    for line in olines:
                        ofile.write(line)


wsdl_xsd(r'c:\sars\source', r'C:\sarsUtil\pytools\xsd_py_gen\xsds\soapy')
