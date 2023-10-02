//********************************************************************************************
//MAPSPAWN.nut is called on newgame or transitions
//********************************************************************************************
printl("==== calling mapspawn.nut")



// Check if this is the server VM
if ( ( "Entities" in this ) )
{
    IncludeScript( "mapsupport/" + GetMapName() );
}