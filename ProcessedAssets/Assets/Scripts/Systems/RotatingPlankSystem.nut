class RotatingPlankSystem
{	
    static typeName = "RotatingPlankSystem";
	constructor()
	{
        Log.Info("RotatingPlankSystem created!");
	}

    function Update(p_deltaTime)
    {
        ComponentManager.ForeachComponent("RotatingPlankComponent", function(rotatingPlankComponent)
        {
            rotatingPlankComponent.GetOwner().GetRigidBody().SetAngularVelocity(0.0, 0.0, rotatingPlankComponent.speed * p_deltaTime);
        }, this);
    }
}
 
this.instance <- RotatingPlankSystem();