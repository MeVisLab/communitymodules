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

#ifndef MLABOSIRIXBRIDGECONTROL_H_
#define MLABOSIRIXBRIDGECONTROL_H_

#include <GUI/mlabWidgetControl.h>

class MLABField;
class MLABOsiriXBridgeWindowDelegate;

class MLABOsiriXBridgeControl : public MLABSimpleWidgetControl
{
  friend class MLABOsiriXBridgeWindowDelegate;
  
  Q_OBJECT
  
public:
  MLABOsiriXBridgeControl();
  virtual ~MLABOsiriXBridgeControl();
  
  //! create the widget
  virtual QWidget *createWidget(QWidget *parent);
  
  MLABFIELD_CHANGED0_CB(fieldChanged, MLABOsiriXBridgeControl)
  
public slots:
  void fieldChanged();
  
  QString windowFilePath() const;
  void setWindowFilePath(const QString &filePath);

signals:
  void seriesDropped(const QVariant &);
  
private:
  //! finds the MDL tag name and returns its value
  QString findValue(const QString &name, const QString &def = QString());
  
  //! callable by other objects
  void emitSeriesDropped(const QVariant &);
  
  MLABOsiriXBridgeWindowDelegate *_p;
  
  QPointer<MLABField> _windowFilePathField;
};

#endif
