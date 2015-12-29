from mako.template import Template
import sys,os

# --- Parsing the arguments of the script and options
import argparse

parser = argparse.ArgumentParser(description="""
Generate the C++ file from the mako/C++  template
""")

parser.add_argument('filename', help = "Name of the file")
parser.add_argument('--outputname', '-o',  help="Name of the xxx_desc.py file [default is same as the filename]", default = '')
args = parser.parse_args()

#------------

output = args.outputname or args.filename.replace(r'.mako','')
tpl = Template(filename=args.filename)
rendered = tpl.render().strip()

with open('.'+ output,'w') as out : 
    out.write(rendered)

# clang formatting
os.system('/usr/local/opt/llvm/bin/clang-format -style=file .%s >%s'%(output,output))
os.system(' rm .%s'%output)
