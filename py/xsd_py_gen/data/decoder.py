import base64

built_file = pocioci.b85

with open(built_file, 'rb') as ifile:
  with open('pococi', 'wb') as ofile:
    data_b85 = ifile.read()
    data = base64.b85decode(data_b85)
    ofile.write(data)



