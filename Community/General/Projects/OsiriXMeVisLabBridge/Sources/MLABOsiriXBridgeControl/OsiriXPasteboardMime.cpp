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

#include "OsiriXPasteboardMime.h"
#include <QtGui/QtGui>

static const char *kPasteBoardOsiriXType = "OsiriX pasteboard";

OsiriXPasteboardMime::OsiriXPasteboardMime() : QMacPasteboardMime(MIME_ALL)
{
  // qRegisterDraggedTypes(QStringList() << kPasteBoardOsiriXType);
}

QString OsiriXPasteboardMime::convertorName()
{
  return QString("OsiriXPasteboardMime");
}

bool
OsiriXPasteboardMime::canConvert(const QString &mime, QString flav)
{
  return mimeFor(flav) == mime;
}

QString
OsiriXPasteboardMime::mimeFor(QString flav)
{
  qDebug() << flav;
  
  if (flav == QString(kPasteBoardOsiriXType))
    return QString("application/x-osirix");
  return QString();
}

QString
OsiriXPasteboardMime::flavorFor(const QString &mime)
{
  if (mime == QString(kPasteBoardOsiriXType))
    return QString("OsiriX pasteboard");
  return QString();
}

QVariant
OsiriXPasteboardMime::convertToMime(const QString &mime, QList<QByteArray> data, QString flav)
{
  Q_UNUSED(mime)
  Q_UNUSED(flav)
  
  QByteArray all;
  foreach (QByteArray i, data) {
    all += i;
  }
  return QVariant(all);
}

QList<QByteArray>
OsiriXPasteboardMime::convertFromMime(const QString &mime, QVariant data, QString flav)
{
  Q_UNUSED(mime)
  Q_UNUSED(data)
  Q_UNUSED(flav)
  
  // Do nothing
  return QList<QByteArray>();
}
