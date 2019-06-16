#include "ChaosEditor/Hierarchy.h"
#include <QMimeData>


using namespace Chaos;

Hierarchy::Hierarchy(QObject* p_parent) : QAbstractItemModel(p_parent), m_scene(SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene())
{
	RegisterEventCallback<Chaos::Events::SceneChangedEvent>(std::bind(&Hierarchy::OnSceneChanged, this, std::placeholders::_1));
}

Hierarchy::~Hierarchy()
{
}

QVariant Hierarchy::data(const QModelIndex& p_index, int role) const
{
	if (!p_index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	Chaos::Scene::SceneNode* item = static_cast<Chaos::Scene::SceneNode*>(p_index.internalPointer());

	if (item->entity == m_scene->GetRoot().entity)
		return SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene()->GetName().string;


	return item->entity->GetEntityID().string;
}

QVariant Hierarchy::headerData(int p_section, Qt::Orientation p_orientation, int p_role) const
{
	return 42;
}

Qt::ItemFlags Hierarchy::flags(const QModelIndex & p_index) const
{
	if (!p_index.isValid())
		return 0;

	return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | QAbstractItemModel::flags(p_index);
}

QModelIndex Hierarchy::index(int p_row, int p_column, const QModelIndex & p_parent) const
{
	if (!hasIndex(p_row, p_column, p_parent))
		return QModelIndex();

	const Chaos::Scene::SceneNode* parentNode;

	
	parentNode = static_cast<Chaos::Scene::SceneNode*>(p_parent.internalPointer());

    Chaos::Scene::SceneNode* childNode;
	if (!p_parent.isValid())
		childNode = &const_cast<Chaos::Scene::SceneNode&>(m_scene->GetRoot());
	else
		childNode = parentNode->childs[p_row];

	if (childNode)
		return createIndex(p_row, p_column, childNode);
	else
		return QModelIndex();
}

QModelIndex Hierarchy::parent(const QModelIndex& p_index) const
{
	if (!p_index.isValid())
		return QModelIndex();

    Chaos::Scene::SceneNode* indexNode = static_cast<Chaos::Scene::SceneNode*>(p_index.internalPointer());

	if (indexNode->parent)
	{
		if (indexNode->parent == &m_scene->GetRoot())
		{
			return createIndex(0, 0, indexNode->parent);
		}
		std::vector<Chaos::Scene::SceneNode*>::iterator it = std::find(indexNode->parent->parent->childs.begin(), indexNode->parent->parent->childs.end(), indexNode->parent);
		return createIndex(std::distance(indexNode->parent->parent->childs.begin(), it), 0, indexNode->parent);
	}
	else
	{
		return QModelIndex();
	}
}

int Hierarchy::rowCount(const QModelIndex& p_parent) const
{
	const Chaos::Scene::SceneNode* parentNode;

	if (p_parent.column() > 0)
		return 0;

	if (!p_parent.isValid())
		return 1;

	else
		parentNode = static_cast<Chaos::Scene::SceneNode*>(p_parent.internalPointer());

	return (int)parentNode->childs.size();
}

int Hierarchy::columnCount(const QModelIndex & p_parent) const
{
	return 1;
}

bool Hierarchy::OnSceneChanged(const Chaos::Events::SceneChangedEvent & e)
{
	beginResetModel();
	m_scene = SystemManager::Get().GetSystem<SceneSystem>()->GetCurrentScene();
	endResetModel();
	return false;
}

Qt::DropActions Hierarchy::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

bool Hierarchy::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) 
{
	if (!canDropMimeData(data, action, row, column, parent))
		return false;

	if (action == Qt::IgnoreAction)
		return true;

	int beginRow;

	if (row != -1)
		beginRow = row;
	else if (parent.isValid())
		beginRow = rowCount(parent);
	else
		beginRow = rowCount(QModelIndex());


	QByteArray encodedData = data->data("SceneNode");
	QModelIndex* nodeIndex = (QModelIndex*)encodedData.toULongLong();
    Chaos::Scene::SceneNode* node = (Chaos::Scene::SceneNode*)nodeIndex->internalPointer();


	beginRemoveRows(nodeIndex->parent(), nodeIndex->row(), nodeIndex->row());
	beginInsertRows(parent, beginRow, beginRow);
	if (parent.parent().isValid())
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetParent(node->entity, ((Chaos::Scene::SceneNode*)parent.internalPointer())->entity);
	}
	else
	{
		Chaos::SystemManager::Get().GetSystem<Chaos::SceneSystem>()->GetCurrentScene()->SetParent(node->entity, m_scene->GetRoot().entity);
	}
	endInsertRows();
	endRemoveRows();

	return true;
}

QStringList Hierarchy::mimeTypes() const
{
	QStringList types;
	types << "SceneNode";
	return types;
}

QMimeData *Hierarchy::mimeData(const QModelIndexList &indexes) const
{
	// make sure there's exactly 1 index and it's valid
	if (indexes.size() != 1 || !indexes[0].isValid())
		return nullptr;

	// get the Node pointer from the index's internal pointer, convert it to quintptr, then to a UTF string
	quintptr nodeAddress = (quintptr)indexes[0].internalPointer();
	//Scene::SceneNode* node = static_cast<Scene::SceneNode*>(indexes[0].internalPointer());
	quintptr node = (quintptr)(&indexes[0]);
	//QByteArray encodedData(QString::number(nodeAddress).toUtf8());
	QByteArray encodedData(QString::number(node).toUtf8());

	// allocate mimeData, provide it with encodedData, and return it
	QMimeData *mimeData = new QMimeData();
	mimeData->setData("SceneNode", encodedData);
	return mimeData;
}

