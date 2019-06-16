#include "ChaosEditor/ComponentsGUI/MaterialComponentGUI.h"
#include <QFrame>
#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include "ChaosEditor/Inspector/DragSpinBox.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include "ChaosEditor/Inspector/Vector2Widget.h"

using namespace Chaos;

namespace ChaosEditor
{

    MaterialComponentGUI::MaterialComponentGUI(QWidget* p_parent) : 
        AComponentGUI("Material", p_parent),
		m_layout(new QGridLayout),
        m_selectedMaterial(new QComboBox)
    {
        m_selectedMaterial->addItem("No Material", (uintptr_t)(nullptr));
        const auto& materials = MaterialManager::Get().GetAllResources();
        for (const auto& pair : materials)
        {
            if (pair.second->GetGroup() == "RenderGroup"_sid)
                continue;
            m_selectedMaterial->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
        }
        QObject::connect(m_selectedMaterial,    SIGNAL(currentIndexChanged(int)),
                         this,                  SLOT(MaterialSelected(int)));

		m_layout->setSpacing(2);
        
		m_layout->addWidget(new QLabel("Material"),		0, 0, Qt::AlignLeft);
		m_layout->addWidget(m_selectedMaterial,			0, 1, Qt::AlignRight);

		m_layout->setColumnStretch(0, 1);
		m_layout->setColumnStretch(1, 2);
		m_layout->setRowMinimumHeight(0, 20);
    }

    void MaterialComponentGUI::SetupFrame()
    {
        CHAOS_ASSERT(m_component, "Drawing non bound ComponentGUI");

		int row = 1;
		for (std::pair<const std::string, std::pair<QLabel*, QWidget*>>& uniform : m_uniformWidgets)
		{
            if (row == 1 && !m_component->GetMaterial()->GetShader())
            {
                m_selectedShader->setCurrentIndex(0);
                return;
            }
            m_selectedShader->setCurrentText(m_component->GetMaterial()->GetShader()->GetName().string);

			m_layout->addWidget(uniform.second.first, row, 0, Qt::AlignLeft);
			m_layout->addWidget(uniform.second.second, row, 1, Qt::AlignRight);
			row++;
		}
        setMinimumHeight(row * 25);
    }

