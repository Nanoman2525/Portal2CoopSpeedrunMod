//-----------------------------------------------------------------------------
// Coop Speedrun Mod VScript
// Author: Nanoman2525
//
// Notes: Right now, we are resorting to using a server plugin to run this
// script since mapspawn.nut causes some errors if it runs too early before
// entities are initialized. In the future, we could pack this into the plugin.
//-----------------------------------------------------------------------------

// List of TODO's
// Fix looping dialogue for Orange pickup gun in calibration
// Fix catapult block checkpoint transition (There's another map that does this as well...)
// Add disc transition in c4
// Fix rat maze taunt award
// Fix mp_coop_tbeam_laser_1 taunt award
// Make lasers panels flip faster
// Make C5 reveal faster

// Check if this is the client VM
if ( !( "Entities" in this ) )
{
    return;
}

//-----------------------------------------------------------------------------
// Shared code to across all maps
//-----------------------------------------------------------------------------
local szMaps =
[
    "mp_coop_doors", "mp_coop_race_2", "mp_coop_laser_2", "mp_coop_rat_maze", "mp_coop_laser_crusher", "mp_coop_teambts",
    "mp_coop_fling_3", "mp_coop_infinifling_train", "mp_coop_come_along", "mp_coop_fling_1", "mp_coop_catapult_1", "mp_coop_multifling_1", "mp_coop_fling_crushers", "mp_coop_fan",
    "mp_coop_wall_intro", "mp_coop_wall_2", "mp_coop_catapult_wall_intro", "mp_coop_wall_block", "mp_coop_catapult_2", "mp_coop_turret_walls", "mp_coop_turret_ball", "mp_coop_wall_5",
    "mp_coop_tbeam_redirect", "mp_coop_tbeam_drill", "mp_coop_tbeam_catch_grind_1", "mp_coop_tbeam_laser_1", "mp_coop_tbeam_polarity", "mp_coop_tbeam_polarity2", "mp_coop_tbeam_polarity3", "mp_coop_tbeam_maze", "mp_coop_tbeam_end",
    "mp_coop_paint_come_along", "mp_coop_paint_redirect", "mp_coop_paint_bridge", "mp_coop_paint_walljumps", "mp_coop_paint_speed_fling", "mp_coop_paint_red_racer", "mp_coop_paint_speed_catch", "mp_coop_paint_longjump_intro",
    "mp_coop_separation_1", "mp_coop_tripleaxis", "mp_coop_catapult_catch", "mp_coop_2paints_1bridge", "mp_coop_paint_conversion", "mp_coop_bridge_catch", "mp_coop_laser_tbeam", "mp_coop_paint_rat_maze", "mp_coop_paint_crazy_box"
]

szMapName <- GetMapName();
iTargetMapIndex <- -1;

foreach ( iMapIndex, szMap in szMaps )
{
    if ( szMapName == szMap )
    {
        iTargetMapIndex = iMapIndex; // Get the current map index from the above list of maps
    }
}

// Helper funcs
local IsCalibration = function() { return ( szMapName == "mp_coop_start" ) }
local IsHub = function() { return ( szMapName == "mp_coop_lobby_2" || szMapName == "mp_coop_lobby_3" ) }
local IsCourse1 = function() { return ( iTargetMapIndex >= 0  && iTargetMapIndex <= 5  ) }
local IsCourse2 = function() { return ( iTargetMapIndex >= 6  && iTargetMapIndex <= 13 ) }
local IsCourse3 = function() { return ( iTargetMapIndex >= 14 && iTargetMapIndex <= 21 ) }
local IsCourse4 = function() { return ( iTargetMapIndex >= 22 && iTargetMapIndex <= 30 ) }
local IsCourse5 = function() { return ( iTargetMapIndex >= 31 && iTargetMapIndex <= 38 ) }
local IsCourse6 = function() { return ( iTargetMapIndex >= 39 && iTargetMapIndex <= 47 ) }

