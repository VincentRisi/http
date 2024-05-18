import sys
import os
import os.path
import glob
try:
    import xml.etree.cElementTree as etree
except:
    import xml.etree.ElementTree as etree

import argparse
arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('-i', '--infile',  type=str)
arg_parser.add_argument('-I', '--inpath',  type=str, help='in paths may be pipe delimited')
arg_parser.add_argument('-o', '--outfile', type=str)
arg_parser.add_argument('-O', '--outpath', type=str)
arg_parser.add_argument('-l', '--logpath', type=str)
arg_parser.add_argument('-v', '--verbose', action='store_true')
args = arg_parser.parse_args()

if args.infile != None:
    infile = os.path.abspath(args.infile)
    if os.path.exists(infile) == False:
        if not '*' in infile:
            sys.stderr.write(f'\ninfile:({infile}) - does not exist\n')
            sys.stderr.flush()
            arg_parser.print_help()
            exit(1)
else:
    arg_parser.print_help()
    exit(1)

if args.inpath != None:
    global inpaths
    inpaths = os.path.abspath(args.inpath).split('|')
    for inpath in inpaths:
        if os.path.exists(inpath) == False:
            sys.stderr.write(f'\ninpath:({inpath}) - does not exist\n')
            sys.stderr.flush()
            arg_parser.print_help()
            exit(1)

PYTHON_FRONT = '''\
import GENDS_UTILITY as xsd_check
from datetime import date, datetime, time
byte = short = int
'''.splitlines()

class LogEnum:
    DEBUG = 'DEBUG'
    INFO  = 'INFO'
    WARN  = 'WARN'
    ERR   = 'ERROR' 

def logln(line, how=LogEnum.INFO):
    global logfile
    logfile.write(f'{how}: {line}\n')
    logfile.flush()
    last_newline = False

def set_paths(infile):
    global outfile, logfile
    path, base = os.path.split(infile.lower())
    name, ext  = os.path.splitext(base)
    delim = '/' if path.find('/') > 0 else '\\'

    if args.logpath != None:
        logpath = os.path.abspath(args.logpath)
        delim = '/' if args.logpath.find('/') > 0 else '\\'
        os.makedirs(logpath, exist_ok=True)
        logfile_name = f'{logpath}{delim}{name}.log'
        logfile = open(logfile_name,'wt')

    if args.outpath != None:
        outfile = f'{args.outpath}{delim}{name}.py'
    elif args.outfile != None:
        outfile = os.path.abspath(args.outfile)
    
    if outfile != None:
        logln(outfile)
        outpath, base = os.path.split(outfile)
        os.makedirs(outpath, exist_ok=True)
        outfile = open(outfile, 'wt')

xs_types = {}
xs_types['xs:anyURI']             = ("''",  'URI')   
xs_types['xs:base64Binary']       = ("''",  'base64')   
xs_types['xs:boolean']            = (0,     'bool')
xs_types['xs:byte']               = (0,     'int')
xs_types['xs:date']               = ("''",  'date')
xs_types['xs:dateTime']           = ("''",  'datetime')
xs_types['xs:decimal']            = (0.0,   'float')
xs_types['xs:double']             = (0.0,   'float')   
xs_types['xs:ENTITIES']           = ("''",  'str')
xs_types['xs:ENTITY']             = ("''",  'str')
xs_types['xs:float']              = (0.0,   'float')   
xs_types['xs:hexBinary']          = ("''",  'hex')   
xs_types['xs:ID']                 = ("''",  'str')
xs_types['xs:IDREF']              = ("''",  'str')
xs_types['xs:IDREFS']             = ("''",  'str')
xs_types['xs:int']                = (0,     'int')
xs_types['xs:integer']            = (0,     'int')
xs_types['xs:language']           = ("''",  'str')
xs_types['xs:long']               = (0,     'long')
xs_types['xs:Name']               = ("''",  'str')
xs_types['xs:NCName']             = ("''",  'str')
xs_types['xs:negativeInteger']    = (0,     'int')
xs_types['xs:NMTOKEN']            = ("''",  'str')
xs_types['xs:NMTOKENS']           = ("''",  'str')
xs_types['xs:nonNegativeInteger'] = (0,     'int')
xs_types['xs:nonPositiveInteger'] = (0,     'int')
xs_types['xs:normalizedString']   = ("''",  'str')
xs_types['xs:NOTATION']           = ("''",  'note')
xs_types['xs:positiveInteger']    = (0,     'int')
xs_types['xs:QName']              = ("''",  'str')
xs_types['xs:short']              = (0,     'short')
xs_types['xs:string']             = ("''",  'str')
xs_types['xs:time']               = ("''",  'time')
xs_types['xs:timestamp']          = ("''",  'timestamp')
xs_types['xs:token']              = ("''",  'str')
xs_types['xs:unsignedByte']       = (0,     'int')
xs_types['xs:unsignedInt']        = (0,     'int')
xs_types['xs:unsignedLong']       = (0,     'long')
xs_types['xs:unsignedShort']      = (0,     'short')

