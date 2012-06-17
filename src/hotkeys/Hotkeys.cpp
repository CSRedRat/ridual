#include "Hotkeys.h"
#include "Hotkey_editor.h"

Hotkeys::Hotkeys(QWidget *parent) :
  QAbstractTableModel(parent),
  parent_widget(parent)
{
  group_name = "hotkeys";
}

Hotkeys::~Hotkeys() {
  foreach (Hotkey* h, hotkeys) {
    delete h;
  }
}

void Hotkeys::add(QString name,
                         QString default_value,
                         QObject* receiver,
                         const char* slot) {
  QSettings settings;
  settings.beginGroup(group_name);

  hotkeys << new Hotkey(name, settings.value(name, default_value).toString(), parent_widget);
  connect(hotkeys.last()->get_shortcut(), SIGNAL(activated()), receiver, slot);

  settings.endGroup();
}

void Hotkeys::set_group_name(QString n) {
  group_name = n;
}

int Hotkeys::columnCount(const QModelIndex &) const {
  return 2;
}

int Hotkeys::rowCount(const QModelIndex &) const {
  return hotkeys.count();
}

QVariant Hotkeys::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (index.column() == column_name) {
      return hotkeys[index.row()]->get_translated_name();
    } else if (index.column() == column_value) {
      return hotkeys[index.row()]->editor_value;
    }
  }
//  return QString("undefined %1").arg(role);
  return QVariant();
}

Qt::ItemFlags Hotkeys::flags(const QModelIndex &index) const {
  if (index.column() == column_value) {
    return Qt::ItemIsEnabled | Qt::ItemIsEditable;
  }
  return Qt::ItemIsEnabled;
}

bool Hotkeys::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::EditRole && index.column() == column_value) {
    hotkeys[index.row()]->editor_value = value.toString();
    emit dataChanged(index, index);
  }
  return false;
}

QVariant Hotkeys::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return section == 0? tr("Action"): tr("Hotkey");
  }
  return QVariant();
}

void Hotkeys::save() {
  QSettings settings;
  settings.beginGroup(group_name);
  foreach (Hotkey* h, hotkeys) {
    h->set_value(h->editor_value);
    settings.setValue(h->get_name(), h->editor_value);
  }
  settings.endGroup();
}

void Hotkeys::open_editor() {
  foreach (Hotkey* h, hotkeys) {
    h->editor_value = h->get_value();
  }
  Hotkey_editor* editor = new Hotkey_editor(this);
  editor->show();
}
