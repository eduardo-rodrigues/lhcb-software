#! /usr/bin/env python

import CondDBUI
from PyCool import cool
import os
import datetime

def dateString():
    import datetime
    now = datetime.datetime.now()
    return now.strftime("%d%m%Y")

def getUNIXTime(dtime):
    # Note dtime must be a date in CET (CERN) time.
    import time
    t = time.mktime(dtime.timetuple())
    zone = time.tzname[0]
    if zone not in ['GMT','CET'] : raise Exception('Unknown time zone '+zone)
    offset = 0
    if time.tzname[0] == 'GMT' : offset = -3600
    return int( (t+offset) * 1e9 )

def genXML(root,cond):
    file = open(root+cond)
    data = ""
    for line in file :
        if -1 != line.find("../../../DTD/structure.dtd"):
            data += """<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">"""
        else:
            data += line
    file.close()
    return data

def addToDB(startTime,rootToFiles,condPath,db):
    start = getUNIXTime(startTime) 
    stop  = cool.ValidityKeyMax
    print " -> Condition", condPath, "updated"
    db.storeXMLString( condPath, genXML(rootToFiles,condPath), start, stop )

def findXMLFiles(rootdir):
    import os
    fileList = [ ]
    for root, subFolders, files in os.walk(rootdir):
        for file in files :
            splits = file.split(".")
            if len(splits) > 1 and splits[len(splits)-1] == "xml" :
                fileList.append(os.path.join(root,file))
    return fileList

def condDBPathFromFile(rootdir,fullfile):
    return fullfile.replace(rootdir,"")

def fileMD5(file):
    import hashlib
    infile = open(file,'rb')
    content = infile.read()
    infile.close()
    m = hashlib.md5()
    m.update(content)
    return m.hexdigest()

# Mirror alignment
#upAlign    = "/usera/jonesc/NFS/DetDB/Mirrors/Up"
dnAlign    = "/usera/jonesc/NFS/DetDB/TmpMirrors"
#dbFileName = "NewMirrorHPDAlignFieldPolarity"
dbFileName = "FixedRICH1Align"

# Mirror alignment
#upAlign    = "/usera/jonesc/NFS/DetDB/Mirrors/Up"
#dnAlign    = "/usera/jonesc/NFS/DetDB/Mirrors/Down"
#dbFileName = "NewMirrorHPDAlignFieldPolarity"
#dbFileName = "NewMirrorAlign"

# Detector Numbers
#upAlign = "/usera/jonesc/NFS/DetDB/DetNumbers"
#dnAlign = "/usera/jonesc/NFS/DetDB/DetNumbers"
#dbFileName = "NewDetNumbersCondDB"

# MDMS
#upAlign = "/usera/jonesc/NFS/DetDB/MDMS"
#dnAlign = "/usera/jonesc/NFS/DetDB/MDMS"
#dbFileName = "NewMDMSCondDB"

# The following dates are extracted from spreadsheets here
# http://marwww.in2p3.fr/~legac/LHCb/

# Hardcode the field changes. Format is date of change and the new polarity
# Dates are in CET !!
field = { }
#                          Year  Month  Day   Hour   Min  Sec

# 2010
#field[ datetime.datetime(  2009,  9,     1,    1,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  4,     5,    2,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  4,     6,    2,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  5,     8,   22,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  5,    14,   12,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  5,    15,   16,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  5,    19,    5,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  7,    13,    4,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  7,    28,   21,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  8,    18,    8,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  8,    29,   17,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  9,    22,   16,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  10,   24,    9,     0,   0  ) ] = dnAlign

# 2011
field[ datetime.datetime(  2011,  1,   1,    1,     0,   0  ) ] = dnAlign 

# Open a new DB file
fulDBname = dbFileName + "-" + dateString() + ".db"
if os.path.exists(fulDBname) : os.remove(fulDBname)
db = CondDBUI.CondDB( "sqlite_file:"+fulDBname+"/LHCBCOND",
                      create_new_db=True, readOnly=False )
print "Opened DB file", fulDBname

createdPaths = [ ]
lastMDsums   = { }

# Loop over the data and fill the DB slice
for start in sorted(field.keys()):
    align = field[start]
    print start, align
    
    # Find XML files
    condPaths = findXMLFiles(align)

    # Fill into the DB
    for path in condPaths :

        # Get CondDB path from file name and rootdir
        condName = condDBPathFromFile(align,path)

        # Create DB path in DB file if first time
        if condName not in createdPaths :
            print " -> Creating DB path", condName
            db.createNode(condName)
            createdPaths += [condName]

        # Check MD5 sum to previous condition
        md = fileMD5(path)
        if condName not in lastMDsums.keys() : lastMDsums[condName] = 0

        # Check if update is needed
        if md != lastMDsums[condName] :
            addToDB(start,align,condName,db)
            lastMDsums[condName] = md
        else:
            print " -> Condition", condName, "same as previous. No update needed"