    void MaterialComponentGUI::BindComponent(IComponent* p_component)
    {
        MaterialComponent* materialComponent = dynamic_cast<MaterialComponent*>(p_component);
        CHAOS_ASSERT(materialComponent, "Wrong component type!");
        m_component = materialComponent;
        setLayout(m_layout);

		int row = 1;
		for (std::pair<const std::string, std::pair<QLabel*, QWidget*>>& uniform : m_uniformWidgets)
		{
			if (m_layout->itemAtPosition(row, 0))
			{
				delete m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(row, 0)))->widget();
				delete m_layout->takeAt(m_layout->indexOf(m_layout->itemAtPosition(row, 1)))->widget();
				m_layout->removeItem(m_layout->itemAtPosition(row, 0));
				m_layout->removeItem(m_layout->itemAtPosition(row, 1));
			}
            ++row;
		}
		m_uniformWidgets.clear();

        if (!m_component->GetMaterial())
        {
            m_selectedMaterial->setCurrentIndex(0);
            return;
        }
        m_selectedMaterial->setCurrentText(m_component->GetMaterial()->GetName().string);

        m_selectedShader = new QComboBox;
        m_selectedShader->addItem("No Shader", (uintptr_t)(nullptr));
        const auto& shaders = ShaderManager::Get().GetAllResources();
        for (const auto& pair : shaders)
        {
            if (pair.second->GetGroup() == "RenderGroup"_sid)
                continue;
            m_selectedShader->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
        }
        QObject::connect(m_selectedShader,  SIGNAL(currentIndexChanged(int)),
                         this,              SLOT(ShaderSelected(int)));

        m_uniformWidgets.insert_or_assign("Shader", std::make_pair<QLabel*, QWidget*>(new QLabel("Shader"), (QWidget*)m_selectedShader));

        row = 2;
        BindUniforms(materialComponent->m_material->m_uniforms, row);
        BindSamplerUniforms(materialComponent->m_material->m_textures, row);

		SetupFrame();
    }

    void MaterialComponentGUI::BindUniforms(std::map<std::string, UniformValue>& p_uniformsMap, int& p_startRow)
    {
        if (!m_component->m_material)
            return;

        for (std::pair<const std::string, Chaos::UniformValue>& uniform : p_uniformsMap)
        {
            switch (uniform.second.type)
            {
                case Chaos::E_ShaderType::SHADER_TYPE_BOOL:
                {

                    QCheckBox* boolCheckBox = new QCheckBox;
                    m_uniformWidgets.insert_or_assign(uniform.first, std::make_pair<QLabel*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)boolCheckBox));
                    boolCheckBox->setProperty("uniform", uniform.first.c_str());
                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                        boolCheckBox->setChecked(*(bool*)&uniform.second.value.asVoidPtr);

                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    QObject::connect(boolCheckBox,  SIGNAL(stateChanged(int)),
                                     this,          SLOT(UniformSelected(int)));
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_INT:
                {
                    DragLabel*      label           = new DragLabel(uniform.first.c_str());
                    DragSpinBox*    intSpinBox      = new DragSpinBox(label);

                    intSpinBox->setProperty("uniform", uniform.first.c_str());

                    m_uniformWidgets[uniform.first] = std::make_pair<QLabel*, QWidget*>((QLabel*)label, (QWidget*)intSpinBox);
                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                        intSpinBox->SetValue(*(int32_t*)&uniform.second.value.asVoidPtr);

                    m_layout->setRowMinimumHeight(p_startRow, 20);

                    QObject::connect(intSpinBox,    SIGNAL(ValueChanged(double)),
                                     this,          SLOT(UniformSelected(double)));
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
                {
                    DragLabel*      label           = new DragLabel(uniform.first.c_str());
                    DragSpinBox*    uintSpinBox     = new DragSpinBox(label);
                    uintSpinBox->setProperty("uniform", uniform.first.c_str());

                    m_uniformWidgets[uniform.first] = std::make_pair<QLabel*, QWidget*>((QLabel*)label, (QWidget*)uintSpinBox);
                
                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                        uintSpinBox->SetValue(*(uint32_t*)&uniform.second.value.asVoidPtr);

                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    QObject::connect(uintSpinBox,   SIGNAL(ValueChanged(double)),
                                     this,          SLOT(UniformSelected(double)));
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_FLOAT:
                {
                    DragLabel*      label           = new DragLabel(uniform.first.c_str());
                    DragSpinBox*    floatSpinBox    = new DragSpinBox(label);
                    floatSpinBox->setProperty("uniform", uniform.first.c_str());

                    m_uniformWidgets[uniform.first] = std::make_pair<QLabel*, QWidget*>((QLabel*)label, (QWidget*)floatSpinBox);
                
                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                        floatSpinBox->SetValue(*(float*)&uniform.second.value.asVoidPtr);

                    QObject::connect(floatSpinBox,  SIGNAL(ValueChanged(double)),
                                     this,          SLOT(UniformSelected(double)));
                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_VEC3:
                {
                    Vector3Widget* vec3Widget = new Vector3Widget;
                    vec3Widget->setProperty("uniform", uniform.first.c_str());
                    m_uniformWidgets[uniform.first] = std::make_pair<QLabel*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)vec3Widget);

                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                    {
                        Chaos::Maths::Vector3* value = (Chaos::Maths::Vector3*)uniform.second.value.asVoidPtr;
                        vec3Widget->SetValue(value->x, value->y, value->z);
                    }

                    QObject::connect(vec3Widget, SIGNAL(ValueChanged(double, double, double)),
                                     this,       SLOT(UniformSelected(double, double, double)));

                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_VEC2:
                {
                    Vector2Widget* vec2Widget = new Vector2Widget;
                    vec2Widget->setProperty("uniform", uniform.first.c_str());
                    m_uniformWidgets[uniform.first] = std::make_pair<QLabel*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)vec2Widget);

                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                    {
                        Chaos::Maths::Vector2* value = (Chaos::Maths::Vector2*)uniform.second.value.asVoidPtr;
                        vec2Widget->SetValue(value->x, value->y);
                    }

                    QObject::connect(vec2Widget, SIGNAL(ValueChanged(double, double)),
                                     this,       SLOT(UniformSelected(double, double)));

                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    break;
                }
            }
            ++p_startRow;
        }
    }

    void MaterialComponentGUI::BindSamplerUniforms(std::map<std::string, UniformValueSampler>& p_uniformsMap, int& p_startRow)
    {
        if (!m_component->m_material)
            return;

        for (std::pair<const std::string, UniformValueSampler>& uniform : p_uniformsMap)
        {
            switch (uniform.second.type)
            {
            case E_ShaderType::SHADER_TYPE_SAMPLER1D:
            case E_ShaderType::SHADER_TYPE_SAMPLER2D:
            case E_ShaderType::SHADER_TYPE_SAMPLER3D:
            {
                QComboBox* textureSelection = new QComboBox;
                m_uniformWidgets.insert_or_assign(uniform.first, std::make_pair<QLabel*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)textureSelection));
                textureSelection->setProperty("uniform", uniform.first.c_str());
                textureSelection->addItem("No Texture", (uintptr_t)(nullptr));
                const auto& textures = TextureManager::Get().GetAllResources();
                for (const auto& pair : textures)
                {
                    if (pair.second->GetGroup() == "RenderGroup"_sid)
                        continue;
                    textureSelection->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
                }
                QObject::connect(textureSelection, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(TextureUniformSelected(int)));

                m_layout->setRowMinimumHeight(p_startRow, 20);

                if (!m_component->m_material->m_textures.at(uniform.first).texture)
                {
                    textureSelection->setCurrentIndex(0);
                    continue;
                }
                else
                {
                    textureSelection->setCurrentText(uniform.second.texture->GetName().string);
                }
                break;
            }
            default:
                break;
            }
            ++p_startRow;
        }

    }


    void MaterialComponentGUI::UniformSelected(int p_uniform)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_component->m_material->m_uniforms[uniform].type)
        {
            case E_ShaderType::SHADER_TYPE_BOOL:
            {
                QCheckBox* widget = static_cast<QCheckBox*>(sender());
                bool widIsChecked = widget->isChecked();
                bool uniformIsChecked = *(bool*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr;
                if (uniformIsChecked != widIsChecked)
                {
                    m_component->m_material->SetUniform(uniform, widget->isChecked());
                    m_component->m_material->Save();
                }
                break;
            }
        }

    }

    void MaterialComponentGUI::UniformSelected(double p_uniform)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_component->m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_INT:
            case Chaos::E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
            {
                DragSpinBox* widget = static_cast<DragSpinBox*>(sender());
                if (*(int*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_component->m_material->SetUniform(uniform, (int)widget->GetValue());
                    m_component->m_material->Save();
                }
                break;
            }
            case Chaos::E_ShaderType::SHADER_TYPE_FLOAT:
            {
                DragSpinBox* widget = static_cast<DragSpinBox*>(sender());
                if (*(float*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_component->m_material->SetUniform(uniform, (float)widget->GetValue());
                    m_component->m_material->Save();
                }
                break;
            }
        }
    }

    void MaterialComponentGUI::UniformSelected(double p_x, double p_y)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_component->m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_VEC2:
            {
                Vector2Widget* widget = static_cast<Vector2Widget*>(sender());
                if (*(Chaos::Maths::Vector2*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_component->m_material->SetUniform(uniform, widget->GetValue());
                    m_component->m_material->Save();
                }
                break;
            }
        }
    }

    void MaterialComponentGUI::UniformSelected(double p_x, double p_y, double p_z)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_component->m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_VEC3:
            {
                Vector3Widget* widget = static_cast<Vector3Widget*>(sender());
                if (*(Chaos::Maths::Vector3*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_component->m_material->SetUniform(uniform, widget->GetValue());
                    m_component->m_material->Save();
                }
                break;
            }
            case Chaos::E_ShaderType::SHADER_TYPE_VEC2:
            {
                Vector2Widget* widget = static_cast<Vector2Widget*>(sender());
                if (*(Chaos::Maths::Vector2*)&m_component->m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_component->m_material->SetUniform(uniform, widget->GetValue());
                    m_component->m_material->Save();
                }
                break;
            }
        }
    }

    void MaterialComponentGUI::TextureUniformSelected(int p_texture)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        Resources::ATexture* texture = reinterpret_cast<Resources::ATexture*>(static_cast<uintptr_t>(static_cast<QComboBox*>(sender())->currentData().toLongLong()));
        if (!texture)
        {
            m_component->m_material->SetTexture(uniform, Resources::TexturePtr());
            m_component->m_material->Save();
        }
        else
        {
            m_component->m_material->SetTexture(uniform, TextureManager::Get().GetTexture(texture->GetName(), texture->GetGroup()));
            m_component->m_material->Save();
        }
    }

    void MaterialComponentGUI::MaterialSelected(int p_shader)
    {
        using namespace Chaos;
        Resources::Material* material = reinterpret_cast<Resources::Material*>(static_cast<uintptr_t>(m_selectedMaterial->currentData().toLongLong()));
        if (!material)
        {
            m_component->SetMaterial(nullptr);
            BindComponent(m_component);
        }
        else if (m_component->GetMaterial() && material == m_component->GetMaterial().get())
        {
            return;
        }
        else
        {
            m_component->SetMaterial(MaterialManager::Get().GetMaterial(material->GetName(), material->GetGroup()));
            BindComponent(m_component);
        }
    }
    void MaterialComponentGUI::ShaderSelected(int p_shader)
    {
        using namespace Chaos;
        Resources::AShader* shader = reinterpret_cast<Resources::AShader*>(static_cast<uintptr_t>(m_selectedShader->currentData().toLongLong()));
        if (!shader)
        {
            m_component->GetMaterial()->SetShader(nullptr);
            m_component->m_material->Save();
            BindComponent(m_component);
        }
        else if (m_component->GetMaterial()->GetShader() && shader == m_component->GetMaterial()->GetShader().get())
        {
            return;
        }
        else
        {
            m_component->GetMaterial()->SetShader(ShaderManager::Get().GetShader(shader->GetName(), shader->GetGroup()));
            m_component->m_material->Save();
            BindComponent(m_component);
        }
    }

}