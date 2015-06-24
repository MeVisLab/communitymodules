//
//  mlOsiriXBridge_p.h
//  MLOsiriXBridge
//
//  Created by Felix Ritter on 18.01.08.
//  This code is in the public domain.
//

// System includes
#import <Foundation/Foundation.h>
#import <mlModule.h>
#import <string>

// Forward declaration
@class BridgeNotificationHandler;
@class MeVisLabLoadDICOM;
@class MLOsiriXServices;
@class EatDicom;

ML_START_NAMESPACE

class OsiriXBridgePrivate
{
public:
  OsiriXBridgePrivate(Module *o);
  ~OsiriXBridgePrivate();
  
  void registerWithClient(const std::string &name);
  void registerWithClient();
  void unRegisterFromClient();
  
  void handleDICOMDataset();
  std::string osirixIncomingFolder();
  
  ml::Module *module() const { return obj; }
  const std::string &targetName() const { return _targetName; }
  /* OBTargetIdentifier */ unsigned long long targetIdentifier() const { return _targetIdentifier; }

private:
  ml::Module *obj;
  BridgeNotificationHandler *notifyHandler;
  
  MeVisLabLoadDICOM *loadDICOM;
  MLOsiriXServices *osirixServices;
  EatDicom *eatDicom;
  
  std::string _targetName;
  /* OBTargetIdentifier */ unsigned long long _targetIdentifier;
};

ML_END_NAMESPACE
