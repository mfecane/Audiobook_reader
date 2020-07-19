#pragma once

#include <QQuickWindow>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

class BLWindow : public QQuickWindow
{
    Q_OBJECT

public:
    explicit BLWindow(QWindow *parent = nullptr);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

signals:

private:
    auto set_borderless(bool enabled) const -> void;
};

