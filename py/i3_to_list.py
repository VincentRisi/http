import glob, os.path, os
path = '../../../build/putty3/sql/csnet7/i3_*.cs'

field_types = {}
field_types['A'] = 'Status'
field_types['b'] = 'BLOB'
field_types['B'] = 'Image'
field_types['c'] = 'CLOB'
field_types['C'] = 'Char'
field_types['D'] = 'Date'
field_types['E'] = 'Money'
field_types['F'] = 'Float'
field_types['H'] = 'HugeCHAR'
field_types['I'] = 'Int'
field_types['L'] = 'Long'
field_types['M'] = 'TimeStamp'
field_types['N'] = 'NChar'
field_types['O'] = 'Boolean'
field_types['P'] = 'ImagePtr'
field_types['R'] = 'Binary'
field_types['S'] = 'Smallint'
field_types['T'] = 'Tinyint'
field_types['U'] = 'UserStamp'
field_types['V'] = 'VarNum'
field_types['X'] = 'DateTime'
field_types['x'] = 'XMLTYPE'
field_types['Z'] = 'Exception'
field_types['z'] = 'ZLOB'
field_types['$'] = 'Dynamic'

def check_function(name):
    if name[-5:] == 'Input':
        return name[:-5], name[-5:]
    else:
        return name[:-6], name[-6:]

def process(println, lines):
    for line in lines:
        n = line.find('//')
        after = ''
        if n > 0:
            after = line[n+2:].strip()
            line = line[:n].strip()
        fields = line.split()
        if len(fields) < 3:
            continue
        if fields[0] == '[XmlRoot]':
            function_name, function_type = check_function(fields[3])
            if function_type == 'Input':
                println('_func = Class()')
                println('_func.listed=False')
                println('_tab.functions.append(_func)')
                println('_func.name=%s' % (repr(function_name)))
                println('_flds = _func.inputs=[]')
            else:
                println('_flds = _func.outputs=[]')
        if fields[0] == '[XmlAttribute]':
            println('_fld = Class()')
            println('_fld.name=%s' % (repr(fields[3])))
            println('_fld.usetype=%s' % (repr(fields[2])))
            println('_flds.append(_fld)')
            field_type, field_name = fields[2], fields[3]
            parts = after.split()
            if len(parts) == 8:
                if parts[0] == 'length:':
                    println('_fld.length=%s' % (parts[1]))
                if parts[2] == 'precision:':
                    println('_fld.precision=%s' % (parts[3]))
                if parts[4] == 'scale:':
                    println('_fld.scale=%s' % (parts[5]))
                if parts[6] == 'type:':
                    println('_fld.datatype=%s' % (repr(field_types[parts[7]])))
        if fields[0] == '[XmlElement]':
            println('_func.listed=True')

def main():
    ofile = open(path.replace('i3_*.cs','i3server.py'), 'wt')
    def println(s):
        ofile.write('%s\n' % (s))
    println('class Class: pass')
    println('server = Class()')
    println('server.tables = []')
    for file in glob.glob(path):
        _, filename = os.path.split(file)
        i3_table, _ = os.path.splitext(filename)
        table_name = i3_table[3:]
        println('_tab = Class()')
        println('server.tables.append(_tab)')
        println('_tab.name=%s' % (repr(table_name)))
        println('_tab.functions = []')
        ifile = open(file, 'rt')
        ilines = ifile.readlines()
        ifile.close()
        process(println, ilines)

if __name__ == '__main__':
    main()
