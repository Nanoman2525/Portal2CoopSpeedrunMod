//********************************************************************************************
//MAPSPAWN.nut is called on newgame or transitions
//********************************************************************************************
// printl("==== calling mapspawn.nut")

// Right now, we are resorting to listenserver.cfg to run this, which is a leftover from standard Source servers
// It will do for now (yuck), but finding a more appropriate way to run VScript automatically would be optimal
// mapspawn.nut runs too early before entities are initialized, so we get some errors

// Check if this is the client VM
if ( !( "Entities" in this ) )
{
    return;
}

local szMaps =
[
    "mp_coop_start", "mp_coop_lobby_3", // We include these maps as part of the list (mp_coop_lobby_2 obsolete)
    "mp_coop_doors", "mp_coop_race_2", "mp_coop_laser_2", "mp_coop_rat_maze", "mp_coop_laser_crusher", "mp_coop_teambts",
    "mp_coop_fling_3", "mp_coop_infinifling_train", "mp_coop_come_along", "mp_coop_fling_1", "mp_coop_catapult_1", "mp_coop_multifling_1", "mp_coop_fling_crushers", "mp_coop_fan",
    "mp_coop_wall_intro", "mp_coop_wall_2", "mp_coop_catapult_wall_intro", "mp_coop_wall_block", "mp_coop_catapult_2", "mp_coop_turret_walls", "mp_coop_turret_ball", "mp_coop_wall_5",
    "mp_coop_tbeam_redirect", "mp_coop_tbeam_drill", "mp_coop_tbeam_catch_grind_1", "mp_coop_tbeam_laser_1", "mp_coop_tbeam_polarity", "mp_coop_tbeam_polarity2", "mp_coop_tbeam_polarity3", "mp_coop_tbeam_maze", "mp_coop_tbeam_end",
    "mp_coop_paint_come_along", "mp_coop_paint_redirect", "mp_coop_paint_bridge", "mp_coop_paint_walljumps", "mp_coop_paint_speed_fling", "mp_coop_paint_red_racer", "mp_coop_paint_speed_catch", "mp_coop_paint_longjump_intro",
    "mp_coop_separation_1", "mp_coop_tripleaxis", "mp_coop_catapult_catch", "mp_coop_2paints_1bridge", "mp_coop_paint_conversion", "mp_coop_bridge_catch", "mp_coop_laser_tbeam", "mp_coop_paint_rat_maze", "mp_coop_paint_crazy_box"
]

local szMapName = GetMapName();
local iTargetMapIndex = -1;

// Get the current map index from the above list of maps
foreach ( iMapIndex, szMap in szMaps )
{
    if ( szMapName == szMap )
    {
        iTargetMapIndex = iMapIndex;
    }
}

//-----------------------------------------------------------------------------
// Shared code to across all maps
//-----------------------------------------------------------------------------

// Speed up checkpoint airlock
for ( local pRelay; pRelay = Entities.FindByClassname( pRelay, "logic_relay" ); )
{
    local pszRelayTargetName = pRelay.GetName();

    if ( pszRelayTargetName.find( "airlock_exit_door_open_rl" ) != null )
    {
        EntFire( "airlock_exit_door_open_rl", "AddOutput", "OnTrigger " + pszRelayTargetName + ":Disable" );
    }
    else if ( pszRelayTargetName.find( "rl_start_exit" ) != null )
    {
        // Speed up ending transition
        EntFire( pszRelayTargetName, "Disable" );
    }
    else
    {
        continue;
    }

    for ( local pManager; pManager = Entities.FindByClassname( pManager, "logic_coop_manager" ); )
    {
        local pszManagerTargetName = pManager.GetName();

        // TODO: This breaks if there are mulitple checkpoints in a map! (i.e. mp_coop_catapult_1)
        // Works otherwise...
        if ( pszManagerTargetName.find( "coopman_airlock_success" ) != null )
        {
            EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + pszRelayTargetName + ":Trigger::0.3" );
        }
    }
}

// Separate iteration over the managers again... We don't want to add this output multiple times
// Makes maps end immediately
for ( local pManager; pManager = Entities.FindByClassname( pManager, "logic_coop_manager" ); )
{
    local pszManagerTargetName = pManager.GetName(); // i.e. "InstanceAuto8-coopman_exit_level"
    local iStringIndexMatch = pszManagerTargetName.find( "coopman_exit_level" );

    if ( iStringIndexMatch != null )
    {
        local szStringInstance = pszManagerTargetName.slice( 0, iStringIndexMatch ); // i.e. "InstanceAuto8-"

        EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "template_movie_level_transition:ForceSpawn" );
        EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "transition_script:RunScriptCode:TransitionFromMap()::2.00" );

        break; // Only one exit area per map...
    }
}

