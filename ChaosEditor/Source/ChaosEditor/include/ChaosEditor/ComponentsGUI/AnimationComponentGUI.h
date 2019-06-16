#pragma once
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include <QLabel>


class QComboBox;
class QCheckBox;
namespace ChaosEditor
{
	class AnimationComponentGUI : public AComponentGUI
	{
		Q_OBJECT
	public:
		AnimationComponentGUI(QWidget* p_parent = nullptr);
		~AnimationComponentGUI() = default;

		virtual void SetupFrame() override;
		virtual void BindComponent(Chaos::IComponent* p_component) override;

	public slots:
		void AnimationSelected(int p_index);

		void StartAnimation(bool p_triggerEnabled);
		void PauseAnimation(bool p_triggerEnabled);
		void StopAnimation(bool p_triggerEnabled);

	protected:
		Chaos::AnimationComponent*   m_component{ nullptr };
		QComboBox*					 m_selectedAnimation{ nullptr };
		QCheckBox*					 m_started{ nullptr };
		QCheckBox*					 m_paused{ nullptr };
		QCheckBox*					 m_stopped{ nullptr };
	};
}