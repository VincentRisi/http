import sys
import json
import yaml
import os

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-b', '--buildpath', type=str, default='../../../build/putty3/sql')
args = parser.parse_args()

def json_test():
    inpath = args.buildpath + '/json'
    ifile = open('%s/server.json' % (inpath), 'rt')
    server = json.load(ifile)['server']
    ifile.close()
    tables = server['tables'].split()
    print (tables)
    for table in tables:
        function_names = server[table]['functions']
        if function_names != None:
            functions = function_names.split()
            print (table, functions)

def yaml_test():
    inpath = args.buildpath + '/yaml'
    ifile = open('%s/server.yaml' % (inpath), 'rt')
    server = yaml.load(ifile)['server']
    ifile.close()
    tables = server['tables'].split()
    print (tables)
    for table in tables:
        function_names = server[table]['functions']
        if function_names != None:
            functions = function_names.split()
            print (table, functions)

def main():
    json_test()
    yaml_test()

if __name__ == '__main__':
    main()

