import sys, json

if sys.version[:3] == '3.6':
    import xmldict3 as xmldict
elif sys.version[:3] == '2.7':
    import xmldict2 as xmldict

def xmlparse(s):
    '''
	This is the original parser using xmlrecord.cpp
	returns a python dictionary representing the data
	in tag.stag.sstag.../attr notation. xpath like.
	Uses %[0-9]+ notation for duplicate tags
    '''
    return xmldict._xmlparse(s)

def xmlbuild(s):
    '''
	This is the original builder using xmlrecord.cpp
	returns a string representing the xml. Drops %[0-9]+
	for duplicate tags.
    '''
    return xmldict._xmlbuild(s)

def printClass(c, name='top'):
    if hasattr(c, '__slots__'):
        items = c.__slots__
    else:
        items = vars(c)
    for item in items:
        try:
            attr = getattr(c, item)
            if hasattr(attr, '__dict__') or hasattr(attr, '__slots__'):
                printClass(attr, '%s.%s' % (name, item))
            else:
                print ('%s.%s=%s' % (name, item, repr(attr)))
        except Exception as ex:
            print (name, item, ex)

def _build_yaml_attrib(attribs_dict, key_name, pad):
    attrib = []
    attrib.append('%sAttributes:' % (pad))
    for key, value in attribs_dict[key_name]:
        attrib.append("%s%s: '%s'" % (pad+'  ', key, value))
    return attrib

def _build_yaml(classes_dict, attribs_dict, key_name, done, pad=''):
    entry = []
    if key_name in attribs_dict:
        attribs = _build_yaml_attrib(attribs_dict, key_name, pad)
        entry.extend(attribs)
    for field_name, field_value in classes_dict[key_name]:
        sub_key_name = key_name + '.' + field_name
        if sub_key_name in classes_dict: 
            if not sub_key_name in done:
                sub_entry = _build_yaml(classes_dict, attribs_dict, sub_key_name, done, pad+'    ')
                entry.append('%s%s:' % (pad, field_name.replace('%', '_')))
                entry.extend(sub_entry)
                done.append(sub_key_name)
        else:
            entry.append('%s%s: %s' % (pad, field_name.replace('%', '_'), field_value))
    return entry

def _build_json_attrib(attribs_dict, group_name):
    attrib = {}
    for field, value in attribs_dict[group_name]:
        attrib[field]=value
    return attrib

def _build_json(classes_dict, attribs_dict, group_name, done):
    group = {}
    if group_name in attribs_dict:
        attribs = _build_json_attrib(attribs_dict, group_name)
        group['Attributes'] = attribs
    for field_name, field_value in classes_dict[group_name]:
        sub_group_name = group_name + '.' + field_name
        if sub_group_name in classes_dict: 
            if not sub_group_name in done:
                sub_group = _build_json(classes_dict, attribs_dict, sub_group_name, done)
                group[field_name.replace('%', '_')] = sub_group
                done.append(sub_group_name)
        else:
            group[field_name.replace('%', '_')] = field_value
    return group

def _class_slots(classes_dict, attribs_dict, module_name):
    slots = []
    for field,_ in classes_dict[module_name]:
        dfield = field.replace('%','_')
        if not dfield in slots:
            slots.append(dfield)
    return slots

def _attrib_slots(attribs_dict, module_name):
    slots = []
    for name, value in attribs_dict[module_name]:
        if not name in slots:
            slots.append(name)
    return slots

class Class: pass

def _build_rec_attrib(attribs_dict, module_name):
    attrib = Class()
    attrib.__slots__ = _attrib_slots(attribs_dict, module_name)
    for field, value in attribs_dict[module_name]:
        setattr(attrib, field, value)
    return attrib

def _build_rec(classes_dict, attribs_dict, module_name, done):
    module = Class()
    module.__slots__ = _class_slots(classes_dict, attribs_dict, module_name)
    if module_name in attribs_dict:
        attrib = _build_rec_attrib(attribs_dict, module_name)
        module.__slots__.append('Attributes')
        setattr(module, 'Attributes', attrib)
    for field_name, field_value in classes_dict[module_name]:
        sub_module_name = module_name + '.' + field_name
        if sub_module_name in classes_dict: 
            if not sub_module_name in done:
                sub_module = _build_rec(classes_dict, attribs_dict, sub_module_name, done)
                setattr(module, field_name.replace('%','_'), sub_module)
                done.append(sub_module_name)
        else:
            setattr(module, field_name.replace('%','_'), field_value)
    return module

