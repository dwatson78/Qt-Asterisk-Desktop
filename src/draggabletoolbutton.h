#ifndef DRAGGABLETOOLBUTTON_H
#define DRAGGABLETOOLBUTTON_H

#include <QToolButton>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QMimeData>

#include "admcallwidget.h"

class MimeDataAstCall : public QMimeData
{
  Q_OBJECT
public:
  explicit MimeDataAstCall();

  AdmCallWidget* admCallWidget() const;
  void setAdmCallWidget(AdmCallWidget *admCallWidget);
  bool hasAdmCallWidget() const;

  bool hasFormat(const QString& mimeType) const;
  QStringList formats() const;
  QVariant retrieveData(const QString& mimeType, QVariant::Type type) const;
private:
  AdmCallWidget* _admCallWidget;
};

class DraggableToolButton : public QToolButton
{
  Q_OBJECT
public:
  explicit DraggableToolButton(QWidget *parent = 0);

public slots:

protected:
  void mousePressEvent(QMouseEvent *event);

signals:

private:


};

#endif // DRAGGABLETOOLBUTTON_H
