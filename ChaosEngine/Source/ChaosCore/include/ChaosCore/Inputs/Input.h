#pragma once
#include "ChaosCore/chaos_core_pch.h"
#include "ChaosCore/Utils/Singleton.h"
#include <KeyCodes.h>

namespace Chaos
{
    class Input : public Singleton<Input>
    {
    public:

        using MousePos = std::pair<float, float>;

        virtual bool			IsKeyPressed(E_ChaosKeyCode keycode)			= 0;
        virtual bool			IsKeyReleased(E_ChaosKeyCode keycode)           = 0;
        virtual bool			IsMouseButtonPressed(E_ChaosMouseCode button)   = 0;
        virtual bool			IsMouseButtonReleased(E_ChaosMouseCode button)  = 0;
        virtual MousePos		GetMousePos()									= 0;
        virtual float			GetMouseX()										= 0;
        virtual float			GetMouseY()										= 0;

		virtual void			SetMousePosition(double p_x, double p_y)		= 0;
		virtual void			HideMouse(bool p_visible)						= 0;


        inline  static  ssq::Table      Expose(ssq::VM& p_vm)
        {
            ssq::Table inputTable = p_vm.addTable("Input");
            inputTable.addFunc("IsKeyPressed",              [&] (int keycode)        {   return Input::Get().IsKeyPressed((E_ChaosKeyCode)keycode);              });
            inputTable.addFunc("IsKeyReleased",             [&] (int keycode)        {   return Input::Get().IsKeyReleased((E_ChaosKeyCode)keycode);             });
            inputTable.addFunc("IsMouseButtonPressed",      [&] (int keycode)        {   return Input::Get().IsMouseButtonPressed((E_ChaosMouseCode)keycode);    });
            inputTable.addFunc("IsMouseButtonReleased",     [&] (int keycode)        {   return Input::Get().IsMouseButtonReleased((E_ChaosMouseCode)keycode);   });
            inputTable.addFunc("GetMousePos",               [&] ()                   {   return Input::Get().GetMousePos();                                      });
            inputTable.addFunc("GetMouseX",                 [&] ()                   {   return Input::Get().GetMouseX();                                        });
            inputTable.addFunc("GetMouseY",                 [&] ()                   {   return Input::Get().GetMouseY();                                        });
            inputTable.addFunc("SetMousePosition",          [&] (float x, float y)   {   Input::Get().SetMousePosition(x,y);                                     });
            inputTable.addFunc("HideMouse",                 [&] (bool p_visible)     {   Input::Get().HideMouse(p_visible);                                      });

            return inputTable;
        }
    };
}