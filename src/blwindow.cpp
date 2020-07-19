#include <QApplication>

#include "blwindow.h"

enum class Style : DWORD {
    windowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
    aero_borderless = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    basic_borderless = WS_POPUP | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
};

auto maximized(HWND hwnd) -> bool {
    WINDOWPLACEMENT placement;
    if (!::GetWindowPlacement(hwnd, &placement)) {
        return false;
    }

    return placement.showCmd == SW_MAXIMIZE;
}

auto adjust_maximized_client_rect(HWND window, RECT& rect) -> void {
    if (!maximized(window)) {
        return;
    }

    auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return;
    }

    MONITORINFO monitor_info{};
    monitor_info.cbSize = sizeof(monitor_info);
    if (!::GetMonitorInfoW(monitor, &monitor_info)) {
        return;
    }

    // when maximized, make the client area fill just the monitor (without task bar) rect,
    // not the whole window rect which extends beyond the monitor.
    rect = monitor_info.rcWork;
}

auto composition_enabled() -> bool {
    auto composition_enabled = FALSE;
    auto success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
    return composition_enabled && success;
}

auto select_borderless_style() -> Style {
    return composition_enabled() ? Style::aero_borderless : Style::basic_borderless;
}

BLWindow::BLWindow(QWindow *parent) :
    QQuickWindow(parent)
{
    setFlags(Qt::FramelessWindowHint);
    set_borderless(true);

}

bool BLWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    auto msg = static_cast<MSG *>(message);

    switch (msg->message)
    {
    case WM_NCCREATE: {
            auto userdata = reinterpret_cast<CREATESTRUCTW*>(msg->lParam)->lpCreateParams;
            // store window instance pointer in window user data
            ::SetWindowLongPtrW(msg->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userdata));
        }
    case WM_ERASEBKGND: {
            auto brush = CreateSolidBrush(RGB(48, 48, 48));
            SetClassLongPtr(msg->hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush));
        }

    case WM_NCCALCSIZE: {
            if (msg->wParam == TRUE) {
                auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                adjust_maximized_client_rect(msg->hwnd, params.rgrc[0]);
            }
            *result = 0;
            return true;
        }

    case WM_NCHITTEST: {
            *result = 0;
            const LONG border_width = 4; //in pixels
            RECT winrect;
            GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            auto resizeWidth = minimumWidth() != maximumWidth();
            auto resizeHeight = minimumHeight() != maximumHeight();

            if (resizeWidth) {
                //left border
                if (x >= winrect.left && x < winrect.left + border_width) {
                    *result = HTLEFT;
                }
                //right border
                if (x < winrect.right && x >= winrect.right - border_width) {
                    *result = HTRIGHT;
                }
            }
            if (resizeHeight) {
                //bottom border
                if (y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOM;
                }
                //top border
                if (y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOP;
                }
            }
            if (resizeWidth && resizeHeight) {
                //bottom left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOMLEFT;
                }
                //bottom right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOMRIGHT;
                }
                //top left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                    y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOPLEFT;
                }
                //top right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                    y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOPRIGHT;
                }
            }

            if(x >= winrect.right - 46 * 3 && y <= winrect.top + 30) {
                *result = HTCLIENT; // TODO: Fix
            }

            if (*result != 0)
                return true;

            if (y >= winrect.top && y < winrect.top + 20) {
                *result = HTCAPTION;
                return true;
            }
            break;

    //        auto action = QApplication::widgetAt(QCursor::pos());
    //        if (action == title_bar_widget_) {
    //            *result = HTCAPTION;
    //            return true;
    //        }
    //        break;
        }

    case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(msg->lParam);

            if (maximized(msg->hwnd)) {

                RECT window_rect;

                if (!GetWindowRect(msg->hwnd, &window_rect)) {
                    return false;

                }

                HMONITOR monitor = MonitorFromRect(&window_rect, MONITOR_DEFAULTTONULL);
                if (!monitor) {
                    return false;
                }

                MONITORINFO monitor_info = { 0 };
                monitor_info.cbSize = sizeof(monitor_info);
                GetMonitorInfo(monitor, &monitor_info);

                RECT work_area = monitor_info.rcWork;
                RECT monitor_rect = monitor_info.rcMonitor;

                mmi->ptMaxPosition.x = abs(work_area.left - monitor_rect.left);
                mmi->ptMaxPosition.y = abs(work_area.top - monitor_rect.top);

                mmi->ptMaxSize.x = abs(work_area.right - work_area.left);
                mmi->ptMaxSize.y = abs(work_area.bottom - work_area.top);
                mmi->ptMaxTrackSize.x = mmi->ptMaxSize.x;
                mmi->ptMaxTrackSize.y = mmi->ptMaxSize.y;

                *result = 1;
                return true;
            }
        }

    case WM_NCACTIVATE: {
            if (!composition_enabled()) {
                // Prevents window frame reappearing on window activation
                // in "basic" theme, where no aero shadow is present.
                *result = 1;
                return true;
            }
            break;
        }

    case WM_SIZE: {
            RECT winrect;
            GetClientRect(msg->hwnd, &winrect);
            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(msg->hwnd, &wp);
            if (this) {
                if (wp.showCmd == SW_MAXIMIZE) {
                    ::SetWindowPos(reinterpret_cast<HWND>(winId()), Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
                }
            }
        }

    default: QQuickWindow::nativeEvent(eventType, message, result);
    } // switch

    return QQuickWindow::nativeEvent(eventType, message, result);
}

auto BLWindow::set_borderless(bool enabled) const -> void
{
    auto new_style = (enabled) ? select_borderless_style() : Style::windowed;
    auto old_style = static_cast<Style>(::GetWindowLongPtrW(reinterpret_cast<HWND>(winId()), GWL_STYLE));

    if (new_style != old_style) {
        ::SetWindowLongPtrW(reinterpret_cast<HWND>(winId()), GWL_STYLE, static_cast<LONG>(new_style));

        // redraw frame
        ::SetWindowPos(reinterpret_cast<HWND>(winId()), nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
    }
}