xs_std_types = []
for key in xs_types:
    set_to, type = xs_types[key]
    if not type in xs_std_types:
        xs_std_types. append(type)

def indent(level):
    return ' '*(level*4)

def outcode(line, ind=0):
    global outfile, last_newline
    outfile.write(f'{indent(ind)}{line}')
    last_newline = False

def outline(line, ind=0):
    global outfile, last_newline
    outfile.write(f'{indent(ind)}{line}\n')
    outfile.flush()
    last_newline = False

def newline():
    global outfile, last_newline
    if last_newline == False:
        outfile.write('\n')
        outfile.flush()
        last_newline = True

def split_tag(entry):
    ret = entry.tag
    prefix = ''
    p1 = ret.find('{')
    if p1 >= 0:
        p2 = ret.find('}')
        namespace = ret[p1+1:p2]
        if namespace == 'http://www.w3.org/2001/XMLSchema':
            prefix = 'xs'
        ret = ret[p2+1:]
    p1 = ret.find(':')
    if p1 > 0:
        prefix = ret[0:p1]
        ret = ret[p1+1]
    return ret, prefix

def name_of(tag):
    if '-' in tag:
        tag = tag.replace('-', '_')
    if ':' in tag:
        return tag.split(':')[-1]
    return tag

class Element: 
    def __init__(self):
        self.attrib = {}
    def has(self, field):
        return hasattr(self, field)

class Simple: 
    def __init__(self):
        self.attrib = {}
        self.restriction = {}
    def has(self, field):
        return hasattr(self, field)

class Attribute:
    def __init__(self):
        self.name = None
        self.type = None
        self.optional = False
        self.use_as = 'str'
        self.set_to = "''"

class Field:
    def __init__(self):
        self.name = None
        self.has_many = False
        self.optional = False
        self.type = None
        self.base = None
        self.set_to = ''
        self.isComplex = False

