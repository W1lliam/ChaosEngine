class BallSpawnerSystem
{
    static typeName = "BallSpawnerSystem";
	constructor()
	{
        Log.Info("BallSpawnerSystem created!");
	}

    function Update(p_deltaTime)
    {
         ComponentManager.ForeachComponent("BallSpawnerComponent", function(ballSpawnerComponent)
        {
            ballSpawnerComponent.spawnTimer += p_deltaTime;
            if(ballSpawnerComponent.spawnTimer > ballSpawnerComponent.spawnCooldown && ballSpawnerComponent.ballCount < ballSpawnerComponent.maxBallCount)
            {
                SpawnBall(ballSpawnerComponent, ballSpawnerComponent.GetOwner().GetTransform().GetPosition());
                ballSpawnerComponent.spawnTimer = 0.0;
            }
        }, this);
    }



    function SpawnBall(ballSpawnerComponent, spawnPos)
    {
        local name              = "sphere" + ballSpawnerComponent.ballCount;
        ++ballSpawnerComponent.ballCount;
        local sphere            = SceneSystem.CreateMeshGameObject(name, "Sphere", name);
        local sphereTransform   = sphere.GetTransform();
        local sphereMaterial    = sphere.GetMaterial();

        local color = Vector3(rand().tofloat() / RAND_MAX, rand().tofloat() / RAND_MAX, rand().tofloat() / RAND_MAX);
        sphereMaterial.SetShader("SolidColor");
        sphereMaterial.SetUniformVector3("albedo", color);
        sphereMaterial.SetUniformVector3("emissive", Vector3(0.0,0.0,0.0));
        sphereMaterial.SetUniformFloat("metallic", 0.3);
        sphereMaterial.SetUniformFloat("roughness", 0.8);



        sphereTransform.SetPositionVec(spawnPos);
        sphereTransform.SetScale(0.3, 0.3, 0.3);
        sphere.AddSphereCollider(Vector3(0.5, 0.6, 0.7), Vector3(0.0, 0.0, 0.0), 0.3);
        local sphereRb = sphere.AddRigidBody(RigidBodyType.DYNAMIC, 50.0, Vector3(0.0, -0.2, 0.0));
        sphereRb.SetAngularVelocity(0.3, 0.3, 0.3);

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
}

this.instance <- BallSpawnerSystem();
