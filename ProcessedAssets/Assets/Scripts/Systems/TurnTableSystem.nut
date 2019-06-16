class TurnTableSystem
{	
    static typeName = "TurnTableSystem";
	constructor()
	{
        Log.Info("TurnTableSystem created!");
	}

    function Update(p_deltaTime)
    {
        ComponentManager.ForeachComponent("TurnTableComponent", function(turnTableComponent)
        {
            turnTableComponent.GetOwner().GetTransform().Rotate(0.0, turnTableComponent.speed * p_deltaTime, 0.0);
        }, this);
    }
}
 
this.instance <- TurnTableSystem();