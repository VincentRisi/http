import sys
import os, os.path
import json
import yaml

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-b', '--buildpath', type=str, default='../../../build/putty3')
parser.add_argument('-i', '--idl2path',  type=str, default='idl2/ingress3/http')
parser.add_argument('-s', '--sqlpath',   type=str, default='sql/yaml')
parser.add_argument('-m', '--module',    type=str, default='ingress3')
parser.add_argument('-o', '--openapi',   action='store_true')
args = parser.parse_args()

if args.openapi == True:
    in_filename  = '%s/%s/%s_openapi.yaml' % (args.buildpath, args.idl2path, args.module)
    openapi_json = '%s/%s/openapi.json' % (args.buildpath, args.idl2path)
    openapi_yaml = '%s/%s/openapi.yaml' % (args.buildpath, args.idl2path)
else:
    in_filename  = '%s/%s/%s_swagger.yaml' % (args.buildpath, args.idl2path, args.module)
    swagger_json = '%s/%s/swagger.json' % (args.buildpath, args.idl2path)
    swagger_yaml = '%s/%s/swagger.yaml' % (args.buildpath, args.idl2path)

def expand(yaml_lines, schema_name, reference, gap, n):
    source = reference[0]
    type, filename = source.split('/')[-2:]
    SKIP_OVER, COPY_ASIS = 0, 1
    state = SKIP_OVER
    if type in ['idl2', 'http']:
        schema_filename = '%s/%s/%s' % (args.buildpath, args.idl2path, filename)
    else:
        schema_filename = '%s/%s/%s' % (args.buildpath, args.sqlpath, filename)
    with open(schema_filename.replace('\\','/'), 'rt') as in_file:
        in_lines = in_file.readlines()
        for line in in_lines:
            if line.find('...') == 0:
                continue
            if line.find(gap) == 0 and line[n] != ' ':
                if schema_name == line[n:-2]:
                    state = COPY_ASIS
                    continue
                if state == COPY_ASIS:
                    break
            if state == SKIP_OVER:
                continue
            yaml_lines.append(line)

def main():
    COPY_ASIS, IN_COMPONENTS, IN_DEFINITIONS = 0, 1, 2
    state = COPY_ASIS
    with open(in_filename, 'rt') as in_file:
        in_lines = in_file.readlines()
        yaml_lines = []
        gap = '    '
        n = 4
        for line in in_lines:
            if line.find('components:') == 0 or line.find('  schemas:') == 0:
                state = IN_COMPONENTS
                yaml_lines.append(line)
                gap = '    '
                continue
            if line.find('definitions:') == 0:
                state = IN_DEFINITIONS
                yaml_lines.append(line)
                gap = '  '
                n = 2
                continue
            if line.find('paths:') == 0:
                state = COPY_ASIS
                yaml_lines.append(line)
                continue
            if state == COPY_ASIS:
                yaml_lines.append(line)
                continue
            if line.find(gap) == 0 and line[n] != ' ':
                schema_name = line[n:-2]
                print (schema_name)
                yaml_lines.append(line)
                continue
            if line.find('$ref:') > 0:
                parts = line.strip().split()
                if len(parts) > 1: 
                    reference = parts[1].replace("'", '').split('#')
                    expand(yaml_lines, schema_name, reference, gap, n)
                continue
            yaml_lines.append(line)
    if args.openapi == True:
        outfile = open(openapi_yaml, 'wt')
    else:
        outfile = open(swagger_yaml, 'wt')
    outfile.writelines(yaml_lines)
    outfile.close()
    try:
        y = yaml.load(''.join(yaml_lines))
        j = json.dumps(y,indent=2)
        if args.openapi == True:
            outfile = open(openapi_json, 'wt')
        else:
            outfile = open(swagger_json, 'wt')
        outfile.writelines(j)
        outfile.close()
    except:
        print ('look at swagger file {0}'.format(openapi_yaml if args.openapi == True else swagger_yaml))

if __name__ == '__main__':
    main()

