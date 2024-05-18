import base64

#built_dir = r'C:\vlab\python\pytools\xsd_py_gen\data'
#zip_file = r'C:\vlab\python\pytools\xsd_py_gen\data\dbportal_linux_5.4_v0001.zip'

#files = '''\
#dbxsrc
#mod_date
#poci2j
#pocidump
#pocilink
#pocioci
#pocioci_sw
#'''.splitlines()

def a85encode(infile, outfile):
  print (infile, outfile)
  with open(infile, 'rb') as ifile:
    with open(outfile, 'wb') as ofile:
      outfile.write(f'{infile}\n')
      data = ifile.read()
      data_a85 = base64.a85encode(data, wrapcol=120)
      ofile.write(data_a85)

#def encode_built():
#  for file in files:
#    infile = f'{built_dir}\\{file}'
#    outfile = f'{built_dir}\\{file}.a85'
#    a85encode(infile, outfile)

def a85decode(infile, outfile=None):
  print (infile, outfile)
  with open(infile, 'rb') as ifile:
    outfile_line = infile.readline()
    with open(outfile, 'wb') as ofile:
      data_a85 = ifile.read()
      data = base64.a85decode(data_a85)
      ofile.write(data)

#def decode_built():
#  for file in files:
#    infile = f'{built_dir}\\{file}.a85'
#    outfile = f'{built_dir}\\{file}.aaa'
#    a85decode(infile, outfile)

#encode_built()
#decode_built()
#infile = zip_file
#outfile = zip_file.replace('.zip', '.a85')
#a85encode(infile, outfile)



