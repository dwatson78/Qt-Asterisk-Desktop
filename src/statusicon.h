#ifndef __STATUSICON_H__
#define __STATUSICON_H__

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>

class StatusIcon : public QWidget
{
  Q_OBJECT
public:
  explicit StatusIcon(QWidget *parent = 0);

public slots:
  void setConnected(bool connected);
  void setLoggedIn(bool loggedIn, QString message);

protected:

signals:

private:
  QHBoxLayout* _lyt;
  QLabel* _lblText;
  QLabel* _lblIcon;
  bool isConnected;
  bool isLoggedIn;
};

#endif // __STATUSICON_H__
