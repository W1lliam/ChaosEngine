#pragma once

#include<ChaosEngine.h>
#include<QAbstractItemModel>
#include<QVariant>

class Hierarchy : public QAbstractItemModel, public Chaos::Events::EventListener
{
    Q_OBJECT
public:
    explicit Hierarchy(QObject* p_parent = nullptr);
    ~Hierarchy();

    QVariant data(const QModelIndex& p_index, int role) const override;
    QVariant headerData(int p_section, Qt::Orientation p_orientation, int p_role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& p_index) const override;

    QModelIndex index(int p_row, int p_column, const QModelIndex& p_parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& p_index) const override;

	Qt::DropActions supportedDropActions() const;

	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);

	QStringList mimeTypes() const;

	QMimeData* mimeData(const QModelIndexList & indexes) const;

    int rowCount	(const QModelIndex& p_parent = QModelIndex()) const override;
    int columnCount	(const QModelIndex& p_parent = QModelIndex()) const override;

protected:
	Chaos::ScenePtr m_scene;

private:
	bool OnSceneChanged(const Chaos::Events::SceneChangedEvent& e);
};

