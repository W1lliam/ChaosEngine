class SqComponent 
{	
    cppComponent = null;

	constructor(p_cppComponent)
	{
        cppComponent = p_cppComponent;
	}
    
    function GetOwner()
    {
        return cppComponent.GetOwner();
    }
}