//
//  mlabOsiriXBridgeWindowDelegate.h
//  MLABOsiriXBridgeControl
//
//  Created by Felix Ritter on 01.02.10.
//  This code is in the public domain.
//

#ifndef MLABOSIRIXBRIDGEWINDOWDELEGATE_H_
#define MLABOSIRIXBRIDGEWINDOWDELEGATE_H_

#include <QObject>
#include <QEvent>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QPaintEvent>

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
