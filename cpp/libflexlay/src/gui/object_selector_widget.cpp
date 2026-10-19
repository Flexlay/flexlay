// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/object_selector_widget.hpp"

#include "flexlay/supertux/object_sprites.hpp"

#include <QIcon>
#include <QListWidgetItem>
#include <QPixmap>

namespace flexlay {

ObjectSelectorWidget::ObjectSelectorWidget(QWidget* parent)
  : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);
  setIconSize(QSize(32, 32));
  setSpacing(2);
  connect(this, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
    if (!item) {
      return;
    }
    const int idx = item->data(Qt::UserRole).toInt();
    if (idx >= 0 && idx < static_cast<int>(m_prototypes.size())) {
      emit prototype_activated(m_prototypes[static_cast<size_t>(idx)]);
    }
  });
}

void ObjectSelectorWidget::set_datadir(std::string datadir)
{
  m_datadir = std::move(datadir);
  if (!m_prototypes.empty()) {
    auto copy = m_prototypes;
    set_prototypes(std::move(copy));
  }
}

void ObjectSelectorWidget::set_prototypes(std::vector<ObjectPrototype> prototypes)
{
  m_prototypes = std::move(prototypes);
  clear();
  for (size_t i = 0; i < m_prototypes.size(); ++i) {
    const auto& proto = m_prototypes[i];
    auto* item = new QListWidgetItem(QString::fromStdString(proto.label), this);
    item->setData(Qt::UserRole, static_cast<int>(i));
    item->setToolTip(QString::fromStdString(proto.type_id));

    std::string img = proto.editor_image;
    if (img.empty()) {
      if (auto info = supertux::object_visual_info(proto.type_id)) {
        img = info->editor_image;
      }
    }
    bool got_icon = false;
    if (!m_datadir.empty() && !img.empty()) {
      if (auto spr = supertux::try_load_sprite(m_datadir, img)) {
        const auto& buf = spr->buffer();
        if (!buf.empty() && !buf.data().empty()) {
          QImage image(reinterpret_cast<const uchar*>(buf.data().data()),
                       buf.width(), buf.height(), buf.width() * 4,
                       QImage::Format_RGBA8888);
          QPixmap pm = QPixmap::fromImage(image.copy());
          if (!pm.isNull()) {
            item->setIcon(QIcon(pm.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            got_icon = true;
          }
        }
      }
    }
    if (!got_icon) {
      QPixmap pm(32, 32);
      pm.fill(QColor(proto.default_color.r, proto.default_color.g,
                     proto.default_color.b, proto.default_color.a));
      item->setIcon(QIcon(pm));
    }
  }
  if (!m_prototypes.empty()) {
    setCurrentRow(0);
  }
}

const ObjectPrototype* ObjectSelectorWidget::current_prototype() const
{
  const int row = currentRow();
  if (row < 0 || row >= static_cast<int>(m_prototypes.size())) {
    return nullptr;
  }
  return &m_prototypes[static_cast<size_t>(row)];
}

} // namespace flexlay
