//
//  mlabOsiriXBridgeWindowDelegate.mm
//  MLABOsiriXBridgeControl
//
//  Created by Felix Ritter on 01.02.10.
//  This code is in the public domain.
//

#import "mlabOsiriXBridgeWindowDelegate.h"
#import "mlabOsiriXBridgeCocoaWindowDelegate.h"
#import "mlabOsiriXBridgeControl.h"
#import "OsiriXPasteboardMime.h"
#import <mlOsiriXServices.h>
#import <macObjectiveAutoreleasePool.h>
#import <QtGui>


class MLABOsiriXBridgeWindowDelegatePrivate
{
public:
  MLABOsiriXBridgeCocoaWindowDelegate *cocoaController;
  MLOsiriXServices *osirixServices;
  
  MLABOsiriXBridgeWindowDelegatePrivate() : cocoaController(nil), osirixServices(nil) {
    cocoaController = [[MLABOsiriXBridgeCocoaWindowDelegate alloc] init];
    osirixServices = [[MLOsiriXServices alloc] init];
  }
  ~MLABOsiriXBridgeWindowDelegatePrivate() {
    [cocoaController release];
    [osirixServices release];
  }
};



MLABOsiriXBridgeWindowDelegate::MLABOsiriXBridgeWindowDelegate(MLABOsiriXBridgeControl *control, QWidget *widget) : _control(control), _mlabWindowWidget(NULL), _highlighted(false)
{
  _p = new MLABOsiriXBridgeWindowDelegatePrivate;
  
  new OsiriXPasteboardMime;
  
  setMLABWindowWidget(widget);
}

MLABOsiriXBridgeWindowDelegate::~MLABOsiriXBridgeWindowDelegate()
{
  delete _p;
}

void
MLABOsiriXBridgeWindowDelegate::setMLABWindowWidget(QWidget *widget)
{
  if (_mlabWindowWidget != widget) {
    if (_mlabWindowWidget) {
      _mlabWindowWidget->removeEventFilter(this);
    }
    
    _mlabWindowWidget = widget;
    
    _mlabWindowWidget->setAcceptDrops(true);
    _mlabWindowWidget->installEventFilter(this);
    
    [_p->cocoaController setMLABWindowView:reinterpret_cast<NSView *> (_mlabWindowWidget->effectiveWinId())];
  }
}

QString
MLABOsiriXBridgeWindowDelegate::windowFilePath() const
{
  return (_mlabWindowWidget) ? _mlabWindowWidget->windowFilePath() : QString();
}

void
MLABOsiriXBridgeWindowDelegate::setWindowFilePath(const QString &filePath)
{
  if (_mlabWindowWidget) {
    _mlabWindowWidget->setWindowFilePath(filePath);
  }
}

bool
MLABOsiriXBridgeWindowDelegate::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::Paint) {
    return paintEvent(static_cast<QPaintEvent *>(event));
  } else if (event->type() == QEvent::DragEnter) {
    return dragEnterEvent(static_cast<QDragEnterEvent *>(event));
  } else if (event->type() == QEvent::DragLeave) {
    return dragLeaveEvent(static_cast<QDragLeaveEvent *>(event));
  } else if (event->type() == QEvent::Drop) {
    return dropEvent(static_cast<QDropEvent *>(event));
  } else {
    // standard event processing
    return QObject::eventFilter(obj, event);
  }
}

bool
MLABOsiriXBridgeWindowDelegate::paintEvent(QPaintEvent * /* event */)
{
  QPainter p(_mlabWindowWidget);
  
  if (_highlighted) {
    p.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    p.drawRect(QRect(1,1,_mlabWindowWidget->width()-3,_mlabWindowWidget->height()-3));
  }
  
  return true;
}

bool
MLABOsiriXBridgeWindowDelegate::dragEnterEvent(QDragEnterEvent *event)
{
  qDebug() << event->mimeData()->formats();
  
  bool acceptProposedAction = false;
  
  if (event->mimeData()->hasFormat("application/x-osirix")) {
    acceptProposedAction = true;
  }
  else if (event->mimeData()->hasUrls()) {
    acceptProposedAction = true;
  }
  
  if (acceptProposedAction) {
    event->acceptProposedAction();
    
    if (! _highlighted) {
      _highlighted = true;
      _mlabWindowWidget->update();
    }

    return true;
  }
  
  return false;
}

bool
MLABOsiriXBridgeWindowDelegate::dragLeaveEvent(QDragLeaveEvent * /* event */)
{
  if (_highlighted) {
    _highlighted = false;
    _mlabWindowWidget->update();
  }
  
  return true;
}

bool
MLABOsiriXBridgeWindowDelegate::dropEvent(QDropEvent *event)
{
  bool acceptProposedAction = false;
  
  if (event->source() == _mlabWindowWidget) {
    // deal with self droppings
  }
  
  if (event->mimeData()->hasFormat("application/x-osirix")) {
    // do something
    
    acceptProposedAction = true;
  }
  else if (event->mimeData()->hasUrls()) {
    QList<QUrl> urlList = event->mimeData()->urls();
    
    if (! urlList.isEmpty() && QFileInfo(urlList.first().toLocalFile()).exists()) {
      macos::ObjectiveAutoreleasePool pool;
      
      NSDictionary *info;
      @try {
        info = [_p->osirixServices seriesInfoOfViewerDisplayingImageSeriesWithFilename:[NSString stringWithUTF8String:urlList.first().toLocalFile().toUtf8().constData()]];
      }
      @catch (NSException *e) {
        NSLog (@"Exception in MLABOsiriXBridgeWindowDelegate::dropEvent():\nname: %@\nreason: %@", [e name], [e reason]);
        
        info = nil;
      }
      @catch (id e) {
        NSLog (@"Unknown exception in MLABOsiriXBridgeWindowDelegate::dropEvent()");
        
        info = nil;
      }
      
      if (info) {
        NSArray *images = [info objectForKey:@"series"];
        int curImage = [[info objectForKey:@"curImage"] intValue];
        float curImageWL = [[info objectForKey:@"curImageWL"] floatValue];
        float curImageWW = [[info objectForKey:@"curImageWW"] floatValue];
        
        QStringList imgList;
        for (NSString *filename in images) {
          imgList << [filename UTF8String];
        }
        
        QMap<QString, QVariant> dict;
        dict["series"] = QVariant(imgList);
        dict["curImage"] = QVariant(curImage);
        dict["curImageWL"] = QVariant(curImageWL);
        dict["curImageWW"] = QVariant(curImageWW);
        
        _control->emitSeriesDropped(QVariant(dict));
      }
    }
    
    acceptProposedAction = true;
  }
  
  if (_highlighted) {
    _highlighted = false;
    _mlabWindowWidget->update();
  }
  
  if (acceptProposedAction) {
    event->acceptProposedAction();
  }
  
  return true;
}
