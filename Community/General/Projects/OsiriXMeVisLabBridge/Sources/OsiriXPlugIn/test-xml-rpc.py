# **InsertLicense** code
# Test OsiriXMeVisLabBridge XML-RPC

import xmlrpclib, socket

proxy = xmlrpclib.ServerProxy("http://localhost:8080/", verbose=True)

try:

    result = proxy.DBWindowFind( {"request": "(name LIKE '*Hauck*')", "table": "Study", "execute": "Select"} )
    print result["error"]
    result = proxy.SendSelectedSeriesToMeVisLab( {"target": "OsiriXBridge"} )
    print result["error"]

except (xmlrpclib.Error, socket.error), err:
    print "ERROR", err
