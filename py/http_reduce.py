import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input', type=str, default=r'C:\nedbank\ownfronts\mcca-single\build\c\online\idl2\mccaserver\server\http\mccaserver_openapi.yaml')
parser.add_argument('-o', '--output', type=str, default=r'C:\nedbank\ownfronts\mcca-single\build\c\online\idl2\mccaserver\server\http\mccaserver_getbal2.yaml')
parser.add_argument('-s', '--services', type=str, default = 'Qry_BAL|Qry_BALLON')
parser.add_argument('-I', '--indent', type=int, default=2)
args = parser.parse_args()
input = args.input
output = args.output
services = args.services.split('|')
indent = ' ' * args.indent

info = []
definitions = {}
use_definition = []
schemas = {}
use_schema = []
consumes = []
produces = []
paths = {}

def indent_of(line):
    for i in (8,7,6,5,4,3,2,1):
       if line.startswith(indent * i) == True:
          return i
    return 0 

def main():
    definition_key = ''
    START, SWAGGER, SWAGGER_INFO, SWAGGER_DEFS, SWAGGER_CONSUMES, SWAGGER_PRODUCES, SWAGGER_PATHS, OPENAPI, OPENAPI_INFO, OPENAPI_SCHEMAS, OPENAPI_PATHS = range(11)
    with open(input, 'rt') as infile:
        lines = infile.readlines()
        state = START
        for line in lines:
            line = line.replace('\n','').replace('\r','')
            if state == START:
                if line[0] == '#':
                    continue
                if line.startswith('swagger:'):
                    swagger = line
                    format = state = SWAGGER
                elif line.startswith('openapi:'):
                    openapi = line
                    format = state = OPENAPI
                continue
            if state == SWAGGER:
                if line.startswith('info:'):
                    state = SWAGGER_INFO
                    info.append(line)
                continue
            if state == SWAGGER_INFO:
                if line.startswith('definitions:'):
                    state = SWAGGER_DEFS
                else:
                    info.append(line)
                continue
            if state == SWAGGER_DEFS:
                if line.startswith('consumes:'):
                    consumes.append(line)
                    state = SWAGGER_CONSUMES
                    continue
                of = indent_of(line)
                if of == 1:
                    definition_key = line[len(indent):]
                    definitions[definition_key] = []
                definitions[definition_key].append(line)
                continue
            if state == SWAGGER_CONSUMES:
                if line.startswith('produces:'):
                    produces.append(line)
                    state = SWAGGER_PRODUCES
                    continue
                consumes.append(line)
                continue
            if state == SWAGGER_PRODUCES:
                if line.startswith('paths:'):
                    state = SWAGGER_PATHS
                    use_path = False
                    continue
                produces.append(line)
                continue
            if state == OPENAPI:
                if line.startswith('info:'):
                    state = OPENAPI_INFO
                    info.append(line)
                continue
            if state == OPENAPI_INFO:
                if line.startswith('components:'):
                    state = OPENAPI_SCHEMAS
                else:
                    info.append(line)
                continue
            if state == SWAGGER_PATHS:
                of = indent_of(line)
                if of == 0:
                    break
                if of == 1:
                    use_path = False
                    for service in services:
                        if line.startswith(f'  /{service}/') or line.startswith(f'  /{service}:'):
                           use_path = True
                           if not service in paths:
                               paths[service] = []
                           print (line, service)
                           break
                if use_path == False:
                    continue
                paths[service].append(line)
                p = line.find('$ref: ')
                if p > 0:
                    pathof = line[p+6:]
                    parts = pathof.replace("'", '').split('/')
                    definition = parts[-1]
                    if not definition in use_definition:
                        use_definition.append(definition)
            if state == OPENAPI_SCHEMAS:
                if line.startswith('paths:'):
                    consumes.append(line)
                    state = OPENAPI_PATHS
                    continue
                of = indent_of(line)
                if of == 2:
                    schema_key = line[len(indent)*2:]
                    schemas[schema_key] = []
                if of >= 2:
                    schemas[schema_key].append(line)
                continue
            if state == OPENAPI_PATHS:
                of = indent_of(line)
                if of == 0:
                    break
                if of == 1:
                    use_path = False
                    for service in services:
                        if line.startswith(f'  /{service}/') or line.startswith(f'  /{service}:'):
                           use_path = True
                           if not service in paths:
                               paths[service] = []
                           print (line, service)
                           break
                if use_path == False:
                    continue
                paths[service].append(line)
                p = line.find('$ref: ')
                if p > 0:
                    pathof = line[p+6:]
                    parts = pathof.replace("'", '').split('/')
                    schema = parts[-1]
                    if not schema in use_schema:
                        use_schema.append(schema)
    with open(output, 'wt') as outfile:
        if format == SWAGGER:
            outfile.write(f'{swagger}\n')
        if format == OPENAPI:
            outfile.write(f'{openapi}\n')
        for line in info:
            outfile.write(f'{line}\n')
        if format == SWAGGER:
            line = 'definitions:'
            outfile.write(f'{line}\n')
            for defin in use_definition:
                for line in definitions[f'{defin}:']:
                    outfile.write(f'{line}\n')
        if format == OPENAPI:
            line = 'components:'
            outfile.write(f'{line}\n')
            line = f'{indent}schemas:'
            outfile.write(f'{line}\n')
            for schm in use_schema:
                for line in schemas[f'{schm}:']:
                    outfile.write(f'{line}\n')
        if format == SWAGGER:
            for line in consumes:
                outfile.write(f'{line}\n')
            for line in produces:
                outfile.write(f'{line}\n')
        line = 'paths:'
        outfile.write(f'{line}\n')
        for service in paths:
            for line in paths[service]:
                outfile.write(f'{line}\n')

if __name__ == '__main__':
    main()



