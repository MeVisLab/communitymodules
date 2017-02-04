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

#ifndef MLABOSIRIXBRIDGEWINDOWDELEGATE_H_
#define MLABOSIRIXBRIDGEWINDOWDELEGATE_H_

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtGui/QWidget>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QPaintEvent>

class MLABOsiriXBridgeControl;
class MLABOsiriXBridgeWindowDelegatePrivate;

class MLABOsiriXBridgeWindowDelegate : public QObject
{
  Q_OBJECT
  
public:
  MLABOsiriXBridgeWindowDelegate(MLABOsiriXBridgeControl *control, QWidget *widget);
  ~MLABOsiriXBridgeWindowDelegate();
  
  void setMLABWindowWidget(QWidget *widget);
  QWidget *mlabWindowWidget() const { return _mlabWindowWidget; }
  
  QString windowFilePath() const;
  void setWindowFilePath(const QString &filePath);
  
protected:
  bool eventFilter(QObject *obj, QEvent *event);
  
  bool paintEvent(QPaintEvent *event);
  
  bool dragEnterEvent(QDragEnterEvent *event);
  bool dragLeaveEvent(QDragLeaveEvent *event);
  bool dropEvent(QDropEvent *event);
  
private:
  MLABOsiriXBridgeControl *_control;
  QWidget *_mlabWindowWidget;
  
  bool _highlighted;
  
  MLABOsiriXBridgeWindowDelegatePrivate *_p;
};

#endif
