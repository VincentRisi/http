'''GENDS_UTILITY
    We supply the following main routines here

    - load_message(cls, message, logfile=sys.stdout)
        payment = Payment()
        load_message(payment, message)

    - as_xml(cls)
        message = as_xml(payment)

    - validate(func, value, default=None)
        valid = GENDS_UTILITY.validate
        OrigSystem = valid(OrigSystemType, 'ABCDEF')

    - amp_fix(value) 
        will escape lt Gt Quote Apostrophe and Ampersand characters
'''
from datetime import date, datetime, time
try:
    import xml.etree.cElementTree as etree
except:
    import xml.etree.ElementTree as etree
from xml.dom import minidom
from io import BytesIO, StringIO
import re, sys

byte = short = int

def validate(func, value, default=None):
    try:
        rc = func(value)
        if rc == True: 
            return value
        else:
            return default
    except Exception as ex:
        print (f'{ex.message}')
        return default

STRING_RESTRICTIONS = ['enumeration', 'length', 'maxLength', 'minLength', 'pattern', 'whiteSpace']
DATE_RESTICTIONS = ['enumeration', 'maxExclusive', 'maxInclusive', 'minExclusive', 'minInclusive', 'pattern', 'whiteSpace']
NUMERIC_RESTRICTIONS = ['enumeration', 'fractionDigits', 'maxExclusive', 'maxInclusive', 'minExclusive', 'minInclusive', 'pattern', 'totalDigits', 'whiteSpace']
MISC_RESTRICTIONS = ['enumeration', 'length', 'maxLength', 'minLength', 'pattern', 'whiteSpace']
BOOL_RESTRICTION = ['pattern', 'whiteSpace']

STRING_TYPES = ['ENTITIES', 'ENTITY', 'ID', 'IDREF', 'IDREFS', 'language', 'Name', 'NCName', 'NMTOKEN', 'NMTOKENS', 'normalizedString', 'QName', 'string', 'token']
DATE_TYPES = ['date', 'dateTime', 'duration', 'gDay', 'gMonth', 'gMonthDay', 'gYear', 'gYearMonth', 'time']
NUMERIC_TYPES = ['byte', 'decimal', 'int', 'integer', 'long', 'negativeInteger', 'nonNegativeInteger', 'nonPositiveInteger', 'positiveInteger', 'short', 'unsignedLong', 
                 'unsignedInt', 'unsignedShort', 'unsignedByte']
MISC_TYPES = ['anyURI', 'base64Binary', 'double', 'float', 'hexBinary', 'NOTATION', 'QName']
BOOL_TYPES = ['boolean']

def _type_of(value):
    parts = value.split(':')
    if len(parts) > 1:
        return parts[1]
    return value
        
