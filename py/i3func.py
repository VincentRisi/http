import sys
import xmlutils
import json
import yaml
import os
sys.path.append('../../../build/putty3/sql/csnet7')
from i3server import server

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-b', '--buildpath', type=str, default='../../../build/putty3/sql')
args = parser.parse_args()

def make_server_yaml():
    outpath = args.buildpath + '/yaml'
    os.makedirs(outpath, exist_ok=True)
    with open('%s/server.yaml' % (outpath), 'wt') as ofile:
        def output(line):
            ofile.write('%s\n' % (line))
        output('%YAML 1.2')
        output('---')
        output('server:')
        tables=''
        for table in server.tables:
            tables+=' '+table.name
        output('  tables:'+tables)
        for table in server.tables:
            output('  %s:' % (table.name))
            functions=''
            for function in table.functions:
                functions+=' '+function.name
            output('    functions:'+functions)
        output('...')

def make_tables_yaml():
    outpath = args.buildpath + '/yaml'
    os.makedirs(outpath, exist_ok=True)
    for table in server.tables:
        with open('%s/%s.yaml' % (outpath, table.name), 'wt') as ofile:
            def output(line):
                ofile.write('%s\n' % (line))
            def output_field(field):
                output('      %s:' % (field.name))
                output('        usetype: %s' % (field.usetype))
                if hasattr(field, 'length'):
                    output('        length: %s' % (field.length))
                    output('        precision: %s' % (field.precision))
                    output('        scale: %s' % (field.scale))
                    output('        datatype: %s' % (field.datatype))
            output('%YAML 1.2')
            output('---')
            output('%s:' % (table.name))
            functions=''
            for function in table.functions:
                functions+=' '+function.name
            output('  functions:'+functions)
            for function in table.functions:
                output('  %s:' % (function.name))
                output('    listed: %s' % (function.listed))
                if len(function.inputs) > 0:
                    output('    input:')
                    fields=''
                    for field in function.inputs:
                        fields+=' '+field.name
                    output('      fields:'+fields)
                    for field in function.inputs:
                        output_field(field)
                if len(function.outputs) > 0:
                    output('    output:')
                    fields=''
                    for field in function.outputs:
                        fields+=' '+field.name
                    output('      fields:'+fields)
                    for field in function.outputs:
                        output_field(field)
            output('...')

def make_server_xml():
    outpath = args.buildpath + '/xml'
    os.makedirs(outpath, exist_ok=True)
    with open('%s/server.xml' % (outpath), 'wt') as ofile:
        def output(line):
            ofile.write('%s\n' % (line))
        output('<?xml version="1.0" encoding="UTF-8" ?>')
        output('<server>')
        tables=''
        for table in server.tables:
            tables+=' '+table.name
        output('  <tables>%s</tables>' % (tables[1:]))
        for table in server.tables:
            output('  <%s>' % (table.name))
            functions=''
            for function in table.functions:
                functions+=' '+function.name
            output('    <functions>%s</functions>' % (functions[1:]))
            output('  </%s>' % (table.name))
        output('</server>')

def make_tables_xml():
    buildpath = args.buildpath + '/xml'
    os.makedirs(buildpath, exist_ok=True)
    for table in server.tables:
        with open('%s/%s.xml' % (buildpath, table.name), 'wt') as ofile:
            def output(line):
                ofile.write('%s\n' % (line))
            def output_field(field):
                output('      <%s>' % (field.name))
                output('        <usetype>%s</usetype>' % (field.usetype))
                if hasattr(field, 'length'):
                    output('        <length>%s</length>' % (field.length))
                    output('        <precision>%s</precision>' % (field.precision))
                    output('        <scale>%s</scale>' % (field.scale))
                    output('        <datatype>%s</datatype>' % (field.datatype))
                output('      </%s>' % (field.name))
            output('<?xml version="1.0" encoding="UTF-8" ?>')
            output('<%s>' % (table.name))
            functions=''
            for function in table.functions:
                functions+=' '+function.name
            output('  <functions>%s</functions>' % (functions[1:]))
            for function in table.functions:
                output('  <%s>' % (function.name))
                output('    <listed>%s</listed>' % ('true' if function.listed else 'false'))
                if len(function.inputs) > 0:
                    output('    <input>')
                    fields=''
                    for field in function.inputs:
                        fields+=' '+field.name
                    output('      <fields>%s</fields>' % (fields[1:]))
                    for field in function.inputs:
                        output_field(field)
                    output('    </input>')
                if len(function.outputs) > 0:
                    output('    <output>')
                    fields=''
                    for field in function.outputs:
                        fields+=' '+field.name
                    output('      <fields>%s</fields>' % (fields[1:]))
                    for field in function.outputs:
                        output_field(field)
                    output('    </output>')
                output('  </%s>' % (function.name))
            output('</%s>' % (table.name))

def make_server_json():
    inpath = args.buildpath + '/yaml'
    buildpath = args.buildpath + '/json'
    os.makedirs(buildpath, exist_ok=True)
    with open('%s/server.yaml' % (inpath), 'rt') as ifile:
        yserver = yaml.load(ifile.read())
        with open('%s/server.json' % (buildpath), 'wt') as ofile:
            json.dump(yserver, ofile, indent=2)

def make_tables_json():
    inpath = args.buildpath + '/yaml'
    buildpath = args.buildpath + '/json'
    os.makedirs(buildpath, exist_ok=True)
    for table in server.tables:
        with open('%s/%s.yaml' % (inpath, table.name), 'rt') as ifile:
            ytable = yaml.load(ifile.read())
            with open('%s/%s.json' % (buildpath, table.name), 'wt') as ofile:
                json.dump(ytable, ofile, indent=2)

def main():
    make_server_yaml()
    make_tables_yaml()
    make_server_xml()
    make_tables_xml()
    make_server_json()
    make_tables_json()
    return

if __name__ == '__main__':
    main()
