// **InsertLicense** code

/*
 _______________________________________________________________________
 __________________  F R A U N H O F E R   M E V I S  __________________
 |
 |   $Rev: 100478 $
 |
 |   Author(s)      : Felix Ritter
 |   Creation date  : 02/2010
 |
 __________________  F R A U N H O F E R   M E V I S  __________________
 _______________________________________________________________________
 */

#include "mlabOsiriXBridgeControl.h"
#include <Tools/mlabBasics.h>
#include <GUI/mlabWidgetControl.h>
#include <GUI/mlabWidgetControlContextInfo.h>
#include <GUI/mlabWidgetControlFactory.h>
#include <Scripting/mlabScriptSignalHandler.h>
#if defined(MACOS)
# include "mlabOsiriXBridgeWindowDelegate.h"
# include <macObjectiveAutoreleasePool.h>
#endif


MLAB_PLUGIN_INIT()
{
  mlabInfoConst("OsiriXBridgeControl loaded.");

  MLAB_ADD_WIDGETCONTROL("OsiriXBridge", MLABOsiriXBridgeControl);
}


//
//  MLABOsiriXBridgeControl
//

MLABOsiriXBridgeControl::MLABOsiriXBridgeControl() : _p(NULL)
{
#if defined(MACOS)



#endif
}

MLABOsiriXBridgeControl::~MLABOsiriXBridgeControl()
{
#if defined(MACOS)

  delete _p;

#endif
}

QWidget *
MLABOsiriXBridgeControl::createWidget(QWidget *parent)
{
#if defined(MACOS)

  if (parent) {
    QWidget *window = parent->window();
    _p = new MLABOsiriXBridgeWindowDelegate(this, window);
    
    MLABTree *attrib;
    CONTROL_FIND_LOCAL(attrib, QLatin1String("windowFilePathField"));
    if (attrib) {
      _windowFilePathField = lookupField(attrib);
      if (_windowFilePathField) {
        MLABFIELD_ADD_OBSERVER(_windowFilePathField, this, fieldChanged);
        fieldChanged();
      }
    }
    
    static QMap<QString,QString> commands;
    commands.insert(QLatin1String("droppedSeriesCommand"), QLatin1String("seriesDropped(const QVariant &)"));
    MLABScriptSignalHandler::setupSignalHandlers(_tree, this, _module, _windowId, commands);
  }

#endif

  return NULL;
}

void
MLABOsiriXBridgeControl::fieldChanged()
{
#if defined(MACOS)

  if (_windowFilePathField) {
    setWindowFilePath(_windowFilePathField->stringValue());
  }

#endif
}

QString
MLABOsiriXBridgeControl::windowFilePath() const
{
#if defined(MACOS)

  return (_p) ? _p->windowFilePath() : QString();

#else

  return QString();

#endif
}

void
MLABOsiriXBridgeControl::setWindowFilePath(const QString &filePath)
{
#if defined(MACOS)

  if (_p) {
    _p->setWindowFilePath(filePath);
  }

#endif
}

void
MLABOsiriXBridgeControl::emitSeriesDropped(const QVariant &info)
{
#if defined(MACOS)

  emit seriesDropped(info);

#endif
}


QString
MLABOsiriXBridgeControl::findValue(const QString &name, const QString &def) 
{
#if defined(MACOS)

  MLABTree *t = findLocalNoCase(name);
  return (t) ? t->getValue() : def;

#else

  return def;

#endif
}
