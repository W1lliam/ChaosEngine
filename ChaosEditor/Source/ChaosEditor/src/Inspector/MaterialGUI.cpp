#include "ChaosEditor/Inspector/MaterialGUI.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QSizePolicy>
#include <QCheckBox>
#include <QPushButton>
#include "ChaosEditor/Inspector/DragSpinBox.h"
#include "ChaosEditor/Inspector/Vector3Widget.h"
#include "ChaosEditor/Inspector/Vector2Widget.h"

namespace ChaosEditor
{
    MaterialGUI::MaterialGUI(Chaos::Resources::MaterialPtr p_material, QWidget* p_parent) :
        QFrame(p_parent),
        m_material(p_material),
        m_layout(new QGridLayout),
        m_selectedShader(new QComboBox)
    {
        using namespace Chaos;
        setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
        m_selectedShader->addItem("No Shader", (uintptr_t)(nullptr));
        const auto& shaders = Chaos::ShaderManager::Get().GetAllResources();
        for (const auto& pair : shaders)
        {
            if (pair.second->GetGroup() == "RenderGroup"_sid)
                continue;
            m_selectedShader->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
        }
        QObject::connect(m_selectedShader,  SIGNAL(currentIndexChanged(int)),
                         this,              SLOT(ShaderSelected(int)));

        m_layout->setSpacing(2);
        m_layout->addWidget(new QLabel("Shader"),   0, 0, Qt::AlignLeft);
        m_layout->addWidget(m_selectedShader,       0, 1, Qt::AlignRight);

        m_layout->setColumnStretch(0, 1);
        m_layout->setColumnStretch(1, 2);
        m_layout->setRowMinimumHeight(0, 20);

        setLayout(m_layout);

        SetupFrame();
    }


    void MaterialGUI::SetupFrame()
    {
        int row = 1;
        for (std::pair<const std::string, std::pair<QWidget*, QWidget*>>& uniform : m_uniforms)
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
        m_uniforms.clear();

        if (!m_material->GetShader())
        {
            m_selectedShader->setCurrentIndex(0);
            return;
        }
        m_selectedShader->setCurrentText(m_material->GetShader()->GetName().string);

        row = 1;
        BindUniforms(m_material->m_uniforms, row);
        BindSamplerUniforms(m_material->m_textures, row);


        QPushButton* saveButton = new QPushButton("Save");
        QObject::connect(saveButton,    SIGNAL(pressed()),
                         this,          SLOT(SaveButtonPressed()));


        m_uniforms.emplace_hint(m_uniforms.end(),"zzButtons", std::make_pair<QWidget*, QWidget*>((QWidget*)saveButton, nullptr));

        row = 1;
        for (std::pair<const std::string, std::pair<QWidget*, QWidget*>>& uniform : m_uniforms)
        {
            m_layout->addWidget(uniform.second.first,   row, 0, Qt::AlignLeft);
            m_layout->addWidget(uniform.second.second,  row, 1, Qt::AlignRight);
            row++;
        }
        setMinimumHeight(row * 25);
    }

