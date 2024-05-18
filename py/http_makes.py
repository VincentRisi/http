import yaml, json, requests
from io import BytesIO, StringIO

headers = {'Accept': 'application/json', 'Content-Type': 'application/json'}

class HttpError(BaseException):
    def __init__(self, message, error, url):
        self.message = message
        self.error = error
        self.url = url
    def __str__(self):
        return repr(self.message)

def make_url(host, port):
    return f'http://{host}:{port}'

def make_path(obj, mask):
    annotations = obj.__annotations__
    for key in annotations:
        x = getattr(obj, key)
        mask = mask.replace('{%s}' % (key), str(x))
    return mask
    
def make_data(cls, yaml_only = False):
    def to_yaml(cls, data, ind, is_list=False):
        indent = '  ' * ind
        plus = ''
        if is_list == True: 
            plus = '- '
        annotations = getattr(cls, '__annotations__')
        for key in annotations:
            annote = annotations[key]
            value = getattr(cls, key)
            if annote is str:
                data.write(f'{indent}{plus}{key}: "{value}"\n')
            elif annote is int:
                data.write(f'{indent}{plus}{key}: {value}\n')
            elif annote is float:
                data.write(f'{indent}{plus}{key}: {value}\n')
            elif type(value) is list:
                data.write(f'{indent}{plus}{key}:\n')
                for v in value:
                    to_yaml(v, data, ind+1, True)
            else:
                data.write(f'{indent}{plus}{key}:\n')
                to_yaml(value, data, ind+1)
            if is_list == True: plus = '  '
    with StringIO() as dataIO:
        to_yaml(cls, dataIO, 0)
        result = dataIO.getvalue()
    if yaml_only == True:    
        print (result)
        return result    
    else:    
        dict = yaml.load(result, Loader=yaml.FullLoader)
        data = json.dumps(dict)
        #print (data)
        return data

def set_error(cls):
    global tError
    tError = cls

def load_response(cls, response):
    def to_cls(cls, data):
        annotations = getattr(cls, '__annotations__')
        for key in annotations:
            if not key in data:
                print (f'{key} missing in data')
                continue
            annote = annotations[key]
            value = data[key]
            if annote is str or annote is int or annote is float:
                setattr(cls, key, value)
            elif type(annote) is list:
                occurs = []
                if value is not None:
                    for i, occur in enumerate(annote):
                        inst = occur()
                        rec = value[i]
                        to_cls(inst, rec)
                        occurs.append(inst)
                setattr(cls, key, occurs)
            else:
                inst = annote()
                to_cls(inst, data[key])
                setattr(cls, key, inst)
    status_code = response.status_code
    if status_code == 400:
        err = tError()
        err.error = response.text
        raise HttpError(err, response.text, response.url)
    data = json.loads(response.text)
    to_cls(cls, data)

def display(cls):
    make_data(cls, True)

