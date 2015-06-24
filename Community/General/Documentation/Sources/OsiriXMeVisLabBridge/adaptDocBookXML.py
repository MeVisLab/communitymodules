# **InsertLicense** code
#! /usr/bin/env python

# adapt FrameMaker DocBook XML export
#
#     usage:  python adaptDocBookXML.py
#
# author: Felix Ritter

from __future__ import with_statement
import sys, re, os.path

# assume the xml file is located within the same directory as this script
file_to_adapt = os.path.join(sys.path[0],'OsiriXMeVisLabBridge.xml')

re_list = [
            # replaces docbook dtd version
            ['"-//OASIS//DTD DocBook XML V4.4//EN"' , '"-//OASIS//DTD DocBook XML V4.5//EN"'],
            # replaces docbook dtd reference version
            ['"http://www.oasis-open.org/docbook/xml/4.4/docbookx.dtd"' , '"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"'],
            # replace psd & pdf image references with png image references
            ['fm_figures\/([^.]*).p[sd][df]' , 'figures/\\1.png']
          ]

with open(file_to_adapt, 'r+U') as file:
    contents = file.read()
    for re_item in re_list:
        contents = re.compile(re_item[0]).sub(re_item[1], contents)
    file.seek(0)
    file.write(contents)
    file.truncate()
