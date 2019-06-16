#pragma once
#include <unordered_map>
#include <ChaosEngine.h>
#include "ChaosEditor/ComponentsGUI/AComponentGUI.h"
#include <QLabel>
#include <QLineEdit>

class QGridLayout;
class QComboBox;

namespace ChaosEditor
{
    class MaterialComponentGUI : public AComponentGUI
    {
        Q_OBJECT
    public:
        MaterialComponentGUI(QWidget* p_parent = nullptr);
        ~MaterialComponentGUI() = default;

        virtual void SetupFrame() override;
        virtual void BindComponent(Chaos::IComponent* p_component)	override;

        void BindUniforms(std::map<std::string, Chaos::UniformValue>& p_uniformsMap, int& p_startRow);
        void BindSamplerUniforms(std::map<std::string, Chaos::UniformValueSampler>& p_uniformsMap, int& p_startRow);

    protected slots:
        void UniformSelected(int p_uniform);
        void UniformSelected(double p_uniform);
        void UniformSelected(double p_x, double p_y);
        void UniformSelected(double p_x, double p_y, double p_z);
        void TextureUniformSelected(int p_texture);
        void MaterialSelected(int p_material);
        void ShaderSelected(int p_shader);

    protected:
        Chaos::MaterialComponent*   m_component         {nullptr};
		QGridLayout*				m_layout		    {nullptr};
        QComboBox*                  m_selectedMaterial  {nullptr};
        QComboBox*                  m_selectedShader    {nullptr};

		std::unordered_map<std::string, std::pair<QLabel*, QWidget*>>  m_uniformWidgets;
    };
}