// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2014, 2026 Ingo Ruhnke <grumbel@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/color.hpp"
#include "flexlay/field.hpp"
#include "flexlay/math/point.hpp"
#include "flexlay/math/rect.hpp"
#include "flexlay/math/size.hpp"
#include "flexlay/qt_compat.hpp"

#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

#include <sstream>

/**
 * Minimal C++ simpleed shell.
 *
 * Milestone A target: grow this into a full tilemap/object editor
 * matching Python simpleed.py. For now it only proves the Qt6 +
 * libflexlay link and exercises Field / math types.
 */
int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QApplication::setApplicationName("flexlay-simpleed");
  QApplication::setOrganizationName("Flexlay");

  flexlay::Field field = flexlay::Field::from_size(8, 6);
  for (int y = 0; y < field.height(); ++y) {
    for (int x = 0; x < field.width(); ++x) {
      field.put(x, y, static_cast<uint32_t>(x + y * field.width()));
    }
  }

  const flexlay::Point p(11, 17);
  const flexlay::Size size(32, 32);
  const flexlay::Rect rect = flexlay::Rect::from_ps(p, size);
  const flexlay::Color color(40, 40, 48);

  std::ostringstream info;
  info << "flexlay-simpleed (C++ / Qt6)\n\n"
       << "libflexlay linked successfully.\n"
       << "Point: " << p.str() << "\n"
       << "Rect: " << rect.str() << " (w=" << rect.width() << " h=" << rect.height() << ")\n"
       << "Field: " << field.width() << "x" << field.height()
       << " sample at (3,2)=" << field.at(3, 2) << "\n\n"
       << "Next: map widget, tile paint, tools, undo.";

  QMainWindow window;
  window.setWindowTitle("Flexlay Simple Editor (C++)");
  window.resize(640, 360);

  auto* central = new QWidget(&window);
  auto* layout = new QVBoxLayout(central);
  auto* label = new QLabel(QString::fromStdString(info.str()), central);
  label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  label->setWordWrap(true);
  label->setStyleSheet(
    QString("QLabel { background-color: rgb(%1,%2,%3); color: #e0e0e0; padding: 16px; font-family: monospace; }")
      .arg(color.r)
      .arg(color.g)
      .arg(color.b));
  layout->addWidget(label);
  window.setCentralWidget(central);
  window.show();

  return app.exec();
}