def fractionDigits(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if not b in NUMERIC_TYPES:
        print (f'{base}, {value}, {rule} -> not a numeric type')
        return False 
    try:
        n = value.find('.')  # 1.23 n:1 4-2=2 rule = '2'
        if n >= 0:
            length = len(value) - (n+1)
            if  length> int(rule):
                print (f'length:{length} > no_digits:{rule}')
                return False
    except Exception as e:
        print (f'{base}, {value}, {rule} -> {e.message}')
        return False
    return True
    
def totalDigits(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if not b in NUMERIC_TYPES:
        print (f'{base}, {value}, {rule} -> not a numeric type')
        return False
    try:
        count = 0
        for ch in value:
            if ch >= '0' and ch <= '9':
                count += 1
        if count > int(rule):
            return False            
    except Exception as e:
        print (f'{base}, {value}, {rule} -> {e.message}')
        return False
    return True
    
def length(base, value, rule):
    '''
    validate function STRING_RESTRICTIONS
    '''
    b = _type_of(base)
    if b not in STRING_TYPES and b not in MISC_TYPES:
        print (f'{base}, {value}, {rule} -> not a string(misc) type')
        return False
    if len(value) != int(rule):
        print (f'length:{len(value)} not equal to {rule}')
        return False
    return True

def maxLength(base, value, rule):
    '''
    validate function MISC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b not in STRING_TYPES and b not in MISC_TYPES:
        print (f'{base}, {value}, {rule} -> not a string(misc) type')
        return False
    if len(value) > int(rule):
        print (f'length:{len(value)} greater than {rule}')
        return False
    return True

def minLength(base, value, rule):
    '''
    validate function MISC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b not in STRING_TYPES and b not in MISC_TYPES:
        print (f'{base}, {value}, {rule} -> not a string(misc) type')
        return False
    if len(value) < int(rule):
        print (f'length:{len(value)} less than {rule}')
        return False
    return True
    
def pattern(base, value, rule):
    '''
    validate function all RESTRICTIONS
    '''
    result = re.fullmatch(rule, value) 
    if result == None:
        print (f'{base}, {value}, {rule} -> fails pattern match')
        return False
    return True
    
def maxExclusive(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b in DATE_TYPES:
        # skipping date min max for now
        pass 
    elif b in NUMERIC_TYPES:
        if float(value) >= float(rule):
            print (f'{base}, {value}, {rule} -> fails {value} greater or equal than {rule}')
            return False
    else:    
        print (f'{base}, {value}, {rule} -> not a date or numeric type')
        return False
    return True
    
def maxInclusive(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b in DATE_TYPES: 
        # skipping date min max for now
        pass 
    elif b in NUMERIC_TYPES:
        if float(value) > float(rule):
            print (f'{base}, {value}, {rule} -> fails {value} greater than {rule}')
            return False
    else:    
        print (f'{base}, {value}, {rule} -> not a date or numeric type')
        return False
    return True
    
def minExclusive(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b in DATE_TYPES: 
        # skipping date min max for now
        pass 
    elif b in NUMERIC_TYPES:
        if float(value) <= float(rule):
            print (f'{base}, {value}, {rule} -> fails {value} less or equal than {rule}')
            return False
    else:    
        print (f'{base}, {value}, {rule} -> not a date or numeric type')
        return False
    return True
    
def minInclusive(base, value, rule):
    '''
    validate function NUMERIC_RESTRICTIONS
    '''
    b = _type_of(base)
    if b in DATE_TYPES: 
        # skipping date min max for now
        pass 
    elif b in NUMERIC_TYPES:
        if float(value) < float(rule):
            print (f'{base}, {value}, {rule} -> fails {value} less than {rule}')
            return False
    else:    
        print (f'{base}, {value}, {rule} -> not a date or numeric type')
        return False
    return True
    
def whiteSpace(base, value, rule):
    # not quite sure what to do with this at present
    return True

class InvalidXSDBasedClassException(Exception):
    pass

def to_datetime(data):
    n = data[12:].find('-')
    dt = data.replace(':','').replace('-','')
    if len(dt) == 15:
        format='%Y%m%dT%H%M%S'
    elif len(dt) > 15:
        format='%Y%m%dT%H%M%S'
        if '.' in dt:
            format += '.%f'
        if '+' in dt or n > 0:
            format += '%z'
    try:
        result = datetime.strptime(dt, format)
    except:
        result = None
    return result

def to_timestamp(data):
    '''
    uses to_datetime
    '''
    return to_datetime(data)

def to_date(data):
    '''
    returns %Y%m%d
    '''
    dt = data.replace(':','').replace('-','')
    format='%Y%m%d'
    try:
        result = datetime.strptime(dt, format)
    except:
        result = None
    return result

def to_time(data):
    '''
    returns %H%M%S
    '''
    dt = data.replace(':','').replace('-','')
    format='%H%M%S'
    try:
        result = datetime.strptime(dt, format)
    except:
        result = None
    return result

def from_datetime(data):
    '''
    if data is None or of type str return asis
    returns in ANSI format adds fpart and timezone if needed
    '''
    if data is None or type(data) is str:
        value = data
    elif type(data) is datetime:
        format='%Y-%m-%dT%H:%M:%S'
        value = data.strftime(format)
        fpart = data.strftime('%f')
        if fpart != '000000':
            value += f'.{fpart}'
        zpart = data.strftime('%z')
        value += zpart
    return value

def from_timestamp(data):
    return from_datetime(data)

def from_date(data):
    if data is None or type(data) is str:
        value = data
    elif type(data) is datetime:
        format='%Y-%m-%d'
        value = data.strftime(format)
    return value

def from_time(data):
    if data is None or type(data) is str:
        value = data
    elif type(data) is datetime:
        format='%H:%M:%S'
        value = data.strftime(format)
        fpart = data.strftime('%f')
        if fpart != '000000':
            value += f'.{fpart}'
    return value

def _get_tag_text(element):
    tag = element.tag
    if tag[0] == '{':
        p = tag.find('}')
        if p > 0:
            tag = tag[p+1:]
    text = element.text
    return tag, text

def _load_element(cls, element, depth, previous, logfile):
    global has_error
    class_name = cls.__class__.__name__
    annotations = getattr(cls, '__annotations__')
    entry_tag, entry_text = _get_tag_text(element)
    if not entry_tag in annotations:
        if 'xmldata' in annotations:
            xmldata = etree.tostring(element, encoding='unicode')
            p = xmldata.find(' xmlns=')
            if p > 0:
                front = xmldata[0:p]
                delim = xmldata[p+7:p+8]
                rest = xmldata[p+8:]
                n = rest.find(delim) 
                back = rest[n+1:]
                xmldata = front + back
            setattr(cls, 'xmldata', xmldata);
        else:
            error = f'Class:{class_name} missing Field:{entry_tag} not in annotations'
            logfile.write(f'{error}\n')
            has_error = True
        return
    annote = annotations[entry_tag]
    if type(annote) is tuple and len(annote) == 2:
        annote_type, annote_make = annote[0], annote[1]
    else:
        annote_type, annote_make = annote, 'asis' 
    field = getattr(cls, entry_tag)
    if type(field) is list or type(annote_type) is list:
        field_type = annote_type[0]
    else:
        field_type = annote_type
    type_name = field_type.__name__
    if type_name == 'date':
        if type(field) is list:
            field.append(to_date(entry_text))
        else:
            setattr(cls, entry_tag, to_date(entry_text))
    elif type_name == 'datetime':
        if type(field) is list:
            field.append(to_datetime(entry_text))
        else:
            setattr(cls, entry_tag, to_datetime(entry_text))
    elif type_name == 'time':
        if type(field) is list:
            field.append(to_time(entry_text))
        else:
            setattr(cls, entry_tag, to_time(entry_text))
    elif type_name == 'timestamp':
        if type(field) is list:
            field.append(to_timestamp(entry_text))
        else:
            setattr(cls, entry_tag, to_timestamp(entry_text))
    elif type_name in ['base64', 'bool', 'float', 'hex', 'int', 'long', 'note', 'short', 'str', 'URI']:
        if type(field) is list:
            field.append(entry_text)
        else:
            setattr(cls, entry_tag, entry_text)
    else:
        if type(field) is list:
            sub_cls = field_type()
            field.append(sub_cls)
        else:
            sub_cls = getattr(cls, entry_tag)
            if sub_cls == None:
                sub_cls = field_type()
                setattr(cls, entry_tag, sub_cls)
        previous = f'{previous}.{entry_tag}'
        _load_attributes(sub_cls, element, logfile)
        for sub_element in element:
            _load_element(sub_cls, sub_element, depth+1, previous, logfile)

def _load_attributes(cls, element, logfile):
    global has_error
    if element.attrib == {}: return
    class_name = cls.__class__.__name__
    annotations = getattr(cls, '__annotations__')
    for attrib in element.attrib:
        text = element.attrib[attrib]
        if not attrib in annotations:
            error = f'class:{class_name} missing attrib:{attrib} not in annotations'
            logfile.write(f'{error}\n')
            has_error = True
            continue
        if 'value' in annotations:
            vnote = annotations['value']
            if type(vnote) is tuple and len(vnote) == 2:
                vnote_type, vnote_make = vnote[0], vnote[1]
                if vnote_make == 'pseudo':
                    setattr(cls, 'value', element.text)
        annote = annotations[attrib]
        if type(annote) is tuple and len(annote) == 2:
            annote_type, annote_make = annote[0], annote[1]
        else:
            annote_type, annote_make = annote, 'asis' 
        field = getattr(cls, attrib)
        type_name = annote_type.__name__
        if type_name == 'date':
            setattr(cls, attrib, to_date(text))
        elif type_name == 'datetime':
            setattr(cls, attrib, to_datetime(text))
        else:
            setattr(cls, attrib, text)

def load_message(cls, message, logfile=sys.stdout):
    global has_error
    has_error = False
    class_name = cls.__class__.__name__
    if hasattr(cls, '__annotations__') == False:
        error = f'Class {class_name} does not have annotations'
        logfile.write(f'{error}\n')
        raise InvalidXSDBasedClassException(f'{error}')
    with BytesIO(message) as bytes:
        tree = etree.parse(bytes)
        root_element = tree.getroot()
        root_tag, root_text = _get_tag_text(root_element)
        if root_tag != class_name:
            error = f'Class {class_name} is not {root_tag}'
            logfile.write(f'{error}\n')
            raise InvalidXSDBasedClassException(f'{error}')
        previous = class_name
        _load_attributes(cls, root_element, logfile)
        for element in root_element:
            _load_element(cls, element, 1, previous, logfile)
    return has_error

def indent(ind):
    return '    ' * ind

def amp_fix(value, asis=False, undo=False):
    ''' 
      asis=True - leaves previous escapes 
      undo=True - undo any prious escapes
    '''
    if type(value) is not str:
        return value
    escapes = [('&', '&amp;'), ('<','&lt;'), ('>', '&gt;'), ('"','&quot;'), ("'",'&apos;')]
    if asis == False or undo == True:
        for pair in escapes:
            value = value.replace(pair[1], pair[0])
    if undo == True:
        return value
    for pair in escapes:
        value = value.replace(pair[0], pair[1])
    return value

def _write_xml(cls, message, field_name, ind):
    annotations = cls.__annotations__
    attribs = {}
    tag_done = False
    for key in annotations:
        annote = annotations[key]
        if type(annote) is tuple and len(annote) == 2:
            annote_type, annote_make = annote[0], annote[1]
        else:
            annote_type, annote_make = annote, 'asis' 
        if annote_make == 'attrib':
            name = key
            value = getattr(cls, key)
            attribs[name]=value
            continue
        if annote_make == 'name':
            pass
        if tag_done == False:
            tag_done = True
            attrib_str = ''
            if attribs != {}:
                for name in attribs:
                    attrib_str = f'{attrib_str} {name}="{amp_fix(value)}"'
            message.write(f'{indent(ind)}<{field_name}{attrib_str}>\n')
        field = getattr(cls, key)
        if type(field) is list:
            field_type = annotations[key][0]
        else:
            field_type = annotations[key]
        if type(field_type) is tuple and field_type[1] == 'pseudo':
            message.write(f'{indent(ind+1)}{field}\n')
            continue
        type_name = field_type.__name__
        if key == 'xmldata':
            message.write(f'{indent(ind+1)}{field.strip()}\n')
        elif type_name == 'date':
            if type(field) is list:
                for occ in field:
                    value = from_date(occ)
            else:
                value = from_date(field)
            message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
        elif type_name == 'datetime':
            if type(field) is list:
                for occ in field:
                    value = from_datetime(occ)
            else:
                value = from_datetime(field)
            message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
        elif type_name == 'time':
            if type(field) is list:
                for occ in field:
                    value = from_time(occ)
            else:
                value = from_time(field)
            message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
        elif type_name == 'timestamp':
            if type(field) is list:
                for occ in field:
                    value = from_timestamp(occ)
            else:
                value = from_timestamp(field)
            message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
        elif type_name in ['base64', 'bool', 'float', 'hex', 'int', 'long', 'note', 'short', 'str', 'URI']:
            if type(field) is list:
                for occ in field:
                    value = amp_fix(occ)
                    message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
            elif field is None:
                pass
            else:
                value = amp_fix(field)
                message.write(f'{indent(ind+1)}<{key}>{value}</{key}>\n')
        else:
            if type(field) is list:
                for subcls in field:
                    _write_xml(subcls, message, key, ind+1)
            else:
                subcls = getattr(cls, key)
                if subcls != None:
                    _write_xml(subcls, message, key, ind+1)
    message.write(f'{indent(ind)}</{field_name}>\n')

def as_xml(cls, encoding='utf-8'):
    field_name = cls.__class__.__name__
    if hasattr(cls, '__annotations__') == False:
        raise InvalidXSDBasedClassException(f'Class {field_name} does not have annotations')
    with StringIO() as message:
        if encoding != None and len(encoding) > 0:
            message.write(f'<?xml version="1.0" encoding="{encoding}"?>\n')
        _write_xml(cls, message, field_name, ind=0)
        result = message.getvalue()
        return result


## String Data Types

## Name	Description
## ENTITIES
## ENTITY
## ID                   A string that represents the ID attribute in XML (only used with schema attributes)
## IDREF                A string that represents the IDREF attribute in XML (only used with schema attributes)
## IDREFS
## language             A string that contains a valid language id
## Name                 A string that contains a valid XML name
## NCName
## NMTOKEN	            A string that represents the NMTOKEN attribute in XML (only used with schema attributes)
## NMTOKENS
## normalizedString     A string that does not contain line feeds, carriage returns, or tabs
## QName
## string               A string
## token                A string that does not contain line feeds, carriage returns, tabs, leading or trailing spaces, or multiple spaces

## Restrictions that can be used with String data types:

## enumeration
## length
## maxLength
## minLength
## pattern (NMTOKENS, IDREFS, and ENTITIES cannot use this constraint)
## whiteSpace

## ----------------------------

## Date and Time Data Types

## Name	Description

## date         Defines a date value
## dateTime     Defines a date and time value
## duration     Defines a time interval
## gDay         Defines a part of a date - the day (DD)
## gMonth       Defines a part of a date - the month (MM)
## gMonthDay    Defines a part of a date - the month and day (MM-DD)
## gYear        Defines a part of a date - the year (YYYY)
## gYearMonth   Defines a part of a date - the year and month (YYYY-MM)
## time	        Defines a time value

## Restrictions that can be used with Date data types:

## enumeration
## maxExclusive
## maxInclusive
## minExclusive
## minInclusive
## pattern
## whiteSpace

## --------------------------------

## Numeric Data Types
## 
## byte                 A signed 8-bit integer
## decimal              A decimal value
## int                  A signed 32-bit integer
## integer              An integer value
## long                 A signed 64-bit integer
## negativeInteger      An integer containing only negative values (..,-2,-1)
## nonNegativeInteger   An integer containing only non-negative values (0,1,2,..)
## nonPositiveInteger   An integer containing only non-positive values (..,-2,-1,0)
## positiveInteger      An integer containing only positive values (1,2,..)
## short                A signed 16-bit integer
## unsignedLong         An unsigned 64-bit integer
## unsignedInt          An unsigned 32-bit integer
## unsignedShort        An unsigned 16-bit integer
## unsignedByte         An unsigned 8-bit integer

## Restrictions that can be used with Numeric data types:

## enumeration
## fractionDigits
## maxExclusive
## maxInclusive
## minExclusive
## minInclusive
## pattern
## totalDigits
## whiteSpace

## -----------------------------------

## Miscellaneous Data Types
## 
## anyURI	 
## base64Binary	 
## boolean	 
## double	 
## float	 
## hexBinary	 
## NOTATION	 
## QName	 

## Restrictions that can be used with the other data types:

## enumeration  (a Boolean data type cannot use this constraint)
## length       (a Boolean data type cannot use this constraint)
## maxLength    (a Boolean data type cannot use this constraint)
## minLength    (a Boolean data type cannot use this constraint)
## pattern
## whiteSpace

## -------------------------------------

## XSD Elements
## 
## appinfo               Specifies information to be used by the application (must go inside annotation)
## attribute             Defines an attribute
## attributeGroup        Defines an attribute group to be used in complex type definitions
## choice                Allows only one of the elements contained in the <choice> declaration to be present within the containing element
## complexContent        Defines extensions or restrictions on a complex type that contains mixed content or elements only
## complexType           Defines a complex type element
## documentation         Defines text comments in a schema (must go inside annotation)
## element               Defines an element
## extension             Extends an existing simpleType or complexType element
## field                 Specifies an XPath expression that specifies the value used to define an identity constraint
## group                 Defines a group of elements to be used in complex type definitions
## import                Adds multiple schemas with different target namespace to a document
## include               Adds multiple schemas with the same target namespace to a document
## key                   Specifies an attribute or element value as a key (unique, non-nullable, and always present) within the containing element in an instance document
## keyref                Specifies that an attribute or element value correspond to those of the specified key or unique element
## list                  Defines a simple type element as a list of values
## notation              Describes the format of non-XML data within an XML document
## redefine              Redefines simple and complex types, groups, and attribute groups from an external schema
## restriction           Defines restrictions on a simpleType, simpleContent, or a complexContent
## schema                Defines the root element of a schema
## selector              Specifies an XPath expression that selects a set of elements for an identity constraint
## sequence              Specifies that the child elements must appear in a sequence. Each child element can occur from 0 to any number of times
## simpleContent         Contains extensions or restrictions on a text-only complex type or on a simple type as content and contains no elements
## simpleType            Defines a simple type and specifies the constraints and information about the values of attributes or text-only elements
## union                 Defines a simple type as a collection (union) of values from specified simple data types
## unique                Defines that an element or an attribute value must be unique within the scope

## XSD Restrictions/Facets for Datatypes

## Constraint	Description

## enumeration           Defines a list of acceptable values
## fractionDigits        Specifies the maximum number of decimal places allowed. Must be equal to or greater than zero
## length                Specifies the exact number of characters or list items allowed. Must be equal to or greater than zero
## maxExclusive          Specifies the upper bounds for numeric values (the value must be less than this value)
## maxInclusive          Specifies the upper bounds for numeric values (the value must be less than or equal to this value)
## maxLength             Specifies the maximum number of characters or list items allowed. Must be equal to or greater than zero
## minExclusive          Specifies the lower bounds for numeric values (the value must be greater than this value)
## minInclusive          Specifies the lower bounds for numeric values (the value must be greater than or equal to this value)
## minLength             Specifies the minimum number of characters or list items allowed. Must be equal to or greater than zero
## pattern               Defines the exact sequence of characters that are acceptable
## totalDigits           Specifies the maximum number of digits allowed. Must be greater than zero
## whiteSpace            Specifies how white space (line feeds, tabs, spaces, and carriage returns) is handled
