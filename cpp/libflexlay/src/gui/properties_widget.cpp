// Flexlay - A Generic 2D Game Editor
// Copyright (C) 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "flexlay/gui/properties_widget.hpp"

#include "flexlay/commands/object_property_command.hpp"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

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

void PropertiesWidget::commit(CommandPtr cmd)
{
  if (!cmd) {
    return;
  }
  // ObjectPropertyCommand is value type from factories — wrap if needed
  if (m_commit) {
    m_commit(std::move(cmd));
  } else {
    cmd->execute();
  }
  emit properties_changed();
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
    const std::string old_n = m_object->name();
    const std::string new_n = m_name_edit->text().toStdString();
    if (old_n == new_n) {
      return;
    }
    auto cmd = std::make_shared<ObjectPropertyCommand>(
      ObjectPropertyCommand::set_name(m_object, old_n, new_n));
    if (!cmd->changed()) {
      return;
    }
    commit(cmd);
  });

  {
    const Pointf pos = m_object->get_pos();
    auto* x_spin = new QDoubleSpinBox(this);
    x_spin->setRange(-1e6, 1e6);
    x_spin->setDecimals(2);
    x_spin->setKeyboardTracking(false);
    x_spin->setValue(pos.x);
    auto* y_spin = new QDoubleSpinBox(this);
    y_spin->setRange(-1e6, 1e6);
    y_spin->setDecimals(2);
    y_spin->setKeyboardTracking(false);
    y_spin->setValue(pos.y);
    m_form->addRow(QStringLiteral("x"), x_spin);
    m_form->addRow(QStringLiteral("y"), y_spin);

    auto apply_pos = [this, x_spin, y_spin]() {
      if (m_block || !m_object) {
        return;
      }
      const Pointf old_p = m_object->get_pos();
      const Pointf new_p(static_cast<float>(x_spin->value()),
                         static_cast<float>(y_spin->value()));
      if (old_p.x == new_p.x && old_p.y == new_p.y) {
        return;
      }
      auto cmd = std::make_shared<ObjectPropertyCommand>(
        ObjectPropertyCommand::set_pos(m_object, old_p, new_p));
      commit(cmd);
    };
    connect(x_spin, &QDoubleSpinBox::editingFinished, this, apply_pos);
    connect(y_spin, &QDoubleSpinBox::editingFinished, this, apply_pos);
  }

  for (const auto& kv : m_object->properties().values()) {
    const QString key = QString::fromStdString(kv.first);
    const PropertyValue& val = kv.second;
    const std::string key_s = kv.first;

    auto commit_prop = [this, key_s](PropertyValue new_v) {
      if (m_block || !m_object) {
        return;
      }
      auto old_v = m_object->properties().get(key_s);
      if (old_v && *old_v == new_v) {
        return;
      }
      auto cmd = std::make_shared<ObjectPropertyCommand>(
        ObjectPropertyCommand::set_property(m_object, key_s, old_v, std::move(new_v)));
      commit(cmd);
    };

    if (const auto* b = std::get_if<bool>(&val)) {
      auto* box = new QCheckBox(this);
      box->setChecked(*b);
      m_form->addRow(key, box);
      connect(box, &QCheckBox::toggled, this, [commit_prop](bool checked) {
        commit_prop(checked);
      });
    } else if (const auto* i = std::get_if<int>(&val)) {
      auto* spin = new QSpinBox(this);
      spin->setRange(-1000000, 1000000);
      spin->setKeyboardTracking(false);
      spin->setValue(*i);
      m_form->addRow(key, spin);
      connect(spin, &QSpinBox::editingFinished, this, [spin, commit_prop]() {
        commit_prop(spin->value());
      });
    } else if (const auto* f = std::get_if<float>(&val)) {
      auto* spin = new QDoubleSpinBox(this);
      spin->setRange(-1e6, 1e6);
      spin->setDecimals(3);
      spin->setKeyboardTracking(false);
      spin->setValue(*f);
      m_form->addRow(key, spin);
      connect(spin, &QDoubleSpinBox::editingFinished, this, [spin, commit_prop]() {
        commit_prop(static_cast<float>(spin->value()));
      });
    } else {
      auto* edit = new QLineEdit(value_to_string(val), this);
      m_form->addRow(key, edit);
      const PropertyType kind = property_type_of(val);
      connect(edit, &QLineEdit::editingFinished, this, [edit, kind, commit_prop]() {
        const QString text = edit->text();
        if (kind == PropertyType::String) {
          commit_prop(text.toStdString());
        } else if (kind == PropertyType::Color) {
          const auto parts = text.split(QLatin1Char(','));
          if (parts.size() >= 3) {
            Color c(parts[0].toInt(), parts[1].toInt(), parts[2].toInt(),
                    parts.size() >= 4 ? parts[3].toInt() : 255);
            commit_prop(c);
          }
        } else if (kind == PropertyType::Point) {
          const auto parts = text.split(QLatin1Char(','));
          if (parts.size() >= 2) {
            commit_prop(Pointf(parts[0].toFloat(), parts[1].toFloat()));
          }
        }
      });
    }
  }

  m_block = false;
}

} // namespace flexlay