// Speed up checkpoint airlocks and ending transitions based on map type
local bAlreadyAddedManagerOutput = false;
for ( local pRelay; pRelay = Entities.FindByClassname( pRelay, "logic_relay" ); )
{
    local pszRelayTargetName = pRelay.GetName();

    if ( pszRelayTargetName.find( "airlock_exit_door_open_rl" ) != null )
    {
        EntFire( pszRelayTargetName, "AddOutput", "OnTrigger " + pszRelayTargetName + ":Disable" );
    }
    else if ( pszRelayTargetName.find( "rl_start_exit_finished" ) != null )
    {
        // Enable the trigger to exit the map and do so immediately
        EntFire( pszRelayTargetName, "Enable" );
    }
    else if ( pszRelayTargetName.find( "rl_start_exit" ) != null )
    {
        if ( IsCourse5() )
        {
            EntFire( "coop_man_start_transition", "AddOutput", "OnChangeToAllTrue " + pszRelayTargetName + ":Trigger" );
        }
        else
        {
            // Prevent cutscene for disassembly
            EntFire( pszRelayTargetName, "Disable" );
        }
        continue; // No need to iterate over managers in this scenario
    }
    else
    {
        continue; // We need a proper relay that either contains "airlock_exit_door_open_rl" or "rl_start_exit_finished"
    }

    for ( local pManager; pManager = Entities.FindByClassname( pManager, "logic_coop_manager" ); )
    {
        local pszManagerTargetName = pManager.GetName();
        local iStringIndexMatchCheckpoint = pszManagerTargetName.find( "coopman_airlock_success" );

        // Speed up whichever logic_relay and logic_coop_manager belong to each other (This allows maps with multiple checkpoints to work properly)
        if ( iStringIndexMatchCheckpoint != null )
        {
            local szSlicedManagerName = pszManagerTargetName.slice( 0, iStringIndexMatchCheckpoint );
            local szSlicedRelayName = pszRelayTargetName.slice( 0, iStringIndexMatchCheckpoint );
            if ( szSlicedManagerName != null && szSlicedRelayName != null && szSlicedManagerName == szSlicedRelayName )
            {
                EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + pszRelayTargetName + ":Trigger::0.3" );
            }
        }

        // Make maps end immediately
        local iStringIndexMatchExit = pszManagerTargetName.find( "coopman_exit_level" );
        if ( iStringIndexMatchExit != null )
        {
            local szStringInstance = pszManagerTargetName.slice( 0, iStringIndexMatchExit ); // i.e. "InstanceAuto8-"
            if ( IsHub() && pszRelayTargetName.find( "rl_start_exit_finished" ) )
            {
                EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "template_movie_level_transition:ForceSpawn" );
                EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "gladosendoflevelrelay:Trigger::1.50" );
            }
            else if ( !bAlreadyAddedManagerOutput )
            {
                EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "template_movie_level_transition:ForceSpawn" );
                EntFire( pszManagerTargetName, "AddOutput", "OnChangeToAllTrue " + szStringInstance + "transition_script:RunScriptCode:TransitionFromMap()::2.00" );

                bAlreadyAddedManagerOutput = true; // For cleanliness, we don't want to add this output multiple times
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Now attempt to filter out the correct map-specific code
// We sort is according to the course type and level name
//-----------------------------------------------------------------------------

local SpeedUpGel = function()
{
    // Get rid of the gel spread RNG on all maps
    for ( local pEnt; pEnt = Entities.FindByClassname( pEnt, "info_paint_sprayer" ); )
    {
        pEnt.__KeyValueFromString( "blob_streak_percentage", "100" );
        pEnt.__KeyValueFromString( "min_streak_time", "2" );
        pEnt.__KeyValueFromString( "min_streak_speed_dampen", "1" );
        pEnt.__KeyValueFromString( "max_streak_speed_dampen", "1" );
    }
}