class Schema:
    def __init__(self):
        self.attrib = {}
        self.elements = []
        self.complex_map = {}
        self.simples = []
        self.simples_map = {}
        self.element = None
        self.simple = None
        self.has_list = ['fractionDigits', 'length', 'minExclusive', 'minInclusive', 'minLength',
                         'maxExclusive', 'maxInclusive', 'maxLength', 'pattern', 'totalDigits']
                         #'whiteSpace']

    def get_element(self, level):
        i = len(self.elements)
        while i > 0:
            i -= 1
            lll = self.elements[i].level
            if lll == level:
                return i
        return -1

    def recurse_include(self, include):
        global inpaths
        for inpath in inpaths:
            delim = '/' if inpath.find('/') > 0 else '\\'
            include_file = f'{inpath}{delim}{include}'
            if os.path.exists(include_file):
                with open(include_file, 'rb') as ifile:
                    try:
                        tree = etree.parse(ifile)
                        root = tree.getroot()
                        self.recurse(root)
                    except Exception as error:
                       logln(f'{include_file} -- {error}', LogEnum.ERR)
                return

    def fix_up_id(self, value):
        if len(value) == 0:
            return '_'
        if value[0] in '0123456789':
            value = '_' + value
        value = value.replace(' ','_')
        value = value.replace('/','_')
        value = value.replace('-','_')
        return value

    def recurse(self, entry, level=0, depth=0):
        tag_type, prefix = split_tag(entry)
        for xyz in ['name', 'type', 'ref', 'memberTypes']:
           if xyz in entry.attrib:
            if '-' in entry.attrib[xyz]:
                logln(f'{xyz}: {entry.attrib[xyz]} *contains hyphens*', LogEnum.WARN)
                entry.attrib[xyz] = entry.attrib[xyz].replace('-', '_')
        if tag_type == 'schema':
            for key in entry.attrib:
                self.attrib[key] = entry.attrib[key]
        elif tag_type == 'import':
            if not 'imports' in self.__dict__:
                self.imports = []
            item = {}
            for key in entry.attrib:
                item[key] = entry.attrib[key]
            #logln(f'{item}')
            if 'schemaLocation' in item:
                self.imports.append(item)
                self.recurse_include(item['schemaLocation'])
        elif tag_type == 'include':
            if not 'includes' in self.__dict__:
                self.includes = []
            item = {}
            for key in entry.attrib:
                item[key] = entry.attrib[key]
            self.includes.append(item)
            self.recurse_include(item['schemaLocation'])
            logln(f'{self.includes}')
        elif tag_type == 'element':
            self.element = Element()
            self.element.depth = depth
            self.element.position = len(self.elements)
            self.elements.append(self.element)
            self.element.level = level
            self.element.prefix = prefix
            for key in entry.attrib:
                self.element.attrib[key] = entry.attrib[key]
            if not 'type' in self.element.attrib:
                self.element.attrib['<deftype>'] = 'xs:string'
        elif tag_type == 'complexType':
            def check_types():
                type = name_of(self.element.attrib['type'])
                name = name_of(entry.attrib['name'])
                result = type == name
                if result == True:
                    logln(f'type:{type} name:{name}', LogEnum.WARN)
                return result
            if depth == 1:
                if self.element != None and self.element.depth == 1 and self.element.has('attrib') and 'type' in self.element.attrib and 'name' in entry.attrib and check_types() == True:
                    self.element.complexType = {}
                    for key in entry.attrib:
                       self.element.complexType[key] = entry.attrib[key] 
                    level = 1
                else:    
                    self.element = Element()
                    self.element.position = len(self.elements)
                    self.element.depth = depth
                    self.elements.append(self.element)
                    self.element.level = level
                    self.element.prefix = prefix
                    self.element.complexType = {}
                    for key in entry.attrib:
                        self.element.attrib[key] = entry.attrib[key]
                        self.element.complexType[key] = entry.attrib[key]
                    level = 1
            else:
                self.element.complexType = {}
                for key in entry.attrib:
                    self.element.complexType[key] = entry.attrib[key] 
                level += 1
            if 'name' in self.element.attrib:
                n = len(self.elements) - 1
                name = name_of(self.element.attrib['name'])
                self.complex_map[name] = n
        elif tag_type == 'annotation':        
            if self.element != None and self.element.has('annotation') == False:
                self.element.annotation = []
        elif tag_type == 'documentation':
            if self.element != None and self.element.has('annotation') == True:
                self.element.annotation.append(entry.text)
        elif tag_type == 'sequence':
            self.element.sequence = level
        elif tag_type == 'all':
            self.element.all = level
        elif tag_type == 'any':
            self.element = Element()
            self.element.position = len(self.elements)
            self.element.depth = depth
            self.elements.append(self.element)
            self.element.level = level
            self.element.prefix = prefix
            self.element.any = level
            self.element.attrib['name'] = 'xmldata'
            self.element.attrib['type'] = 'xmldata'
            for key in entry.attrib:
                self.element.attrib[key] = entry.attrib[key]
        elif tag_type == 'choice':
            self.element.choice = level
        elif tag_type == 'attribute':
            if self.simple != None and self.simple.has('extension'):
                if self.simple.has('attributes') == False:
                    self.simple.attributes = []
                attribute = {}
                for key in entry.attrib:
                    attribute[key] = entry.attrib[key]
                self.simple.attributes.append(attribute)
            else:    
                no = self.get_element(level-1)
                if no != -1:
                    attribute_element = self.elements[no]
                    if attribute_element.has('attributes') == False:
                        attribute_element.attributes = []
                    attribute = {}
                    for key in entry.attrib:
                        attribute[key] = entry.attrib[key]
                    attribute_element.attributes.append(attribute)
        elif tag_type == 'simpleType':
            self.simple = Simple()
            self.simple.prefix = prefix
            for key in entry.attrib:
                self.simple.attrib[key] = entry.attrib[key]
            if depth == 1:
                self.simples.append(self.simple)
                if 'name' in self.simple.attrib:
                    n = len(self.simples) - 1
                    name = name_of(self.simple.attrib['name'])
                    self.simples_map[name] = n
            else:
                self.element.simple = self.simple
        elif tag_type == 'restriction':
            for key in entry.attrib:
                self.simple.restriction[key] = entry.attrib[key]
        elif tag_type == 'simpleContent':
            self.simple.simpleContent = True
            self.element.simpleContent = True
        elif tag_type == 'extension':
            self.simple.extension = entry.attrib['base']
            self.element.extension = entry.attrib['base']
        elif tag_type == 'union':
            if self.simple.has('union') == False:
                self.simple.union = {}
            for key in entry.attrib:
                self.simple.union[key] = entry.attrib[key]
        elif tag_type == 'enumeration':
            if self.simple.has('enumeration') == False:
                self.simple.enumeration = {}
            value = entry.attrib['value']
            if 'id' in entry.attrib:
                id = entry.attrib['id']
            else:
                id = self.fix_up_id(value)
            self.simple.enumeration[id] = value
        elif tag_type == 'fractionDigits':
            if 'value' in entry.attrib:
                self.simple.fractionDigits = entry.attrib['value']
        elif tag_type == 'length':
            if 'value' in entry.attrib:
                self.simple.length = entry.attrib['value']
        elif tag_type == 'minExclusive':
            if 'value' in entry.attrib:
                self.simple.minExclusive = entry.attrib['value']
        elif tag_type == 'minInclusive':
            if 'value' in entry.attrib:
                self.simple.minInclusive = entry.attrib['value']
        elif tag_type == 'minLength':
            if 'value' in entry.attrib:
                self.simple.minLength = entry.attrib['value']
        elif tag_type == 'maxExclusive':
            if 'value' in entry.attrib:
                self.simple.maxExclusive = entry.attrib['value']
        elif tag_type == 'maxInclusive':
            if 'value' in entry.attrib:
                self.simple.maxInclusive = entry.attrib['value']
        elif tag_type == 'maxLength':
            if 'value' in entry.attrib:
                self.simple.maxLength = entry.attrib['value']
        elif tag_type == 'pattern':
            self.simple.pattern = entry.attrib['value']
        elif tag_type == 'totalDigits':
            if 'value' in entry.attrib:
                self.simple.totalDigits = entry.attrib['value']
        elif tag_type == 'whiteSpace':
            if 'value' in entry.attrib:
                self.simple.whiteSpace = entry.attrib['value']
        else:
            logln(f'{indent(level)}***{repr(tag_type)} not handled', LogEnum.WARN)
        for child in entry:
            self.recurse(child, level, depth+1)
    
    def gensimple(self, simple):
        if 'base' in simple.restriction:
            base = simple.restriction["base"]
        else:
            base = 'str'
        if simple.has('enumeration'):
            name = f'{simple.attrib["name"]}Enum'
            outline(f'class {name}:', 0)
            enumeration = simple.enumeration
            for id in enumeration:
                outline(f'{id} = {repr(enumeration[id])}', 1)
            newline()
        if simple.has('whiteSpace'):
            outline(f'def {simple.attrib["name"]}WS(value):', 0)
            outline(f'base = {repr(base)}', 1)
            outline(f'rule = {repr(getattr(simple, type))}', 1)
            outline(f'return value', 1)
        outline(f'def {simple.attrib["name"]}(value):', 0)
        outline(f'base = {repr(base)}', 1)
        done = False
        if simple.has('union'):
            members = simple.union['memberTypes'].split()
            ind = 0
            for member in members:
                if done == True:
                    outline(f'if valid == False:', 1)
                    ind = 1
                outline(f'valid = {member}(value)', 1+ind)
                done = True

        def has(type, done):
            ind = 0
            if simple.has(type):
                if done == True:
                    outline(f'if valid == True:', 1)
                    ind = 1
                outline(f'valid = xsd_check.{type}(base, value, {repr(getattr(simple, type))})', 1+ind)
                done = True
            return done

        for type in self.has_list:
            done = has(type, done)
        if simple.has('enumeration'):
            comma = ''
            ind = 0
            if done == True:
                outline(f'if valid == True:', 1)
                ind = 1
            outcode(f'elist = [', 1+ind)
            for k in simple.enumeration:
                if len(k) > 0:
                    e = simple.enumeration[k]
                    outcode(f'{comma}{repr(e)}')
                    comma = ', '
            outline(']')
            outline(f'valid = value in elist', 1+ind)
            done = True
        outline(f'return valid', 1)
        newline()
    
    def gensimples(self):
        if self.simples != {}:
            for simple in self.simples:
                self.gensimple(simple)
        if self.elements != {}:
            for no, element in enumerate(self.elements):
                if element.has('simple'):
                    element.simple.attrib['name'] = f'{element.attrib["name"]}Type'
                    self.gensimple(element.simple)

    def get_field(self, element, base_name):
        field = Field()
        field.name = name_of(element.attrib['name'])
        if 'maxOccurs' in element.attrib:
            maxOccurs = element.attrib['maxOccurs']
            field.has_many = maxOccurs == 'unbounded' or maxOccurs > '1'
        if 'minOccurs' in element.attrib:
            minOccurs = element.attrib['minOccurs']
            field.optional = minOccurs == '0'

        def _get_simple(field):
            if not 'type' in element.attrib and element.has('simple') == False:
                if '<deftype>' in element.attrib:
                    element.attrib['type'] = element.attrib['<deftype>']
                    logln(f"type:{element.attrib['type']} assumed missing in action", LogEnum.WARN)
            if 'type' in element.attrib:
                field.type = name_of(element.attrib['type'])
                if field.type in self.simples_map:
                    n = self.simples_map[field.type]
                    return self.simples[n]
            elif element.has('simple'):
                return element.simple
            return None

        def _use_simple(simple, field):
            if simple.has('restriction') and 'base' in simple.restriction:
                field.base = simple.restriction['base']
                if field.base in xs_types:
                    field.set_to, field.type = xs_types[field.base]
                    return True
            elif simple.has('union') and 'memberTypes' in simple.union:
                memberTypes = simple.union['memberTypes'].split()
                first = memberTypes[0]
                if first in self.simples_map:
                    n = self.simples_map[first]
                    simple2 = self.simples[n]
                    if _use_simple(simple2, field):
                        return True
            return False
        simple = _get_simple(field)
        if simple != None:
            done = _use_simple(simple, field)
            if done:
                return field

        def _get_complex(field):
            if 'type' in element.attrib:
                field.type = name_of(element.attrib['type'])
                if field.type in self.complex_map:
                    n = self.complex_map[field.type]
                    return self.elements[n]
            return None

        complex = _get_complex(field)
        if complex != None:
            field.isComplex = True
            complex.name = name_of(complex.attrib['name'])
            if field.has_many:
                field.type = f'[{complex.name}] ## {complex.name}'
                field.set_to = '[]'
            else:
                field.type = f'{complex.name}'
                field.set_to = f'{complex.name}()'
            return field
        if element.has('complexType'):
            complexType = element.complexType
            field.isComplex = True
            if field.has_many:
                field.type = f'[{field.name}] ## {base_name}.{field.name}'
                field.set_to = '[]'
            else:
                field.type = f'{field.name}'
                if base_name == '':
                    field.set_to = f'{field.name}()'
                else:
                    field.set_to = f'{base_name}.{field.name}()'
            return field
        elif 'type' in element.attrib:
            field.type = name_of(element.attrib['type'])
            if field.type in xs_types:
                field.set_to, field.type = xs_types[field.type]
            else:
                field.set_to, field.type = "''", 'str' 
            return field
        return field
    
    def get_attribs(self, element):
        attribs = []
        if element.has('attributes'):
            for attribute in element.attributes:
                attrib = Attribute()
                attribs.append(attrib)
                attrib.name = f"{attribute['name']}"
                if 'type' in attribute:
                    attrib.type = attribute['type']
                    if attrib.type in xs_types:
                        attrib.set_to, attrib.use_as = xs_types[attrib.type]
                else:
                    attrib.set_to, attrib.use_as = "''", 'str'
                if 'use' in attribute:
                    if attribute['use'] == 'optional':
                        attrib.optional = True
        return attribs

    def get_basename(self, member_list):
        base_name = ''
        if len(member_list) > 1:
           base_name = member_list[0]
           i = 1
           while i < len(member_list) - 1:
               base_name += f'.{member_list[i]}'
               i += 1
        return base_name

    def classline(self, s, ind):
        self.classes[self.classes_key].append((s, ind))

    def add_class_depends(self, type):
        if type == None:
            return
        if '[' in type:
            x = type.replace('[','').replace(']',"").replace('#','').split()
            type = x[0]
        if not type in xs_std_types:
            if not type in self.classes_depends[self.classes_key]:
                self.classes_depends[self.classes_key].append(type)

    def genvalue_class(self, main, level):
        ind = level
        n = self.simples_map[main.extension]
        simple = self.simples[n]
        value_set_to, value_type = 0, 'float'
        if simple.has('restriction'):
            value_base = simple.restriction['base']
            if value_base in xs_types:
                value_set_to, value_type = xs_types[value_base]
        attribs = []
        if simple.has('attributes'):
            for field in simple.attributes:
                attrib = Attribute()
                attrib.name = field['name']
                attrib.type = field['type']
                if 'use' in field:
                    attrib.optional = field['use'] == 'optional'
                attrib.set_to, attrib.use_as = "''", 'str'
                if attrib.type in xs_types: 
                    attrib.set_to, attrib.use_as = xs_types[attrib.type]
                elif attrib.type in self.simples_map:
                    n = self.simples_map[attrib.type]
                    simple = self.simples[n]
                    if simple.has('restriction'):
                        attrib.type = simple.restriction['base']
                        if attrib.type in xs_types:
                            attrib.set_to, attrib.use_as = xs_types[attrib.type]
                attribs.append(attrib)
        for attrib in attribs:
            name, type = attrib.name, attrib.use_as
            self.classline(f"{name}: ({type}, 'attrib')", ind+1)
            self.add_class_depends(type)
        self.classline(f"value: ({value_type}, 'pseudo')", ind+1)
        self.add_class_depends(value_type)
        self.classline(f'def __init__(self):', ind+1)
        for attrib in attribs:
            name, set_to, optional = attrib.name, attrib.set_to, attrib.optional
            if optional:
                set_to = None
            self.classline(f"self.{name} = {set_to}", ind+2)
        self.classline(f"self.value = {value_set_to}", ind+2)
        newline()

    def genclass(self, level):
        elements = self.elements_dict[level]
        ind = level
        name, no = self.elements_name[ind]
        if name in self.complex_map:
            index = self.complex_map[name]
            main = self.elements[index]
            if main.has('simpleContent') and main.has('extension'):
                self.genvalue_class(main, level)
                return
            if main.has('attrib_fields'):
                attribs = main.attrib_fields
            else:
                attribs = []
        if len(elements) == 0 and len(attribs) == 0:
            return
        for attrib in attribs:
            self.classline(f"{attrib.name}: ({attrib.use_as}, 'attrib')", ind+1)
        fields = []
        for no, name in elements:
            element = self.elements[no]
            base_name = element.basename
            level = element.level
            field = self.get_field(element, base_name)
            fields.append(field)
        for field in fields:
            self.classline(f'{field.name}: {field.type}', ind+1)
            self.add_class_depends(field.type)
        self.classline(f'def __init__(self):', ind+1)
        for attrib in attribs:
            name, set_to, optional = attrib.name, attrib.set_to, attrib.optional
            if optional:
               set_to = 'None'
            self.classline(f'self.{name} = {set_to}', ind+2)
        for field in fields:
            name, set_to, optional, has_many = field.name, field.set_to, field.optional, field.has_many
            if optional and has_many == False:
                set_to = 'None'
            self.classline(f'self.{name} = {set_to}', ind+2)
        newline()

    def gencomplex(self, no, element):
        level = element.level
        name = name_of(element.attrib['name'])
        if level > 0:
            self.elements_dict[level-1].append([no, name]) 
        if element.has('complexType'):
            element.attrib_fields = self.get_attribs(element)
            complexType = element.complexType
            #logln(f'{level} - {self.elements_dict.keys()}')
            keys = list(self.elements_dict.keys())
            if len(keys) > 0:
                keys.reverse()
                for key in keys:
                    if key >= level:
                        self.genclass(key)
                        self.elements_dict[key] = []
            if level == 0:
                self.classes_key = name
                self.classes[self.classes_key] = []
                self.classes_depends[self.classes_key] = []
            self.classline(f'class {name}:', level)
            self.elements_dict[level] = []
            self.elements_name[level] = (name, no)

    def do_depends(self, name):
        if not name in self.classes_depends:
            return
        depends = self.classes_depends[name]
        if depends == []:
            return
        for depend in depends:
            #logln(f'{depend}')
            self.do_depends(depend)
            if depend in self.done:
                continue
            if depend in self.classes:
                for line in self.classes[depend]:
                    outline(line[0], line[1])
                self.done.append(depend)
                newline()

    def genpython(self):
        for line in PYTHON_FRONT:
           outline(line)
        newline()
        self.gensimples()
        if self.elements != {}:
            self.elements_dict = {}
            self.elements_name = {}
            #self.elements_base = {}
            self.classes = dict()
            self.classes_depends = dict()
            member_list = []
            for no, element in enumerate(self.elements):
                while element.level < len(member_list):
                    member_list.pop()
                if not 'name' in element.attrib and 'ref' in element.attrib:
                    element.attrib['name'] = element.attrib['ref']
                if element.level == len(member_list):
                    member_list.append(name_of(element.attrib['name']))
                else:
                    member_list[element.level] = name_of(element.attrib['name'])
                self.elements[no].basename = self.get_basename(member_list)
                #logln(f'{no}: {element.attrib["name"]} of {self.elements[no].basename}')
            for no, element in enumerate(self.elements):
                self.gencomplex(no, element)
            no = len(self.elements_dict) - 1
            while no >= 0:
              self.genclass(no)
              no -= 1
            self.done = []
            for name in self.classes:
                if name in self.done:
                    continue
                #logln(f'{name}')
                self.do_depends(name)
                for line in self.classes[name]:
                    outline(line[0], line[1])
                self.done.append(name)
                newline()
            for done in self.done:
                logln(f'{done}')

def process(infile):
    set_paths(infile)
    with open(infile, 'rb') as ifile:
        tree = etree.parse(ifile)
    root = tree.getroot()
    schema = Schema()
    schema.recurse(root)
    logln('------------------simple--------------------')
    for simple in schema.simples:
        logln(simple.__dict__)
    logln('------------------element-------------------')
    for element in schema.elements:
        logln(f'{indent(element.level)}{element.__dict__}')
        if element.has('simple'):
            logln(f'{indent(element.level)}=> {element.simple.__dict__}')
    schema.genpython()

def main():
    if '*' in args.infile:
        infiles = glob.glob(args.infile)
        for infile in infiles:
            process(infile)
    else:
        process(args.infile)
  
if __name__ == '__main__':
    main()