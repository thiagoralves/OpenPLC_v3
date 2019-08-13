# -*- coding: utf-8 -*-

import os
import glob
import json
import uuid
import datetime

from . import PY3
if PY3:
    import urllib.request
else:
    import urllib2


def read_file(file_path):
    if not os.path.exists(file_path):
        return None, "file path '%s' not exist" % file_path

    with open(file_path, "r") as f:
        return f.read(), None
    return None, "Error reading '%' " % file_path

def write_file(file_path, contents):
    with open(file_path, "w") as f:
        f.write(contents)
        return None
    return "OOPS in write_file()"


def to_json(data, minify=False):
    """Serializes python data to a :ref:`json` string

    :type data: dict or list
    :param data: the python data to be encoded
    :type minify: bool
    :param minify:

            - When **`False`** the json string is minimized with no spaces, new lines etc.
            - When **`True`** the json string is human readable indented with four spaces, and sorted by key.

            .. note:: **Important**

                - By default this project uses **`minify=False`**.
                - For versioning (eg git), it is recommended to use **`minify=False`** as the string
                  will always be the same, ie sorted keys, and indentation


    :return: a `tuple` containing

             - `str` with the encoded json
             - An `Error` message if encoding error, otherwise `None`
    """
    # todo: catch json error
    if minify:
        return json.dumps(data, ensure_ascii=True, separators=(',', ':')), None
    return json.dumps(data, ensure_ascii=True, indent=4, sort_keys=True, separators=(',', ': ')), None



def read_json_dir(dir_path):
    files, err = list_dir(dir_path)
    print(files, err)
    lst = []
    for fn in files:
        rec, err = read_json_file(os.path.join(dir_path, fn))
        if err:
            # return None, err
            print("ERROR", err)
            # TODO
            continue
        lst.append(rec)
    return lst, None


def write_json_file(file_path, data, minify=False):
    """Saves python data in a :ref:`json` encoded file

    :param file_path: The relative or absolute path of file to save.
    :type file_path: str
    :param data: The python data to save
    :type data: dict or list
    :param minify: see :func:`~ogt.utils.to_json`
    :type minify: bool
    :return: `Error` message if write error, otherwise `None`
    """
    with open(file_path, "w") as f:
        json_str, err = to_json(data, minify=minify)
        if err:
            return err
        f.write(json_str)
        f.close()
    return None

def read_json_file(file_path):
    """Read and decodes a :ref:`json` encoded file


    :param file_path: The relative or absolute path of file to read.
    :type file_path: str
    :rtype: tuple
    :return:
             - A decoded python `dict` or `list`
             - An `Error` message if file or decoding error, otherwise `None`
    """
    try:
        with open(file_path, "r") as f:
            data = json.load(f)
            return data, None
    except Exception as e:
        return None, str(e)


def delete_dir_contents(dir_path):
    """Deletes all the contents of a directory, not the directory itself

    :type dir_path: str
    :param dir_path: The relative or absolute path to the dir
    """
    filelist = glob.glob("%s/*" % dir_path)
    for f in filelist:
        #print "f=", f
        pass #os.remove(f)


def list_dir(dir_path, path=True):
    """return list of files in a dir"""
    try:
        lst = os.listdir(dir_path), None
        if path:
            return [os.path.join(dir_path, fn) for fn in lst]
        return lst

    except Exception as e:
        return None, str(e)


def to_int(obj):
    try:
        return int(obj)
    except:
        pass
    return None


def http_fetch(url, plain=False):

    if PY3:
        try:
            contents = urllib.request.urlopen(url).read()
            if plain:
                return contents, None
            return json.loads(contents), None

        except Exception as e:
            return None, str(e)
    else:
        try:
            contents = urllib2.urlopen(url).read()
            if plain:
                return contents, None
            return json.loads(contents), None

        except Exception as e:
            return None, str(e)


def gen_uuid():
    """Generates an unique ID"""

    # return base64.b64encode(os.urandom(32))[:8]
    return str(uuid.uuid4().hex)[:8]

def now(timestamp=False):
    utc_ts = datetime.datetime.utcnow()
    if timestamp:
        return utc_ts
    return datetime.datetime.strftime(utc_ts, "%Y-%m-%d %H:%M:%S")