    void MaterialGUI::BindUniforms(std::map<std::string, Chaos::UniformValue>& p_uniformsMap, int & p_startRow)
    {
        for (std::pair<const std::string, Chaos::UniformValue>& uniform : p_uniformsMap)
        {
            switch (uniform.second.type)
            {
                case Chaos::E_ShaderType::SHADER_TYPE_BOOL:
                {

                    QCheckBox* boolCheckBox = new QCheckBox;
                    m_uniforms.insert_or_assign(uniform.first, std::make_pair<QWidget*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)boolCheckBox));
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

                    m_uniforms[uniform.first]       = std::make_pair<QWidget*, QWidget*>((QLabel*)label, (QWidget*)intSpinBox);
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

                    m_uniforms[uniform.first]       = std::make_pair<QWidget*, QWidget*>((QLabel*)label, (QWidget*)uintSpinBox);
                
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

                    m_uniforms[uniform.first]       = std::make_pair<QWidget*, QWidget*>((QLabel*)label, (QWidget*)floatSpinBox);
                
                    if ((int64_t)uniform.second.value.asVoidPtr != -1)
                        floatSpinBox->SetValue(*(float*)&uniform.second.value.asVoidPtr);

                    m_layout->setRowMinimumHeight(p_startRow, 20);
                    QObject::connect(floatSpinBox,  SIGNAL(ValueChanged(double)),
                                     this,          SLOT(UniformSelected(double)));
                    break;
                }
                case Chaos::E_ShaderType::SHADER_TYPE_VEC3:
                {
                    Vector3Widget* vec3Widget = new Vector3Widget;
                    vec3Widget->setProperty("uniform", uniform.first.c_str());
                    m_uniforms[uniform.first] = std::make_pair<QWidget*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)vec3Widget);

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
                    m_uniforms[uniform.first] = std::make_pair<QWidget*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)vec2Widget);

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
    void MaterialGUI::BindSamplerUniforms(std::map<std::string, Chaos::UniformValueSampler>& p_uniformsMap, int & p_startRow)
    {
        for (std::pair<const std::string, Chaos::UniformValueSampler>& uniform : p_uniformsMap)
        {
            switch (uniform.second.type)
            {
            case Chaos::E_ShaderType::SHADER_TYPE_SAMPLER1D:
            case Chaos::E_ShaderType::SHADER_TYPE_SAMPLER2D:
            case Chaos::E_ShaderType::SHADER_TYPE_SAMPLER3D:
            {
                QComboBox* textureSelection = new QComboBox;
                m_uniforms.insert_or_assign(uniform.first, std::make_pair<QWidget*, QWidget*>(new QLabel(uniform.first.c_str()), (QWidget*)textureSelection));
                textureSelection->setProperty("uniform", uniform.first.c_str());
                textureSelection->addItem("No Texture", (uintptr_t)(nullptr));
                const auto& textures = Chaos::TextureManager::Get().GetAllResources();
                for (const auto& pair : textures)
                {
                    using namespace Chaos;
                    if (pair.second->GetGroup() == "RenderGroup"_sid)
                        continue;
                    textureSelection->addItem(pair.second->GetName().string, (uintptr_t)(pair.second.get()));
                }
                QObject::connect(textureSelection, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(TextureUniformSelected(int)));

                m_layout->setRowMinimumHeight(p_startRow, 20);

                if (!m_material->m_textures.at(uniform.first).texture)
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
        }
    }

    void MaterialGUI::UniformSelected(int p_uniform)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_material->m_uniforms[uniform].type)
        {
            case E_ShaderType::SHADER_TYPE_BOOL:
            {
                QCheckBox* widget = static_cast<QCheckBox*>(sender());
                bool widIsChecked = widget->isChecked();
                bool uniformIsChecked = *(bool*)&m_material->m_uniforms[uniform].value.asVoidPtr;
                if (uniformIsChecked != widIsChecked)
                {
                    m_material->SetUniform(uniform, widget->isChecked());
                }
                break;
            }
        }
       
    }

    void MaterialGUI::UniformSelected(double p_uniform)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_INT:
            case Chaos::E_ShaderType::SHADER_TYPE_UNSIGNED_INT:
            {
                DragSpinBox* widget = static_cast<DragSpinBox*>(sender());
                if (*(int*)&m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_material->SetUniform(uniform, (int)widget->GetValue());
                }
                break;
            }
            case Chaos::E_ShaderType::SHADER_TYPE_FLOAT:
            {
                DragSpinBox* widget = static_cast<DragSpinBox*>(sender());
                if (*(float*)&m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_material->SetUniform(uniform, (float)widget->GetValue());
                }
                break;
            }
        }
    }

    void MaterialGUI::UniformSelected(double p_x, double p_y)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_VEC2:
            {
                Vector2Widget* widget = static_cast<Vector2Widget*>(sender());
                if (*(Chaos::Maths::Vector2*)&m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_material->SetUniform(uniform, widget->GetValue());
                }
                break;
            }
        }
    }

    void MaterialGUI::UniformSelected(double p_x, double p_y, double p_z)
    {
        using namespace Chaos;
        std::string uniform = sender()->property("uniform").toString().toStdString();
        switch (m_material->m_uniforms[uniform].type)
        {
            case Chaos::E_ShaderType::SHADER_TYPE_VEC3:
            {
                Vector3Widget* widget = static_cast<Vector3Widget*>(sender());
                if (*(Chaos::Maths::Vector3*)&m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_material->SetUniform(uniform, widget->GetValue());
                }
                break;
            }
            case Chaos::E_ShaderType::SHADER_TYPE_VEC2:
            {
                Vector2Widget* widget = static_cast<Vector2Widget*>(sender());
                if (*(Chaos::Maths::Vector2*)&m_material->m_uniforms[uniform].value.asVoidPtr != widget->GetValue())
                {
                    m_material->SetUniform(uniform, widget->GetValue());
                }
                break;
            }
        }
    }

    void MaterialGUI::TextureUniformSelected(int p_texture)
    {
        using namespace Chaos;

        std::string uniform = sender()->property("uniform").toString().toStdString();
        Resources::ATexture* texture = reinterpret_cast<Resources::ATexture*>(static_cast<uintptr_t>(static_cast<QComboBox*>(sender())->currentData().toLongLong()));
        if (!texture)
        {
            m_material->SetTexture(uniform, Resources::TexturePtr());
        }
        else
        {
            m_material->SetTexture(uniform, TextureManager::Get().GetTexture(texture->GetName(), texture->GetGroup()));
        }
    }

    void MaterialGUI::ShaderSelected(int p_shader)
    {
        using namespace Chaos;
        Resources::AShader* shader = reinterpret_cast<Resources::AShader*>(static_cast<uintptr_t>(m_selectedShader->currentData().toLongLong()));
        if (!shader)
        {
            m_material->SetShader(nullptr);
            SetupFrame();
        }
        else if (m_material->GetShader() && shader == m_material->GetShader().get())
        {
            return;
        }
        else
        {
            m_material->SetShader(ShaderManager::Get().GetShader(shader->GetName(), shader->GetGroup()));
            SetupFrame();
        }
    }

    void MaterialGUI::SaveButtonPressed()
    {
        m_material->Save();
        std::string name(m_material->m_name.string);
        if (filesystem::exists("Assets/DefaultResources/Editor.resdecl"))
        {
            std::ofstream out("Assets/DefaultResources/Editor.resdecl", std::ios::out | std::ios::app);
            if (out.is_open())
                out << "\n" + name + ",MATERIAL,Editor";
            out.close();
        }
    }

    void MaterialGUI::LoadButtonPressed()
    {
        using namespace Chaos;
        ResourceGroupManager::Get().ClearResourceGroup("Editor"_sid);
        ResourceGroupManager::Get().LoadResourceDeclaration("Assets/DefaultResources/Editor.resdecl");
        ResourceGroupManager::Get().InitializeResourceGroup("Editor"_sid);
        ResourceGroupManager::Get().LoadResourceGroup("Editor"_sid);
        SetupFrame();
    }
}
