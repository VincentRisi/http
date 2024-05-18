methods = '''\
_b64Decode
_b64Encode
_cache
_cacheClear
_commit
_logon
_parameters
_prompt
_getMessage
_reply
_rollBack
_route
_setoutfile
_setParameters
_setReturnCode
_setMessageId
_putMessage
_putMessageEx
_zCompress
_zDecompress
'''.splitlines()

for method in methods:
   print (f'static PyObject* {method}(PyObject* self, PyObject* args)')
   print ('{')
   print (f' return check_run(self, args, "{method}", DO{method});')
   print ('}\n')
