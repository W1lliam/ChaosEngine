#pragma once

namespace Ui {
    class Editor;
}


namespace ChaosEditor
{
    class Inspector;

    class MenuBar
    {
    public:
        MenuBar(Ui::Editor* p_ui);
        static inline void BindInspector(Inspector* p_inspector) { s_inspector = p_inspector; };
    protected:
        static void CreateNewScene();
        static void SaveScene();
        static void LoadScene();
        static void CreateEmptyGameObject();
        static void CreateMaterial();
        static void GenerateBRDF();
        static void ImportAsset();
    public:
        inline static Inspector*    s_inspector {nullptr};
        inline static Ui::Editor*   s_ui        {nullptr};
    };
}