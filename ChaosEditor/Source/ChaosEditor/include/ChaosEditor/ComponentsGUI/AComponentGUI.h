#pragma once
#include <string>
#include <QFrame>
#include "ChaosCore.h"

namespace Chaos
{
    class IComponent;
}

class QTreeWidget;
class QTreeWidgetItem;

namespace ChaosEditor
{
    class Category;

    class AComponentGUI : public QFrame
    {
        Q_OBJECT
    public:
        AComponentGUI(const std::string& p_name, QWidget* p_parent = nullptr);

        virtual void AddToTree(QTreeWidget* p_container, Chaos::IComponent* p_component);
        virtual void RemoveFromTree(QTreeWidget* p_container);
        virtual void BindComponent(Chaos::IComponent* p_component) = 0;

        inline const std::string& GetName() const { return m_name; }
        virtual void SetupFrame() = 0;

		inline Category* GetCategory() const { if (m_category) return m_category; return nullptr; }

    protected:

        std::string         m_name; 
        QTreeWidgetItem*    m_titleContainer    {nullptr};
        QTreeWidgetItem*    m_contentContainer  {nullptr};
        Category*           m_category          {nullptr};

    };
}