// Run the correct VScript accordingly to the course type
if ( iTargetMapIndex <= 7 ) // Course 1 (And calibration)
{
    // Map-specific fixes...
    if ( szMapName == "mp_coop_start" )
    {
        // TODO
    }
    else if ( /*szMapName == "mp_coop_lobby_2" || */szMapName == "mp_coop_lobby_3" )
    {
        // Remove starting tube blocker
        EntFire( "relay_elevator_open", "Trigger" );
        EntFire( "relay_elevator_open", "Disable" );
        EntFire( "relay_elevator_open1", "Trigger" );
        EntFire( "relay_elevator_open1", "Disable" );
        EntFire( "blocker_blue", "Kill" );
        EntFire( "blocker_orange", "Kill" );
        Entities.FindByClassnameNearest( "trigger_playerteam", Vector( 3924, 3352, -480 ), 3 ).Destroy();
        Entities.FindByClassnameNearest( "trigger_playerteam", Vector( 3924, 3496, -480 ), 3 ).Destroy();

        // Open C1-C4 hub doors
        local bOpenedC1Door = false; // This is here to that we know if someone just came out of calibration
        for ( local iCourse = 1; iCourse <= 4; iCourse++ )
        {
            for ( local iLevel = 1; iLevel <= 9; iLevel++ ) // 9 levels is the highest that a course has
            {
                // If any level is completed from THIS course, open its door automatically (Splitscreen has everything unlocked)
                // Critical TODO: This check below doesn't work even when in splitscreen ??
                if ( IsLevelComplete( iCourse - 1, iLevel - 1 ) || IsLocalSplitScreen() )
                {
                    bOpenedC1Door = true; // Not fom cali

                    EntFire( "track" + iCourse.tostring() + "-math_coop_door_open", "Add", "1" );
                    EntFire( "track" + iCourse.tostring() + "-math_coop_door_open", "Kill" );
                }
            }
        }

        if ( !bOpenedC1Door )
        {
            // Open C1
            EntFire( "track1-math_coop_door_open", "Add", "1" );
            EntFire( "track1-math_coop_door_open", "Kill" );
        }
    }
    else if ( szMapName == "mp_coop_laser_2" )
    {
        // Scrap this... Changing speed midgame does not work
        // For each of the same exact entity type, differing by 64 units
        // Speed up panel flip blocking turrets
        // for ( local vecYPos = 184; vecYPos != -72; vecYPos = vecYPos - 64 )
        // {
        //     Entities.FindByClassnameNearest( "func_door", Vector( 488, vecYPos, -614 ), 3 ).__KeyValueFromString( "speed", "10" );
        //     // EntFireByHandle( Entities.FindByClassnameNearest( "func_door", Vector( 488, vecYPos, -614 ), 3 ), "AddOutput", "speed 10", 0, null, null );
        // }

        // TODO: Continue working on this
        // Entities.FindByName( null, "button_arms-proxy" ).Destroy();
        // EntFireByHandle( Entities.FindByName( null, "button_arms-button" ), "AddOutput", "OnPressed arm_1-robot_makeramp_05:SetAnimation:arm_1-robot_makeramp_05", 0, null, null );
    }
    else if ( szMapName == "mp_coop_rat_maze" )
    {
        // Prevent taunt at start of map, but also trigger the dialogue at the right time and conditions
        Entities.FindByName( null, "coop_man_give_taunt" ).Destroy();
        Entities.CreateByClassname( "logic_coop_manager" ).__KeyValueFromString( "targetname", "p2coop_taunt_manager" );
        EntFire( "p2coop_taunt_manager", "AddOutput", "OnChangeToAllTrue @glados:RunScriptCode:RespondToTaunt(3)::0.10" );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_playerteam", Vector( -176, -656, -632 ), 3 ), "AddOutput", "OnStartTouchBluePlayer p2coop_taunt_manager:SetStateBTrue", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_playerteam", Vector( -176, -656, -632 ), 3 ), "AddOutput", "OnStartTouchOrangePlayer p2coop_taunt_manager:SetStateATrue", 0, null, null );
    }
    else if ( szMapName == "mp_coop_teambts" )
    {
        // Make disc transition the map
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 1164, -3051, 5682 ), 3 ), "AddOutput", "OnStartTouch template_movie_level_transition:ForceSpawn", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 1164, -3051, 5682 ), 3 ), "AddOutput", "OnStartTouch transition_script:RunScriptCode:TransitionFromMap()::1.60", 0, null, null );
    }
}
else if ( iTargetMapIndex >= 8 && iTargetMapIndex <= 15 ) // Course 2
{

}
else if ( iTargetMapIndex >= 16 && iTargetMapIndex <= 23 ) // Course 3
{

}
else if ( iTargetMapIndex >= 24 && iTargetMapIndex <= 32 ) // Course 4
{

}
else if ( iTargetMapIndex >= 33 && iTargetMapIndex <= 40 ) // Course 5
{

}
else if ( iTargetMapIndex >= 41 && iTargetMapIndex <= 49 ) // Course 6
{

}