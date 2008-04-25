import unittest
from LbUtils import VersionsDB
__author__ = "Marco Clemencic <Marco.Clemencic@cern.ch>"
__version__ = "$Id: test_VersionsDB.py,v 1.1 2008-04-25 15:33:51 marcocle Exp $"

class VersionsDBTest(unittest.TestCase):
    def _prepareXML(self, data):
        """Prepare the XML to be tested.
        'data' must have this format:
        [("ReleaseName","BaseRelease",[("proj1","v1"),("proj2","v2"),...]),...]"""
        xml = """<?xml version="1.0" encoding="ISO-8859-1"?><releases_db>"""
        for name,base,projects in data:
            xml += '<release name="' + name
            if base: xml += '" base="' + base
            xml += '">'
            for p_v in projects:
                xml += '<project name="%s" version="%s"/>'%p_v
            xml += '</release>'
        xml += """</releases_db>"""
        return xml
    def _fillDB(self,data):
        VersionsDB.loadString(self._prepareXML(data))
    
    def setUp(self):
        VersionsDB.Release.__releases__ = {}
    def tearDown(self):
        VersionsDB.Release.__releases__ = {}
    
    def test_010_create_release(self):
        ra = VersionsDB.Release("ReleaseA")
        rb = VersionsDB.Release("ReleaseB",base="ReleaseA")
        self.assertEqual(VersionsDB.getRelease("ReleaseA"),ra)
        self.assertEqual(VersionsDB.getRelease("ReleaseB"),rb)
        self.assertEquals(rb.base, "ReleaseA")
    def test_020_duplicate_release(self):
        VersionsDB.Release("ARelease")
        try:
            VersionsDB.Release("ARelease")
            self.fail("DuplicatedReleaseError exception expected")
        except VersionsDB.DuplicatedReleaseError, x:
            self.assertEquals(x.release,"ARelease")
    def test_030_add_project(self):
        r = VersionsDB.Release("ReleaseA")
        r["Project1"] = "version1"
        r["Project2"] = "version2"
        r = VersionsDB.Release("ReleaseB",base="ReleaseA")
        r["Project2"] = "version2b"
        
        r = VersionsDB.getRelease("ReleaseA")
        self.assertEquals(r["Project1"],"version1")
        self.assertEquals(r["Project2"],"version2")
        r = VersionsDB.getRelease("ReleaseB")
        self.assertEquals(r["Project1"],"version1")
        self.assertEquals(r["Project2"],"version2b")
    def test_040_missing_project(self):
        r = VersionsDB.Release("ReleaseA")
        r["Project1"] = "version1"
        r["Project2"] = "version2"
        self.assert_("Project3" not in r)
        self.assert_(not ("Project3" in r))
        
    def test_110_basic_parse(self):
        """Basic parsing"""
        data = [("ReleaseName",None,[("Proj%d"%i,"v%d"%i) for i in range(3)])]
        VersionsDB.loadString(self._prepareXML(data))

    def test_120_parse_errors(self):
        """XML errors"""
        data = [("ReleaseName",None,[]),("ReleaseName",None,[])]
        try:
            VersionsDB.loadString(self._prepareXML(data))
            self.fail("DuplicatedReleaseError exception expected")
        except VersionsDB.DuplicatedReleaseError:
            pass

        data = [("ReleaseName",None,[("P","V"),("P","V")])]
        try:
            VersionsDB.loadString(self._prepareXML(data))
            self.fail("DuplicatedProjectError exception expected")
        except VersionsDB.DuplicatedProjectError:
            pass
        
    def test_200_findReleases(self):
        data = [("R1",None,[("P1","V1"),("P2","V1"),("P3","V1")]),
                ("R2",None,[("P1","V1"),("P2","V2"),("P3","V1")]),
                ("R3","R2",[("P3","V2")]),
                ]
        self._fillDB(data)
        self.assertEquals(VersionsDB._findReleases("P1","V1"),["R1","R2","R3"])
        self.assertEquals(VersionsDB._findReleases("P2","V1"),["R1"])
        self.assertEquals(VersionsDB._findReleases("P2","V2"),["R2","R3"])
        self.assertEquals(VersionsDB._findReleases("P3","V1"),["R1","R2"])
        self.assertEquals(VersionsDB._findReleases("P3","V2"),["R3"])
        # all releases
        self.assertEquals(VersionsDB._findAllReleases("P2",["V1","V2"]),
                          ["R1","R2","R3"])
        # non-existing versions
        self.assertEquals(VersionsDB._findReleases("P1","V2"),[])
        self.assertEquals(VersionsDB._findReleases("P4","V1"),[])
        # get versions
        self.assertEquals(VersionsDB._getVersions("P1",["R1"]),["V1"])
        self.assertEquals(VersionsDB._getVersions("P1",["R1","R2","R3"]),["V1"])
        self.assertEquals(VersionsDB._getVersions("P2",["R1","R2","R3"]),["V1","V2"])
        self.assertEquals(VersionsDB._getVersions("P3",["R1","R2","R3"]),["V1","V2"])
        self.assertEquals(VersionsDB._getVersions("P5",["R1","R2","R3"]),[])

    def test_300_getCompatibleVersions(self):
        data = [("R1",None,[("P1","V1"),("P2","V1"),("P3","V1"),("P4","V1")]),
                ("R2",None,[("P1","V2"),("P2","V2"),("P3","V1"),("P4","V1")]),
                ("R3",None,[("P1","V3"),("P2","V2"),("P3","V2"),("P4","V1")]),
                ("R4",None,[("P1","V4"),("P2","V2"),("P3","V2"),("P4","V2")]),
                ("R5","R4",[                        ("P3","V3")]),
                ("R6",None,[("P1","V5"),("P2","V2"),("P3","V4"),("P4","V3")]),
                ]
        self._fillDB(data)
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V2",
                                                           "P4",
                                                           "P1"),["V1","V2"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V3",
                                                           "P4",
                                                           "P1"),["V2"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P4","V1",
                                                           "P3",
                                                           "P1"),["V1","V2"])
                
        self.assertEquals(VersionsDB.getCompatibleVersions("P2","V2",
                                                           "P3",
                                                           "P1"),["V1","V2","V3","V4"])
        
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V3",
                                                           "P4",
                                                           "P2"),["V1","V2","V3"])

        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V5",
                                                           "P4",
                                                           "P1"),[])
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V3",
                                                           "P5",
                                                           "P1"),[])
        self.assertEquals(VersionsDB.getCompatibleVersions("P3","V3",
                                                           "P4",
                                                           "P5"),[])


unittest.main()