// Map-specific fixes...
if ( IsCalibration() )
{
    // Skip the video from playing on map loads
    EntFire( "teleport_start", "Enable" );
    EntFire( "@relay_start_both_connected", "Disable" );

    // Skip the taunting area
    local pTauntTrigger = Entities.FindByClassnameNearest( "trigger_playerteam", Vector( -9952, -4384, 2965.64 ), 3 );
    EntFireByHandle( pTauntTrigger, "AddOutput", "OnStartTouchAll relay_gesture_1_move_on:Trigger", 0, null, null );

    // Skip the starting dialogue and instead use the orange pinging one
    Entities.FindByClassnameNearest( "trigger_once", Vector( -10056, -4400, 2784 ), 3 ).Destroy();
    EntFireByHandle( pTauntTrigger, "AddOutput", "OnStartTouchAll @glados:RunScriptCode:OrangePingStartDone = 1;", 0, null, null ); // glados_coop.nut (Done this way so that it doesn't start GladosThink() until both players have spawned in)

    // Skip the blue pinging area
    local pBluePingTrigger = Entities.FindByClassnameNearest( "trigger_once", Vector( -9984, -4384, 2072 ), 3 );
    EntFireByHandle( pBluePingTrigger, "AddOutput", "OnTrigger relay_ping_1_move_on:Trigger", 0, null, null );

    // Catch up our state to enable Orange pinging the elements
    EntFire( "@global_no_pinging_blue", "TurnOn" ); // Blue cannot ping in this state, so just disable it from the start of the map
    local pRedPingTrigger = Entities.FindByClassnameNearest( "trigger_once", Vector( -9984, -4392, 1015 ), 3 );
    EntFireByHandle( pRedPingTrigger, "AddOutput", "OnTrigger trigger_orange_ping_hint_2:Enable", 0, null, null ); // Make the hint show
    EntFireByHandle( pRedPingTrigger, "AddOutput", "OnTrigger relay_begin_ping_2:Trigger", 0, null, null );
    EntFireByHandle( pRedPingTrigger, "AddOutput", "OnTrigger script_ping_select_test:RunScriptCode:ORANGE_enabled = 1; SelectChoicesOrangeStart()", 0, null, null ); // mp_coop_ping_select_test.nut (We do it this way so that Orange cannot ping the panels while falling down)

    // Expose the two weapon_portalgun's faster and as soon as we get to them
    EntFire( "player_1_portalgun_door_pedistal_removal_lvl2", "SetSpeed", "40" ); // Red
    EntFire( "player_2_portalgun_door_pedistal_removal_lvl2", "SetSpeed", "40" ); // Blue
    pGunCalibrationTrigger <- Entities.FindByClassnameNearest( "trigger_once", Vector( -10004, -4400, 300 ), 3 );
    local AddCalibrationTriggerOutput = function( szOutput ) { EntFireByHandle( pGunCalibrationTrigger, "AddOutput", "OnTrigger " + szOutput, 0, null, null ); }
    AddCalibrationTriggerOutput( "player_1_portalgun_door_pedistal_removal_lvl2" + ":Close" );
    AddCalibrationTriggerOutput( "player_2_portalgun_door_pedistal_removal_lvl2" + ":Close" );
    AddCalibrationTriggerOutput( "ping_detector_blue:Enable::1.5" );
    AddCalibrationTriggerOutput( "ping_detector_orange:Enable::1.5" );
    AddCalibrationTriggerOutput( "tank_portalgun_orange:Activate::1.5" );
    AddCalibrationTriggerOutput( "tank_portalgun_blue:Activate::1.5" );
    AddCalibrationTriggerOutput( "snd_gun_steam:PlaySound" );
    AddCalibrationTriggerOutput( "snd_gun_steam:Kill::1.00" );
    AddCalibrationTriggerOutput( "steam_guns:Start" );
    AddCalibrationTriggerOutput( "steam_guns:Stop::1.00" );
    AddCalibrationTriggerOutput( "steam_guns:Kill::1.00" );
    delete pGunCalibrationTrigger;
}
else if ( IsHub() )
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
    // We do it this way since it is too early to tell if the game is splitscreen or not
    function OpenCourseDoors()
    {
        local bFromCali = true;
        local bIsAnyCourse5LevelComplete = false;
        for ( local iCourse = 1; iCourse <= 5; iCourse++ )
        {
            for ( local iLevel = 1; iLevel <= 9; iLevel++ ) // 9 levels is the highest that a course has (Courses 4 and 6)
            {
                // If any level is completed from its course, keep the course door open (Splitscreen has everything unlocked)
                if ( IsLevelComplete( iCourse - 1, iLevel - 1 ) || IsLocalSplitScreen() )
                {
                    if ( iCourse == 5 )
                    {
                        bIsAnyCourse5LevelComplete = true; // Someone has completed a Course 5 level, so there is no cutscene for its reveal
                    }
                    else
                    {
                        bFromCali = false;
                        EntFire( "track" + iCourse.tostring() + "-math_coop_door_open", "Add", "1" );
                        EntFire( "track" + iCourse.tostring() + "-math_coop_door_open", "Kill" );
                    }

                    break; // Move on to the next course
                }
            }
        }

        // Open C1 if we have no progress whatsoever in any other levels
        if ( bFromCali )
        {
            EntFire( "track1-math_coop_door_open", "Add", "1" );
            EntFire( "track1-math_coop_door_open", "Kill" );
        }
        // If we aren't newbies, check if this is a reveal for Course 5 and speed it up
        else if ( !bIsAnyCourse5LevelComplete )
        {
            // TODO
        }

        Entities.FindByName( null, "trigger_run_script" ).DisconnectOutput( "OnStartTouchBluePlayer", "OpenCourseDoors" ); // We only need to fire this function once
    }
    Entities.FindByName( null, "trigger_run_script" ).ConnectOutput( "OnStartTouchBluePlayer", "OpenCourseDoors" );

    // Exists in only the new lobby. Speed up the DLC course area
    if ( szMapName == "mp_coop_lobby_3" )
    {
        // Reveal faster
        if ( !GetHaveSeenDLCTubesReveal() )
        {
            EntFire( "dlc_transport_sheath", "SetSpeed", "500" );
        }

        // Skip the viewcontrol for the tube travel to the DLC area
        // Note that Blue will always end up on the left side, while Orange ends up on the right side
        Entities.FindByName( null, "blue_hub_to_dlc_cam" ).Destroy();
        Entities.FindByName( null, "blue_hub_to_dlc_fade" ).Destroy();
        Entities.FindByName( null, "orange_hub_to_dlc_cam" ).Destroy();
        Entities.FindByName( null, "orange_hub_to_dlc_fade" ).Destroy();
        local pFirstTeleportTrigger = Entities.FindByClassnameNearest( "trigger_playerteam", Vector( 2112, -1244, -832 ), 3 );
        EntFireByHandle( pFirstTeleportTrigger, "AddOutput", "OnStartTouchBluePlayer left_hub_to_dlc_destination:TeleportEntity:!activator", 0, null, null );
        EntFireByHandle( pFirstTeleportTrigger, "AddOutput", "OnStartTouchOrangePlayer right_hub_to_dlc_destination:TeleportEntity:!activator", 0, null, null );

        // Skip viewcontrol back to hub
        // Note that unlike normal spawns, Blue ends up on the left side and Orange ends up on left
        Entities.FindByName( null, "blue_dlc_to_hub_cam" ).Destroy();
        Entities.FindByName( null, "blue_dlc_to_hub_fade" ).Destroy();
        Entities.FindByName( null, "orange_dlc_to_hub_cam" ).Destroy();
        Entities.FindByName( null, "orange_dlc_to_hub_fade" ).Destroy();
        local pSecondTeleportTrigger = Entities.FindByClassnameNearest( "trigger_playerteam", Vector( 2432, -1244, -832 ), 3 );
        EntFireByHandle( pSecondTeleportTrigger, "AddOutput", "OnStartTouchBluePlayer left_dlc_to_hub_destination:TeleportEntity:!activator", 0, null, null );
        EntFireByHandle( pSecondTeleportTrigger, "AddOutput", "OnStartTouchOrangePlayer right_dlc_to_hub_destination:TeleportEntity:!activator", 0, null, null );
    }
}
else if ( IsCourse1() )
{
    if ( szMapName == "mp_coop_laser_2" )
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
        // TODO: Rework to award the taunt
        // Entities.FindByName( null, "coop_man_give_taunt" ).Destroy();
        // Entities.CreateByClassname( "logic_coop_manager" ).__KeyValueFromString( "targetname", "p2coop_taunt_manager" );
        // EntFire( "p2coop_taunt_manager", "AddOutput", "OnChangeToAllTrue @glados:RunScriptCode:RespondToTaunt(3)::0.10" );
        // EntFireByHandle( Entities.FindByClassnameNearest( "trigger_playerteam", Vector( -176, -656, -632 ), 3 ), "AddOutput", "OnStartTouchBluePlayer p2coop_taunt_manager:SetStateBTrue", 0, null, null );
        // EntFireByHandle( Entities.FindByClassnameNearest( "trigger_playerteam", Vector( -176, -656, -632 ), 3 ), "AddOutput", "OnStartTouchOrangePlayer p2coop_taunt_manager:SetStateATrue", 0, null, null );
    }
    else if ( szMapName == "mp_coop_teambts" )
    {
        // Make disc transition the map
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 1164, -3051, 5682 ), 3 ), "AddOutput", "OnStartTouch template_movie_level_transition:ForceSpawn", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 1164, -3051, 5682 ), 3 ), "AddOutput", "OnStartTouch transition_script:RunScriptCode:TransitionFromMap()::1.60", 0, null, null );
    }
}
else if ( IsCourse2() )
{
    if ( szMapName == "mp_coop_fan" )
    {
        // Make fan faster at slowing down, but not speeding up
        EntFire( "catcher", "AddOutput", "OnPowered brush_fan:AddOutput:fanfriction 90" );
        EntFire( "catcher", "AddOutput", "OnUnPowered brush_fan:AddOutput:fanfriction 18" );

        // Make disc transition the map
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( -259.8, 881.89, 231.28 ), 3 ), "AddOutput", "OnStartTouch template_movie_level_transition:ForceSpawn", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( -259.8, 881.89, 231.28 ), 3 ), "AddOutput", "OnStartTouch transition_script:RunScriptCode:TransitionFromMap()::1.60", 0, null, null );
    }
}
else if ( IsCourse3() )
{
    if ( szMapName == "mp_coop_tbeam_laser_1" )
    {
        // TODO: Award taunt, but don't force it
    }
    if ( szMapName == "mp_coop_wall_5" )
    {
        // Make disc transition the map
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 2133.35, -1587.71, 297.86 ), 3 ), "AddOutput", "OnStartTouch template_movie_level_transition:ForceSpawn", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 2133.35, -1587.71, 297.86 ), 3 ), "AddOutput", "OnStartTouch transition_script:RunScriptCode:TransitionFromMap()::1.60", 0, null, null );
    }
}
else if ( IsCourse4() )
{

}
else if ( IsCourse5() )
{
    SpeedUpGel();

    if ( szMapName == "mp_coop_paint_longjump_intro" )
    {
        // Speed up spawning door
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 224, -7366, 948 ), 3 ), "AddOutput", "OnTrigger AutoInstance1-door_open:Trigger", 0, null, null );
        EntFireByHandle( Entities.FindByClassnameNearest( "trigger_once", Vector( 224, -7366, 948 ), 3 ), "AddOutput", "OnTrigger AutoInstance1-door_open:Disable::0.3", 0, null, null );
        EntFire( "AutoInstance1-door_upper", "SetSpeed", "150" );
        EntFire( "AutoInstance1-door_lower", "SetSpeed", "150" );
    }
}
else if ( IsCourse6() )
{
    SpeedUpGel();
}

delete szMapName;
delete iTargetMapIndex;