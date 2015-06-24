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

#ifndef OSIRIXPASTEBOARDMIME_H_
#define OSIRIXPASTEBOARDMIME_H_

#include <QtGui/QMacPasteboardMime>

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
