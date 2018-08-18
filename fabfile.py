

import urllib2
import json
from fabric.api import env, local, run, lcd, cd, sudo, warn_only


def sass():
    """Start the sass scss compiler.. or was it sccs"""
    src_pth = "webserver/static/openplc.2.scss"
    out_pth = "webserver/static/openplc.2.css"
    #fname = "denzil.%s" % 5
    local("sass --trace --sourcemap=none --watch %s  " % (src_pth))