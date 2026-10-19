// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/properties_widget.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

#include <sstream>

namespace flexlay {
namespace {

QString value_to_string(const PropertyValue& v)
{
  if (const auto* b = std::get_if<bool>(&v)) {
    return *b ? QStringLiteral("true") : QStringLiteral("false");
  }
  if (const auto* i = std::get_if<int>(&v)) {
    return QString::number(*i);
  }
  if (const auto* f = std::get_if<float>(&v)) {
    return QString::number(static_cast<double>(*f));
  }
  if (const auto* s = std::get_if<std::string>(&v)) {
    return QString::fromStdString(*s);
  }
  if (const auto* c = std::get_if<Color>(&v)) {
    return QStringLiteral("%1,%2,%3,%4").arg(c->r).arg(c->g).arg(c->b).arg(c->a);
  }
  if (const auto* p = std::get_if<Pointf>(&v)) {
    return QStringLiteral("%1,%2").arg(static_cast<double>(p->x)).arg(static_cast<double>(p->y));
  }
  return {};
}

} // namespace

PropertiesWidget::PropertiesWidget(QWidget* parent)
  : QWidget(parent)
{
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(4, 4, 4, 4);
  m_empty_label = new QLabel(QStringLiteral("No object selected"), this);
  m_empty_label->setAlignment(Qt::AlignCenter);
  root->addWidget(m_empty_label);
  m_form = new QFormLayout();
  root->addLayout(m_form);
  root->addStretch(1);
}

void PropertiesWidget::clear()
{
  m_object.reset();
  rebuild_form();
}

void PropertiesWidget::set_object(std::shared_ptr<ObjMapObject> obj)
{
  m_object = std::move(obj);
  rebuild_form();
}

void PropertiesWidget::refresh()
{
  rebuild_form();
}

void PropertiesWidget::rebuild_form()
{
  m_block = true;
  while (m_form->rowCount() > 0) {
    m_form->removeRow(0);
  }
  m_name_edit = nullptr;

  if (!m_object) {
    m_empty_label->show();
    m_block = false;
    return;
  }
  m_empty_label->hide();

  m_name_edit = new QLineEdit(QString::fromStdString(m_object->name()), this);
  m_form->addRow(QStringLiteral("name"), m_name_edit);
  connect(m_name_edit, &QLineEdit::editingFinished, this, [this]() {
    if (m_block || !m_object || !m_name_edit) {
      return;
    }
    m_object->set_name(m_name_edit->text().toStdString());
    emit properties_changed();
  });

  // Position (read/write via object API, not bag)
  {
    const Pointf pos = m_object->get_pos();
    auto* x_spin = new QDoubleSpinBox(this);
    x_spin->setRange(-1e6, 1e6);
    x_spin->setDecimals(2);
    x_spin->setValue(pos.x);
    auto* y_spin = new QDoubleSpinBox(this);
    y_spin->setRange(-1e6, 1e6);
    y_spin->setDecimals(2);
    y_spin->setValue(pos.y);
    m_form->addRow(QStringLiteral("x"), x_spin);
    m_form->addRow(QStringLiteral("y"), y_spin);
    auto apply_pos = [this, x_spin, y_spin]() {
      if (m_block || !m_object) {
        return;
      }
      m_object->set_pos(Pointf(static_cast<float>(x_spin->value()),
                               static_cast<float>(y_spin->value())));
      emit properties_changed();
    };
    connect(x_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, apply_pos);
    connect(y_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, apply_pos);
  }

  for (const auto& kv : m_object->properties().values()) {
    const QString key = QString::fromStdString(kv.first);
    const PropertyValue& val = kv.second;

    if (const auto* b = std::get_if<bool>(&val)) {
      auto* box = new QCheckBox(this);
      box->setChecked(*b);
      m_form->addRow(key, box);
      connect(box, &QCheckBox::toggled, this, [this, key = kv.first](bool checked) {
        if (m_block || !m_object) {
          return;
        }
        m_object->properties().set(key, checked);
        emit properties_changed();
      });
    } else if (const auto* i = std::get_if<int>(&val)) {
      auto* spin = new QSpinBox(this);
      spin->setRange(-1000000, 1000000);
      spin->setValue(*i);
      m_form->addRow(key, spin);
      connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this,
              [this, key = kv.first](int v) {
                if (m_block || !m_object) {
                  return;
                }
                m_object->properties().set(key, v);
                emit properties_changed();
              });
    } else if (const auto* f = std::get_if<float>(&val)) {
      auto* spin = new QDoubleSpinBox(this);
      spin->setRange(-1e6, 1e6);
      spin->setDecimals(3);
      spin->setValue(*f);
      m_form->addRow(key, spin);
      connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
              [this, key = kv.first](double v) {
                if (m_block || !m_object) {
                  return;
                }
                m_object->properties().set(key, static_cast<float>(v));
                emit properties_changed();
              });
    } else {
      // string, color, point — text field
      auto* edit = new QLineEdit(value_to_string(val), this);
      m_form->addRow(key, edit);
      connect(edit, &QLineEdit::editingFinished, this, [this, key = kv.first, edit, kind = property_type_of(val)]() {
        if (m_block || !m_object) {
          return;
        }
        const QString text = edit->text();
        if (kind == PropertyType::String) {
          m_object->properties().set(key, text.toStdString());
        } else if (kind == PropertyType::Color) {
          const auto parts = text.split(QLatin1Char(','));
          if (parts.size() >= 3) {
            Color c(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(),
                    parts.size() >= 4 ? parts[3].toInt() : 255);
            m_object->properties().set(key, c);
          }
        } else if (kind == PropertyType::Point) {
          const auto parts = text.split(QLatin1Char(','));
          if (parts.size() >= 2) {
            m_object->properties().set(key, Pointf(parts[0].toFloat(), parts[1].toFloat()));
          }
        }
        emit properties_changed();
      });
    }
  }

  m_block = false;
}

} // namespace flexlay
