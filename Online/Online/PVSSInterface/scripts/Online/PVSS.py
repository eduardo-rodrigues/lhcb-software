import os, sys, time, string, platform
import PyCintex as PyLCGDict

#PyLCGDict.gbl.Cintex.SetDebug(1)
gbl  = PyLCGDict.makeNamespace('')
PVSS = PyLCGDict.makeNamespace('PVSS')
if platform.system()=='Linux':
  PyLCGDict.loadDict('libSTLRflx')
  PyLCGDict.loadDict('libPVSSInterfaceDict')
else:
  PyLCGDict.loadDict('STLRflx')

#  PyLCGDict.loadDict('PVSSInterfaceDict')
  
# == External class definitions ===============================================
Sensor            = gbl.Sensor
Event             = gbl.Event
Interactor        = gbl.Interactor
IAPIManager       = PVSS.IAPIManager
Printer           = PVSS.Printer
DeviceSensor      = PVSS.DeviceSensor
DevElement        = PVSS.DevElement
DevTypeElement    = PVSS.DevTypeElement
DeviceIO          = PVSS.DeviceIO
ReadTransaction   = PVSS.ReadTransaction
CfgDetail         = PVSS.CfgDetail
CfgAttribute      = PVSS.CfgAttribute
NamedIdentified   = PVSS.NamedIdentified
BitAlarm          = PVSS.BitAlarm
BasicAlarm        = PVSS.BasicAlarm
ArchiveSetting    = PVSS.ArchiveSetting
CfgSetting        = PVSS.CfgSetting
WriteTransaction  = PVSS.WriteTransaction
DpID              = PVSS.DpID
DataPoint         = PVSS.DataPoint
def DataPoint_get(self):
  return self.value().data()
def DataPoint_set(self,value):
  import traceback
  #print 'DataPoint_set:',value.__class__
  if isinstance(value,list) or isinstance(value,tuple):
    if len(value) > 0:
      traceback.print_stack()
      print 'DataPoint_set_item:',value.__class__,value[0].__class__
      if isinstance(value[0],int):
        v = gbl.std.vector('int')()
        for i in value: v.push_back(i)
        self.set(v)
        return self
      elif isinstance(value[0],float):
        v = gbl.std.vector('float')()
        for i in value: v.push_back(i)
        self.set(v)
        return self
      elif isinstance(value[0],str):
        v = gbl.std.vector('std::string')()
        for i in value: v.push_back(i)
        self.set(v)
        return self
      elif isinstance(value[0],DataPoint):
        v = gbl.std.vector('PVSS::DataPoint')()
        for i in value: v.push_back(i)
        self.set(v)
        return self
      elif isinstance(value[0],DpID):
        v = gbl.std.vector('PVSS::DpID')()
        for i in value: v.push_back(i)
        self.set(v)
        return self
    else:
      try:
        self.value().data().clear()
      except:
        pass
  self.set(value)
  return self
DataPoint.data = property(DataPoint_get,DataPoint_set)

def debug():
  return PVSS.pvss_debug()
def setDebug(val):
  PVSS.pvss_set_debug(val)
DPTime            = PVSS.DPTime
DPRef             = PVSS.DPRef
Value             = PVSS.Value
ControlsManager   = PVSS.ControlsManager
Environment       = PVSS.Environment
DevAnswer         = PVSS.DevAnswer
CfgType           = PVSS.CfgType
CfgManager        = PVSS.CfgManager
DevType           = PVSS.DevType
DevTypeManager    = PVSS.DevTypeManager
DeviceManager     = PVSS.DeviceManager
HotLinkCallback   = PVSS.HotLinkCallback
PyDeviceListener  = PVSS.PyDeviceListener
DpVectorActor     = PVSS.DataPointContainerActor('std::vector<PVSS::DataPoint>')
DpListActor       = PVSS.DataPointContainerActor('std::list<PVSS::DataPoint>')
# == External function definitions =============================================
defaultSystemID   = PVSS.defaultSystemID
defaultSystemName = PVSS.defaultSystemName
# == STL container definitions =================================================
DpIDVector        = gbl.std.vector('PVSS::DpID')
DataPointVector   = gbl.std.vector('PVSS::DataPoint')
DataPointVectorP  = gbl.std.vector('PVSS::DataPoint*')
DataPointList     = gbl.std.list('PVSS::DataPoint')
DataPointIdMap    = gbl.std.map('PVSS::DpID','PVSS::DataPoint*')
DataPointStringMap= gbl.std.map('std::string','PVSS::DataPoint*')
StringVector      = gbl.std.vector('std::string')
IntVector         = gbl.std.vector('int')

