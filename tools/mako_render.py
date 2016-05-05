# Usage : make_render filein 
# reads the first files
# ## filename a = x; b = y;  ....

from mako.template import Template
from mako.lookup import TemplateLookup

import sys, os
fi, outdir = sys.argv[1:3]

mylookup = TemplateLookup(directories=[os.path.dirname(fi)])
fi = open(fi, 'r').read()
tpl = Template(fi, lookup=mylookup)
lines = fi.split('\n')
for l in lines:
    if l.strip().startswith(r'/*') : continue
    if not l.strip().startswith('## ') : break
    _, fo, varlist = l.strip().split(' ',2)
    varlist_split = varlist.split(';')
    varlist = dict ( [ map(lambda x: x.strip(), x.split('=')) for x in varlist_split if x])
    try : 
        os.makedirs(outdir)
    except: 
        pass
    with open(outdir + '/' + fo,'w') as f:
        f.write(tpl.render(**varlist).strip())

#os.system('/usr/local/Cellar/llvm/3.6.2/bin/clang-format %s > %s'%(fo,fo))
