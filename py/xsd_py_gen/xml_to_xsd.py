import sys
import os
import os.path
import glob
from lxml import etree
etree_parser = etree.XMLParser(remove_comments=True)
from io import BytesIO

schema   =  '<xs:schema attributeFormDefault="unqualified" elementFormDefault="qualified" xmlns:xs="http://www.w3.org/2001/XMLSchema">'

complex  =  '''\
<xs:element name="%s">
    <xs:complexType>
        <xs:sequence>'''

element = '<xs:element name="%s" type="xs:%s" maxOccurs="1" minOccurs="1" />'

endcomp  = '''\
        </xs:sequence>
    </xs:complexType>
</xs:element>'''

endsch   =  '</xs:schema>'

def ind(no):
    return '            ' * no + '    '

code = []
blocks = {}
multiples = []

def field_type(value):
    try:
        x = int(value)
        if len(value) == 8 and int(value[:4]) > 2000 and int(value[:4]) < 2050:
            return 'date'
        return 'int'
    except: pass
    try:
        x = float(value)
        return 'float'
    except: pass
    return 'string'

def gen_xsd(child, depth, super):
    if child.text == None: 
        text = ' '
    else:
        text = child.text.strip()
    key = f'{super}.{child.tag}.{depth}'
    if len(text) > 0:
        code.append(f'{ind(depth)}{element % (child.tag, field_type(text))}')
    else:
        if key in blocks:
           if not key in multiples:
               multiples.append(key)
           return
        blocks[key] = len(code)
        block = complex % child.tag
        for line in block.splitlines():
            code.append(f'{ind(depth)}{line}')
        for subchild in child:
            gen_xsd(subchild, depth + 1, f'{super}.{child.tag}')
        for line in endcomp.splitlines():
            code.append(f'{ind(depth)}{line}')

def to_xsd(message):
    code.append(schema)
    with BytesIO(message) as bytes:
        tree = etree.parse(bytes, etree_parser)
        root = tree.getroot()
        block = complex % root.tag
        for line in block.splitlines():
            code.append(f'  {line}')
        for child in root:
            gen_xsd(child, 1, root.tag)
    for line in endcomp.splitlines():
        code.append(f'  {line}')
    code.append(endsch)
    print (multiples)
    for key in multiples:
        occ = blocks[key]
        line = code[occ]
        line = line.replace('>', ' maxOccurs="10">')
        code[occ] = line
    for line in code:
        print(line)

def main_to_xsd():
    with open(r'testfiles\xml\GPSPayment3.xml', 'rb') as ifile:
        message = ifile.read()
    to_xsd(message)

main_to_xsd()