def _clear_sub_marker(field):
    n = field.find('%')
    if n > 0:
        return field[:n]
    return field

def _build_xml(classes_dict, attribs_dict, tag_name, done, pad=''):
    lines = []
    attribs = ''
    if tag_name in attribs_dict:
        for field, value in attribs_dict[tag_name]:
            attribs += ' %s=%s' % (field, repr(value))
    tags = tag_name.split('.')
    start_tag = '%s<%s%s>' % (pad, _clear_sub_marker(tags[-1]), attribs)
    end_tag = '%s</%s>' % (pad, _clear_sub_marker(tags[-1])) 
    lines.append(start_tag)
    for field_name, field_value in classes_dict[tag_name]:
        sub_tag_name = tag_name + '.' + field_name
        if sub_tag_name in classes_dict:
            if not sub_tag_name in done:
                sub_lines = _build_xml(classes_dict, attribs_dict, sub_tag_name, done, pad + '    ')
                lines.extend(sub_lines)
                done.append(sub_tag_name)
        else:
            lines.append('{0}<{1}>{2}</{1}>'.format(pad + '    ', field_name, field_value))
    lines.append(end_tag)
    return lines

def _check_attribs(key, name, value, attribs_dict):
    if len(name) > 0:
        if not key in attribs_dict:
            attribs_dict[key] = []
        attribs_dict[key].append((name, value))

def load_in_dicts(input):
    classes_dict = {}
    start_module = ''
    done = []
    attribs_dict = {}
    for input_key in input:
        input_value = input[input_key]
        attrib_name = ''
        n = input_key.find('/')
        if n != -1:
            attrib_name = input_key[n + 1:]
            input_key = input_key[:n]
        parts = input_key.split('.')
        if len(parts) == 1:
            _check_attribs(input_key, attrib_name, input_value, attribs_dict)
            continue
        if parts[0] != start_module:
            start_module = parts[0]
        class_key = ''
        attrib_key = ''
        for i in range(len(parts) - 1):
            class_key += parts[i]
            class_field = parts[i + 1]
            attrib_key = class_key + '.' + class_field
            if class_key not in classes_dict:
                classes_dict[class_key] = []
            if class_field not in classes_dict[class_key]:
                classes_dict[class_key].append((class_field, input_value))
            class_key += '.'
        _check_attribs(attrib_key, attrib_name, input_value, attribs_dict)
    return start_module, classes_dict, attribs_dict

def dict_to_rec(input_dict):
    '''
    Returns a dynamic class rec instance of the dictionary.
	d['Aaa.Bbb']=12 translates to Aaa.Bbb with a value of 12
	(getattr, setattr and hasattr -- could be useful)
    '''
    done = []
    start_name, classes_dict, attribs_dict = load_in_dicts(input_dict)
    return _build_rec(classes_dict, attribs_dict, start_name, done)

def dict_to_xml(input_dict):
    '''
	Returns a string representing the xml. Drops %[0-9]+
	for duplicate tags. Does not use xmlrecord - pure python.
    '''
    done = []
    start_name, classes_dict, attribs_dict = load_in_dicts(input_dict)
    return _build_xml(classes_dict, attribs_dict, start_name, done)

def dict_to_json(input_dict):
    '''
	Returns a string in json format translates %[0-9] to _[0-9]
	for duplicate tags.
    '''
    done = []
    start_name, classes_dict, attribs_dict = load_in_dicts(input_dict)
    return repr(_build_json(classes_dict, attribs_dict, start_name, done)).replace("'", '"')

def dict_to_yaml(input_dict):
    '''
	Returns a string in json format translates %[0-9] to _[0-9]
	for duplicate tags.
    '''
    done = []
    start_name, classes_dict, attribs_dict = load_in_dicts(input_dict)
    lines = _build_yaml(classes_dict, attribs_dict, start_name, done)
    result = '%YAML 1.2\n---\n'
    for line in lines:
        result += line + '\n'
    return result