# =============================================================================
def defaultSystem():
  return (PVSS.defaultSystemID(),PVSS.defaultSystemName())
# =============================================================================
class APIManager:
  """
      Wrapper around PVSS APIManager instance.
      Should never be directly instantiated by the user
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self,dll='',function=''):
    "Create PVSS API manager for python."
    apiManager = PVSS.pvss_create_manager(dll, function)
    return apiManager.start()
# Instantiate API manager. Should never be called by user directly
if platform.system()=='Linux':
  pass
else:
  apiManager = APIManager()

# =============================================================================
def controlsMgr(systemID=None,systemName=None):
  "Create controls manager."
  if systemID is None: systemID = PVSS.defaultSystemID()
  if systemName is None: systemName = PVSS.defaultSystemName()
  return PVSS.ControlsManager(systemID, systemName)

# =============================================================================
class DeviceListener(PyDeviceListener):
  """ 
      Standard Python PVSS DeviceListener
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    self.manager = manager
    PyDeviceListener.__init__(self, self, self.manager)

  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    return 1
  
# =============================================================================
class DevicePrintListener(PyDeviceListener):
  """ 
      Python PVSS DeviceListener, which prints the content of the 
      sensor's datapoint list
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    PyDeviceListener.__init__(self,self, manager)

  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    d = self.devArray()
    print 'Devices changed:',d.size()
    for i in d:
      print '  ->',i.name(), '=', i.value().data()
    return 1
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    print 'Device changed:', self.dp().name(), '=', self.dp().value().data()
    return 1
  
# =============================================================================
class CommandListener(PyDeviceListener):
  """ 
      Python PVSS DeviceListener, which prints the content of the 
      sensor's datapoint list
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, sysname, datapoint, npar, cmd='Command', state='State'):
    "Standard constructor: needs ControlsManager as argument "
    PyDeviceListener.__init__(self,self, manager)
    self.manager  = manager
    self.sysname  = sysname
    self.numParam = npar
    self.writer   = self.manager.devWriter()
    self.control  = DataPoint(self.manager,DataPoint.original(datapoint+'.'+cmd))
    self.state    = DataPoint(self.manager,DataPoint.original(datapoint+'.'+state))
    self.sensor   = DeviceSensor(self.manager,self.control)

  # ===========================================================================
  def run(self):
    "Run the instance and listen to commands."
    self.sensor.addListener(self)
    self.sensor.run(1)
    
  # ===========================================================================
  def makeAnswer(self,status,msg):
    "Create answer object from status."
    self.state.data = status + msg
    self.writer.add(self.state)
    if self.writer.execute(0,1):
      return 1
    return 0
  
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def handle(self,items):
    "Dummy routine to be overloaded by clients."
    pass
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    from Online.Utils import log, error
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      log('Command received:'+nam[:nam.find(':')]+' -> '+cmd,timestamp=1)
      itms = cmd.split('/')
      if len(itms) == self.numParam:
        command   = itms[0]
        sysName   = itms[1]
        answer = ''
        for i in xrange(len(itms)):
          if i > 0: answer = answer + '/' + itms[i]
        result = None
        if sysName == self.sysname:
          try:
            result = self.handle(itms, answer)
            if result is None:
              error('The command:"'+cmd+'" failed. [Internal Error] ',timestamp=1)
              return self.makeAnswer('ERROR',answer)
            return result
          except Exception,X:
            error('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        error('The command:"'+cmd+'" failed. [Bad System] '+sysName,timestamp=1)
        return self.makeAnswer('ERROR',answer)
      error('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1)
      return 0
    except Exception,X:
      error('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
      traceback.print_exc()
      return 0
    except:
      error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
      traceback.print_exc()
    return 0
