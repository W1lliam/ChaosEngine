class PlayerComponent extends SqComponent
{	
	constructor(p_cppComponent)
	{
        base.constructor(p_cppComponent);
        Log.Info("PlayerComponent created!");
	}
    ballCount = 0;
    name = "Player";
    speed = 500.0;
    throwForce = 5.0;
}