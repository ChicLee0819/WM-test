#ifndef WINDOWEVENTFILTER_H
#define WINDOWEVENTFILTER_H

#include <QObject>

class WindowEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit WindowEventFilter(QObject *parent = 0);

protected:
     bool eventFilter(QObject *obj, QEvent *event);

signals:
    void windowStateChange();
};

#endif // WINDOWEVENTFILTER_H