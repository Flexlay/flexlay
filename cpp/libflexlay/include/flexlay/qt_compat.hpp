// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLEXLAY_QT_COMPAT_HPP
#define FLEXLAY_QT_COMPAT_HPP

/** Qt conversion helpers — kept out of core math/color headers so pure
 *  domain types do not depend on Qt (tests, headless tools). */

#include "flexlay/color.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/math/size.hpp"

#include <QColor>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>
#include <QSizeF>

namespace flexlay {

inline Point point_from_qt(const QPoint& q) { return Point(q.x(), q.y()); }
inline QPoint to_qt(const Point& p) { return QPoint(p.x, p.y); }

inline Pointf pointf_from_qt(const QPointF& q)
{
  return Pointf(static_cast<float>(q.x()), static_cast<float>(q.y()));
}
inline QPointF to_qt(const Pointf& p) { return QPointF(p.x, p.y); }

inline QSize to_qt(const Size& s) { return QSize(s.width, s.height); }
inline QSizeF to_qt(const Sizef& s) { return QSizeF(s.width, s.height); }

inline QRect to_qt(const Rect& r) { return QRect(r.left, r.top, r.width(), r.height()); }
inline QRectF to_qt(const Rectf& r) { return QRectF(r.left, r.top, r.width(), r.height()); }

inline QColor to_qt(const Color& c) { return QColor(c.r, c.g, c.b, c.a); }
inline QColor to_qt(const Colorf& c) { return QColor::fromRgbF(c.r, c.g, c.b, c.a); }

} // namespace flexlay

#endif
