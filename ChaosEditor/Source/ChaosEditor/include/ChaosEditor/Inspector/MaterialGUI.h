#pragma once
#include <QFrame>
#include <ChaosCore.h>

class QGridLayout;
class QComboBox;
class QLabel;

namespace ChaosEditor
{
    class MaterialGUI : public QFrame
    {
        Q_OBJECT
    public:
        MaterialGUI(Chaos::Resources::MaterialPtr p_material, QWidget* p_parent = nullptr);
        
        inline Chaos::Resources::MaterialPtr GetMaterial() { return m_material; }
    
    protected:
        void SetupFrame();

        void BindUniforms(std::map<std::string, Chaos::UniformValue>& p_uniformsMap, int& p_startRow);
        void BindSamplerUniforms(std::map<std::string, Chaos::UniformValueSampler>& p_uniformsMap, int& p_startRow);

    protected slots:
        void ShaderSelected(int p_shader);

        void UniformSelected(int p_uniform);
        void UniformSelected(double p_uniform);
        void UniformSelected(double p_x, double p_y);
        void UniformSelected(double p_x, double p_y, double p_z);
        void TextureUniformSelected(int p_texture);
        void SaveButtonPressed();
        void LoadButtonPressed();

    protected:
        Chaos::Resources::MaterialPtr m_material;

        QGridLayout*				m_layout        {nullptr};
        QComboBox*                  m_selectedShader{nullptr};

        std::unordered_map<std::string, std::pair<QWidget*, QWidget*>>  m_uniforms;
    };
}