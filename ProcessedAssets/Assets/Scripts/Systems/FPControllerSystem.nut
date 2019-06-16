class FPControllerSystem
{
    static typeName = "FPControllerSystem";
	constructor()
	{
        Log.Info("FPControllerSystem created!");
	}
    playerComp = null;
    player = null;
    mouseStartPosX = 0.0;
    mouseStartPosY = 0.0;
    captureMouse = true;
    rotY =-90.0;


    function Start()
    {
        FetchPlayer();
        // if(playerComp != null && player != null && player.GetRigidBody() == null)
        // {
        //     player.GetTransform().SetPosition(0.0,20.0,0.0);

        //     player.AddSphereCollider(Vector3(5.0, 0.8, 0.0), Vector3(0.0, 0.0, 0.0), 1.2);
        //     local playerRB = player.AddRigidBody(RigidBodyType.DYNAMIC, 50.0, Vector3(0.0, 0.0, 0.0));
        //     playerRB.SetXRotationConstraint(true);
        //     playerRB.SetZRotationConstraint(true);
        //     playerRB.SetGravity(false);
        // }
        Input.HideMouse(true);
        Input.SetMousePosition(Renderer.GetScreenWidth() * 0.5, Renderer.GetScreenHeight() *0.5);
        mouseStartPosX = Input.GetMouseX();
        mouseStartPosY = Input.GetMouseY();
    }

    function Stop()
    {
        Input.HideMouse(false);
        captureMouse = false;
    }

    function Update(p_deltaTime)
    {
        if(playerComp != null && player != null)
        {
            UpdateMouseInput(p_deltaTime);
            UpdateKeyInput(p_deltaTime);
        }
        else
        {
            FetchPlayer();
        }
    }

    function UpdateMouseInput(p_deltaTime)
    {
        if(captureMouse)
        {
            local xOffset = Input.GetMouseX() - mouseStartPosX;
            rotY += xOffset * 10.0 * p_deltaTime;
            rotY %= 360.0;
            player.GetTransform().SetOrientation(0.0, rotY, 0.0);
            Input.SetMousePosition(mouseStartPosX, mouseStartPosY);
        }
    }
    function UpdateKeyInput(p_deltaTime)
    {
        local xTranslate = 0.0;
        local yTranslate = 0.0;
        local zTranslate = 0.0;
        local playerTransform = player.GetTransform();
        // Log.Trace("Forward: " + playerTransform.Forward().tostring());
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_W))
        {
            zTranslate += playerComp.speed;
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_S))
        {
            zTranslate -= playerComp.speed;
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_D))
        {
            xTranslate += playerComp.speed;
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_A))
        {
            xTranslate -= playerComp.speed;
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_SPACE))
        {
            yTranslate += playerComp.speed;
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_LEFT_CONTROL))
        {
            yTranslate -= playerComp.speed;
        }

        local forward = playerTransform.Forward();
        if(rotY > -90 && rotY < 90.0)
            forward *= -1.0;
        local direction = playerTransform.Right() * xTranslate + forward * zTranslate + playerTransform.Up() * yTranslate;
        if(direction.Length() > 0)
        {
            // direction = direction.Normalized() * playerComp.speed ;
            playerTransform.TranslateVec(direction * p_deltaTime);
            // player.GetRigidBody().SetVelocityVec(direction);
        }
    }

    function OnKeyPressed(p_keycode, p_repeatCount)
    {
        if(p_keycode == KeyCode.CHAOS_KEY_LEFT_ALT)
        {
            captureMouse = false;
            Input.HideMouse(false);
        }
    }

    function OnKeyReleased(p_keycode)
    {
        if(p_keycode == KeyCode.CHAOS_KEY_LEFT_ALT)
        {
            captureMouse = true;
            Input.HideMouse(true);
        }
    }

    function FetchPlayer()
    {
        playerComp = SceneSystem.GetCurrentScene().FindComponentOfType("FPControllerComponent");
        Log.Trace("FPControllerComponent found: " + (playerComp != null).tostring() + " instance of FPControllerComponent: " + (playerComp instanceof FPControllerComponent).tostring());
        player = playerComp.GetOwner();
    }
}

this.instance <- FPControllerSystem();
