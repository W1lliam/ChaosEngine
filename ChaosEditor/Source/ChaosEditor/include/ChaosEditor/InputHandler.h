#pragma once
#include <ChaosCore.h>
#include <map>
#include <QKeyEvent>

class InputHandler : public Chaos::Input
{
public:
	InputHandler()  = default;
	~InputHandler() = default;

	static std::map<Qt::Key, bool>          keyInput;
	static std::map<Qt::MouseButton, bool>  mouseInput;

    static  void            Init();

    static E_ChaosKeyCode     QtToChaosKeyCode(Qt::Key p_code);
    static Qt::Key            ChaosToQtKeyCode(E_ChaosKeyCode p_code);

    static E_ChaosMouseCode   QtToChaosMouseCode(Qt::MouseButton p_code);
    static Qt::MouseButton    ChaosToQtMouseCode(E_ChaosMouseCode p_code);

protected:
    virtual bool			IsKeyPressed(E_ChaosKeyCode keycode)            override;
    virtual bool			IsKeyReleased(E_ChaosKeyCode keycode)           override;
    virtual bool			IsMouseButtonPressed(E_ChaosMouseCode button)   override;
    virtual bool			IsMouseButtonReleased(E_ChaosMouseCode button)  override;
    virtual MousePos		GetMousePos()                                   override;
    virtual float			GetMouseX()                                     override;
    virtual float			GetMouseY()                                     override;

    virtual void			SetMousePosition(double p_x, double p_y)	    override;
    virtual void			HideMouse(bool p_visible)					    override;

    static const std::unordered_map<E_ChaosKeyCode, Qt::Key>            s_keyCodeMap;
    static const std::unordered_map<E_ChaosMouseCode, Qt::MouseButton>  s_mouseCodeMap;

};