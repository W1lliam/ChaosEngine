class PlayerSystem
{	
    static typeName = "PlayerSystem";
	constructor()
	{
        Log.Info("PlayerSystem created!");
        // FetchPlayer();
	}
    ballCount = 0;
    playerComp = null;
    player = null;
    mouseStartPosX = 0.0;
    mouseStartPosY = 0.0;
    captureMouse = true;

    function Start()
    {
        FetchPlayer();
        
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
            local yOffset = Input.GetMouseY() - mouseStartPosY;
            player.GetRigidBody().SetAngularVelocity(0.0, xOffset * p_deltaTime, 0.0);
            Input.SetMousePosition(mouseStartPosX, mouseStartPosY);
        }
    }
    function UpdateKeyInput(p_deltaTime)
    {
        local direction = Vector3(0.0,0.0,0.0);
        local playerTransform = player.GetTransform();
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_W))
        {
            direction += playerTransform.Forward();
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_S))
        {
            direction -= playerTransform.Forward();
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_D))
        {
            direction += playerTransform.Right();
        }
        if(Input.IsKeyPressed(KeyCode.CHAOS_KEY_A))
        {
            direction -= playerTransform.Right();
        }
        local yVelocity = player.GetRigidBody().GetVelocity().y;
        Log.Trace("Forward: " + playerTransform.Forward().tostring() + " Direction: " + direction.tostring());
        direction = direction.Normalized() * playerComp.speed * p_deltaTime;
        player.GetRigidBody().SetVelocity(direction.x, yVelocity, direction.z);
    }
    
    function ThrowBall()
    {
        local name              = "sphere" + playerComp.ballCount;
        ++playerComp.ballCount;
        local sphere            = SceneSystem.CreateMeshGameObject(name, "Sphere", name);
        local sphereTransform   = sphere.GetTransform();
        local sphereMaterial    = sphere.GetMaterial();

        local color = Vector3(rand().tofloat() / RAND_MAX, rand().tofloat() / RAND_MAX, rand().tofloat() / RAND_MAX);
        sphereMaterial.SetShader("SolidColor");
        sphereMaterial.SetUniformVector3("albedo", color);
        sphereMaterial.SetUniformVector3("emissive", color);
        sphereMaterial.SetUniformFloat("metallic", 0.3);
        sphereMaterial.SetUniformFloat("roughness", 0.8);



        sphereTransform.SetPositionVec(player.GetTransform().GetPosition() + player.GetTransform().Forward() * player.GetRigidBody().GetVelocity().Length());
        sphereTransform.SetScale(0.3, 0.3, 0.3);
        sphere.AddSphereCollider(Vector3(0.5, 0.6, 0.7), Vector3(0.0, 0.0, 0.0), 0.2);
        sphere.AddRigidBody(RigidBodyType.DYNAMIC, 50.0, player.GetTransform().Forward() * playerComp.throwForce);
        local audioSource = sphere.AddAudioSource("noot", "Editor");
        audioSource.SetVolume(1.0);
        audioSource.SetSpaceMode(true);
        audioSource.Play();

        local sphereLight = sphere.AddLight();
        if(sphereLight == null)
        {
            Log.Trace("Error creating Light");
            return;
        }
        sphereLight.SetType(LightType.SPOT);
        sphereLight.SetRange(30.0);
        sphereLight.SetAngle(180.0);

        sphereLight.SetColor(color.x, color.y, color.z, 1.0);
        sphereLight.SetIntensity(5.0);
        sphereLight.SetDirty(true);
    }

    function OnKeyPressed(p_keycode, p_repeatCount)
    {
        if(p_keycode == KeyCode.CHAOS_KEY_B)
        {
            ThrowBall();
        }
        if(p_keycode == KeyCode.CHAOS_KEY_SPACE && p_repeatCount == 0)
        {
            player.GetRigidBody().AddForce(0.0, 5.0, 0.0, ForceType.IMPULSE);
        }
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
        playerComp = SceneSystem.GetCurrentScene().FindComponentOfType("PlayerComponent");
        Log.Trace("Player Component found: " + (playerComp != null).tostring() + " instance of PlayerComponent: " + (playerComp instanceof PlayerComponent).tostring());
        if(playerComp != null)
        {
            player = playerComp.GetOwner();
            Log.Trace("Player found: " + (player != null).tostring() + " instance of Entity: " + (player instanceof Entity).tostring() + " Name: " + player.GetName());

            if(playerComp != null && player != null && player.GetRigidBody() == null)
            {
                player.GetTransform().SetPosition(0.0,20.0,0.0);

                player.AddBoxCollider(Vector3(5.0, 0.8, 0.0), Vector3(0.0, 0.0, 0.0), Vector3(0.5, 1.0, 0.5));
                local playerRB = player.AddRigidBody(RigidBodyType.DYNAMIC, 50.0, Vector3(0.0, 0.0, 0.0));
                playerRB.SetXRotationConstraint(true);
                playerRB.SetZRotationConstraint(true);
            }
            Input.HideMouse(true);
            Input.SetMousePosition(Renderer.GetScreenWidth() * 0.5, Renderer.GetScreenHeight() *0.5);
            mouseStartPosX = Input.GetMouseX();
            mouseStartPosY = Input.GetMouseY();
        }
    }
}
 
this.instance <- PlayerSystem();