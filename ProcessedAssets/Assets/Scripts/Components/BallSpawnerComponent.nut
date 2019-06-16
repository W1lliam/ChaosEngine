class BallSpawnerComponent extends SqComponent
{	
	constructor(p_cppComponent)
	{
        base.constructor(p_cppComponent);
        Log.Info("BallSpawnerComponent created!");
	}
    ballCount = 0;
    maxBallCount = 50;
    spawnCooldown = 2.0;
    spawnTimer = 0.0;
}