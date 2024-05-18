
class Class: pass
module = Class()

def printClass(c, name='module'):
    def listed(attr):
        if type(attr) is list:
            if len(attr) > 0:
                if hasattr(attr[0], '__dict__'):
                    return True
        return False
    items = vars(c)
    for item in items:
        try:
            if item == 'parameters':
                pass
            attr = getattr(c, item)
            if hasattr(attr, '__dict__'):
                printClass(attr, '%s.%s' % (name, item))
            elif listed(attr):
                for i, x in enumerate(attr):
                    printClass(x, '%s.%s[%d]' % (name, item, i))
            else:
                print ('%s.%s=%s' % (name, item, repr(attr)))
        except Exception as ex:
            print (name, item, ex)

def dict_to_class(cls, data):
    for tag in data:
       if tag == 'parameters':
           pass
       entry = data[tag]
       if type(entry) is dict:
           newcls = Class()
           result = dict_to_class(newcls, entry)
           setattr(cls, tag, result)
       elif type(entry) is list:
           newlst = []
           for x in entry:
               if type(x) is dict:
                   newcls = Class()
                   result = dict_to_class(newcls, x)
                   newlst.append(result)
               #elif type(x) is list:
               #    pass
               else:
                   newlst.append(x)
           setattr(cls, tag, newlst)
           pass
       else:
           setattr(cls, tag, data[tag])
    return cls

def yaml_test1(infile):
    import yaml
    load = yaml.load
    Loader = yaml.Loader
    with open(infile, 'rt') as ifile:
        data = ifile.read()
    oa_dict = load(data, Loader=Loader)
    module = Class()
    module = dict_to_class(module, oa_dict)
    printClass(module, 'accuity')
    return 0

def yaml_test2(infile):
    import strictyaml as yaml
    load = yaml.load
    with open(infile, 'rt') as ifile:
        data = ifile.read()
    oa_dict = load(data)
    module = Class()
    module = dict_to_class(module, oa_dict.data)
    printClass(module, 'accuity')
    return 0

def json_test(infile):
    import json
    load = json.loads
    #Loader = yaml.Loader
    with open(infile, 'rt') as ifile:
        data = ifile.read()
    oa_dict = load(data)
    module = Class()
    module = dict_to_class(module, oa_dict)
    printClass(module, 'accuity')
    return 0

yaml_test2(r'D:\vlab\python\jtools\out\idl2\http\accuity_openapi.yaml')
#yaml_test(r'D:\vlab\python\jtools\out\idl2\http\accuity_swagger.yaml')
#json_test(r'D:\vlab\python\jtools\out\idl2\http\accuity_openapi.json')
#json_test(r'D:\vlab\python\jtools\out\idl2\http\accuity_swagger.json')