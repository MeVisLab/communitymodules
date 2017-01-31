//
//  OsiriXPasteboardMime.h
//  MLABOsiriXBridgeControl
//
//  Created by Felix Ritter on 01.02.10.
//  This code is in the public domain.
//

#ifndef OSIRIXPASTEBOARDMIME_H_
#define OSIRIXPASTEBOARDMIME_H_

#include <QMacPasteboardMime>

class OsiriXPasteboardMime : public QMacPasteboardMime
{
public:
  OsiriXPasteboardMime();
  
  QString convertorName();
  bool canConvert(const QString &mime, QString flav);
  
  QString mimeFor(QString flav);
  QString flavorFor(const QString &mime);
  
  QVariant convertToMime(const QString &mime, QList<QByteArray> data, QString flav);
  QList<QByteArray> convertFromMime(const QString &mime, QVariant data, QString flav);
};

#endif
