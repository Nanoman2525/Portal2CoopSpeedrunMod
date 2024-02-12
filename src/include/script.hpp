static const char* script =
    "//-----------------------------------------------------------------------------\n"\
    "// Coop Speedrun Mod VScript\n"\
    "// Author: Nanoman2525\n"\
    "// Special thanks: Some really cool guy named Bumpy (P.S. he is green)\n"\
    "//\n"\
    "// Notes: Right now, we are resorting to using a server plugin to run this\n"\
    "// script since mapspawn.nut causes some errors if it runs too early before\n"\
    "// entities are initialized.\n"\
    "//-----------------------------------------------------------------------------\n"\
    "\n"\
    "// Check if this is the client VM\n"\
    "if ( !( \"Entities\" in this ) )\n"\
    "{\n"\
    "    return;\n"\
    "}\n"\
    "\n"\
    "//-----------------------------------------------------------------------------\n"\
    "// Shared code to across all maps\n"\
    "//-----------------------------------------------------------------------------\n"\
    "local pszMaps =\n"\
    "[\n"\
    "    \"mp_coop_doors\", \"mp_coop_race_2\", \"mp_coop_laser_2\", \"mp_coop_rat_maze\", \"mp_coop_laser_crusher\", \"mp_coop_teambts\",\n"\
    "    \"mp_coop_fling_3\", \"mp_coop_infinifling_train\", \"mp_coop_come_along\", \"mp_coop_fling_1\", \"mp_coop_catapult_1\", \"mp_coop_multifling_1\", \"mp_coop_fling_crushers\", \"mp_coop_fan\",\n"\
    "    \"mp_coop_wall_intro\", \"mp_coop_wall_2\", \"mp_coop_catapult_wall_intro\", \"mp_coop_wall_block\", \"mp_coop_catapult_2\", \"mp_coop_turret_walls\", \"mp_coop_turret_ball\", \"mp_coop_wall_5\",\n"\
    "    \"mp_coop_tbeam_redirect\", \"mp_coop_tbeam_drill\", \"mp_coop_tbeam_catch_grind_1\", \"mp_coop_tbeam_laser_1\", \"mp_coop_tbeam_polarity\", \"mp_coop_tbeam_polarity2\", \"mp_coop_tbeam_polarity3\", \"mp_coop_tbeam_maze\", \"mp_coop_tbeam_end\",\n"\
    "    \"mp_coop_paint_come_along\", \"mp_coop_paint_redirect\", \"mp_coop_paint_bridge\", \"mp_coop_paint_walljumps\", \"mp_coop_paint_speed_fling\", \"mp_coop_paint_red_racer\", \"mp_coop_paint_speed_catch\", \"mp_coop_paint_longjump_intro\",\n"\
    "    \"mp_coop_separation_1\", \"mp_coop_tripleaxis\", \"mp_coop_catapult_catch\", \"mp_coop_2paints_1bridge\", \"mp_coop_paint_conversion\", \"mp_coop_bridge_catch\", \"mp_coop_laser_tbeam\", \"mp_coop_paint_rat_maze\", \"mp_coop_paint_crazy_box\"\n"\
    "]\n"\
    "\n"\
    "pszMapName <- GetMapName();\n"\
    "iTargetMapIndex <- -1;\n"\
    "\n"\
    "foreach ( iMapIndex, pszMap in pszMaps )\n"\
    "{\n"\
    "    if ( pszMapName == pszMap )\n"\
    "    {\n"\
    "        iTargetMapIndex = iMapIndex; // Get the current map index from the above list of maps\n"\
    "    }\n"\
    "}\n"\
    "\n"\
    "if ( !( pszMapName.len() >= 7 && pszMapName.slice( 0, 7 ) == \"mp_coop\" ) )\n"\
    "{\n"\
    "    return; // Not a coop campaign map\n"\
    "}\n"\
    "\n"\
    "// Helper funcs\n"\
    "local IsCalibration = function() { return ( pszMapName == \"mp_coop_start\" ); }\n"\
    "local IsHub         = function() { return ( pszMapName == \"mp_coop_lobby_2\" || pszMapName == \"mp_coop_lobby_3\" ); }\n"\
    "local IsCourse1     = function() { return ( iTargetMapIndex >= 0  && iTargetMapIndex <= 5  ); }\n"\
    "local IsCourse2     = function() { return ( iTargetMapIndex >= 6  && iTargetMapIndex <= 13 ); }\n"\
    "local IsCourse3     = function() { return ( iTargetMapIndex >= 14 && iTargetMapIndex <= 21 ); }\n"\
    "local IsCourse4     = function() { return ( iTargetMapIndex >= 22 && iTargetMapIndex <= 30 ); }\n"\
    "local IsCourse5     = function() { return ( iTargetMapIndex >= 31 && iTargetMapIndex <= 38 ); }\n"\
    "local IsCourse6     = function() { return ( iTargetMapIndex >= 39 && iTargetMapIndex <= 47 ); }\n"\
    "\n"\
    "// Speed up checkpoint airlocks and ending transitions based on map type\n"\
    "local bAlreadyAddedManagerOutput = false;\n"\
    "for ( local pRelay; pRelay = Entities.FindByClassname( pRelay, \"logic_relay\" ); )\n"\
    "{\n"\
    "    local pszRelayTargetName = pRelay.GetName();\n"\
    "    local bIsExitRelay = false;\n"\
    "\n"\
    "    if ( pszRelayTargetName.find( \"airlock_exit_door_open_rl\" ) != null )\n"\
    "    {\n"\
    "        EntFire( pszRelayTargetName, \"AddOutput\", \"OnTrigger \" + pszRelayTargetName + \":Disable\" );\n"\
    "    }\n"\
    "    else if ( pszRelayTargetName.find( \"rl_start_exit_finished\" ) != null )\n"\
    "    {\n"\
    "        // Enable the trigger to exit the map and do so immediately\n"\
    "        EntFire( pszRelayTargetName, \"Enable\" );\n"\
    "        bIsExitRelay = true;\n"\
    "    }\n"\
    "    else if ( pszRelayTargetName.find( \"rl_start_exit\" ) != null )\n"\
    "    {\n"\
    "        if ( IsCourse5() )\n"\
    "        {\n"\
    "            EntFire( \"coop_man_start_transition\", \"AddOutput\", \"OnChangeToAllTrue \" + pszRelayTargetName + \":Trigger\" );\n"\
    "        }\n"\
    "        else\n"\
    "        {\n"\
    "            // Prevent cutscene for disassembly\n"\
    "            EntFire( pszRelayTargetName, \"Disable\" );\n"\
    "        }\n"\
    "        continue; // No need to iterate over managers in this scenario\n"\
    "    }\n"\
    "    else\n"\
    "    {\n"\
    "        continue; // We need a proper relay that either contains \"airlock_exit_door_open_rl\" or \"rl_start_exit_finished\"\n"\
    "    }\n"\
    "\n"\
    "    for ( local pManager; pManager = Entities.FindByClassname( pManager, \"logic_coop_manager\" ); ) // Yuck, but it works!!\n"\
    "    {\n"\
    "        local pszManagerTargetName = pManager.GetName();\n"\
    "\n"\
    "        if ( bIsExitRelay )\n"\
    "        {\n"\
    "            // Make maps end immediately\n"\
    "            local iStringIndexMatchExit = pszManagerTargetName.find( \"coopman_exit_level\" );\n"\
    "            if ( iStringIndexMatchExit != null )\n"\
    "            {\n"\
    "                local pszStringInstance = pszManagerTargetName.slice( 0, iStringIndexMatchExit ); // i.e. \"InstanceAuto8-\"\n"\
    "                if ( IsHub() && pszRelayTargetName.find( \"rl_start_exit_finished\" ) )\n"\
    "                {\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszStringInstance + \"fade_out:Fade\" );\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszStringInstance + \"template_movie_level_transition:ForceSpawn::0.6\" );\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszStringInstance + \"gladosendoflevelrelay:Trigger::2.10\" );\n"\
    "                }\n"\
    "                else if ( !bAlreadyAddedManagerOutput )\n"\
    "                {\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszStringInstance + \"fade_out:Fade\" );\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszStringInstance + \"template_movie_level_transition:ForceSpawn::0.6\" );\n"\
    "\n"\
    "                    // Hacky workaround for the fact that delays do not work with RunScriptCode (We do this so that we can have a nice fade on level exit)\n"\
    "                    Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"p2_coop_srm_transition_standard_rl\" );\n"\
    "                    if ( IsCalibration() )\n"\
    "                    {\n"\
    "                        EntFire( \"p2_coop_srm_transition_standard_rl\", \"AddOutput\", \"OnTrigger \" + pszStringInstance + \"@command:Command:changelevel mp_coop_lobby_3:2.0\" );\n"\
    "                        EntFire( \"p2_coop_srm_transition_standard_rl\", \"AddOutput\", \"OnTrigger \" + pszStringInstance + \"@command:Command:changelevel mp_coop_lobby_2:2.1\" ); // Backup\n"\
    "                    }\n"\
    "                    else\n"\
    "                    {\n"\
    "                        EntFire( \"p2_coop_srm_transition_standard_rl\", \"AddOutput\", \"OnTrigger \" + pszStringInstance + \"transition_script:RunScriptCode:TransitionFromMap();\" );\n"\
    "                    }\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue p2_coop_srm_transition_standard_rl:Trigger::0.25\" );\n"\
    "\n"\
    "                    bAlreadyAddedManagerOutput = true; // For cleanliness, we don't want to add this output multiple times in standard levels\n"\
    "                }\n"\
    "            }\n"\
    "        }\n"\
    "        else\n"\
    "        {\n"\
    "            // Speed up whichever logic_relay and logic_coop_manager belong to each other (This allows maps with multiple checkpoints to work properly)\n"\
    "            local iStringIndexMatchCheckpoint = pszManagerTargetName.find( \"coopman_airlock_success\" );\n"\
    "            if ( iStringIndexMatchCheckpoint != null )\n"\
    "            {\n"\
    "                local pszSlicedManagerName = pszManagerTargetName.slice( 0, iStringIndexMatchCheckpoint );\n"\
    "                local pszSlicedRelayName = pszRelayTargetName.slice( 0, iStringIndexMatchCheckpoint );\n"\
    "                if ( pszSlicedManagerName != null && pszSlicedRelayName != null && pszSlicedManagerName == pszSlicedRelayName )\n"\
    "                {\n"\
    "                    EntFire( pszManagerTargetName, \"AddOutput\", \"OnChangeToAllTrue \" + pszRelayTargetName + \":Trigger::0.3\" );\n"\
    "                }\n"\
    "            }\n"\
    "        }\n"\
    "    }\n"\
    "}\n"\
    "\n"\
    "//-----------------------------------------------------------------------------\n"\
    "// Helper funcs\n"\
    "//-----------------------------------------------------------------------------\n"\
    "local FastFall = function( pszBlueTriggerName, pszRedTriggerName )\n"\
    "{\n"\
    "    // Make players fall faster\n"\
    "    EntFire( pszBlueTriggerName, \"AddOutput\", \"OnEndTouchBluePlayer !activator:AddOutput:basevelocity 0 0 -1000\" );\n"\
    "    EntFire( pszRedTriggerName, \"AddOutput\", \"OnEndTouchOrangePlayer !activator:AddOutput:basevelocity 0 0 -1000\" );\n"\
    "}\n"\
    "\n"\
    "local AddDiscTransition = function( vecTriggerx, vecTriggery, vecTriggerz )\n"\
    "{\n"\
    "    // Make disc transition the map\n"\
    "    EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( vecTriggerx, vecTriggery, vecTriggerz ), 3 ), \"AddOutput\", \"OnStartTouch transition_fadeout_1:Fade\", 0, null, null );\n"\
    "    EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( vecTriggerx, vecTriggery, vecTriggerz ), 3 ), \"AddOutput\", \"OnStartTouch template_movie_level_transition:ForceSpawn::1\", 0, null, null );\n"\
    "\n"\
    "    // Hacky workaround for the fact that delays do not work with RunScriptCode (We do this so that we can have a nice fade on level exit)\n"\
    "    Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"p2_coop_srm_transition_rl\" );\n"\
        "    EntFireByHandle( Entities.FindByName( null, \"p2_coop_srm_transition_rl\" ), \"AddOutput\", \"OnTrigger transition_script:RunScriptCode:SaveMPStatsData()\", 0, null, null );\n"\
        "    EntFireByHandle( Entities.FindByName( null, \"p2_coop_srm_transition_rl\" ), \"AddOutput\", \"OnTrigger transition_script:RunScriptCode:MarkMapComplete( \\\"\" + pszMapName + \"\\\" )\", 0, null, null );\n"\
        "    EntFireByHandle( Entities.FindByName( null, \"p2_coop_srm_transition_rl\" ), \"AddOutput\", \"OnTrigger transition_script:RunScriptCode:SetCameFromLastDLCMapFlag()\", 0, null, null );\n"\
        "    EntFireByHandle( Entities.FindByName( null, \"p2_coop_srm_transition_rl\" ), \"AddOutput\", \"OnTrigger @command:Command:changelevel mp_coop_lobby_3:1.0\", 0, null, null );\n"\
        "    EntFireByHandle( Entities.FindByName( null, \"p2_coop_srm_transition_rl\" ), \"AddOutput\", \"OnTrigger @command:Command:changelevel mp_coop_lobby_2:1.1\", 0, null, null ); // Backup\n"\
    "    EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( vecTriggerx, vecTriggery, vecTriggerz ), 3 ), \"AddOutput\", \"OnStartTouch p2_coop_srm_transition_rl:Trigger::1.60\", 0, null, null );\n"\
    "}\n"\
    "\n"\
    "local SpeedUpGel = function()\n"\
    "{\n"\
    "    // Get rid of the gel spread RNG on all maps\n"\
    "    for ( local pEnt; pEnt = Entities.FindByClassname( pEnt, \"info_paint_sprayer\" ); )\n"\
    "    {\n"\
    "        pEnt.__KeyValueFromString( \"blob_streak_percentage\", \"100\" );\n"\
    "        pEnt.__KeyValueFromString( \"min_streak_time\", \"2\" );\n"\
    "        pEnt.__KeyValueFromString( \"min_streak_speed_dampen\", \"1\" );\n"\
    "        pEnt.__KeyValueFromString( \"max_streak_speed_dampen\", \"1\" );\n"\
    "    }\n"\
    "}\n"\
    "\n"\
    "//-----------------------------------------------------------------------------\n"\
    "// Now attempt to filter out the correct map-specific code\n"\
    "// We sort it according to the course type and level name\n"\
    "//-----------------------------------------------------------------------------\n"\
    "if ( IsCalibration() )\n"\
    "{\n"\
    "    // Skip the video from playing on map loads\n"\
    "    EntFire( \"teleport_start\", \"Enable\" );\n"\
    "    EntFire( \"@relay_start_both_connected\", \"Disable\" );\n"\
    "\n"\
    "    // Skip the taunting area\n"\
    "    local pTauntTrigger = Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( -9952, -4384, 2965.64 ), 3 );\n"\
    "    EntFireByHandle( pTauntTrigger, \"AddOutput\", \"OnStartTouchAll relay_gesture_1_move_on:Trigger\", 0, null, null );\n"\
    "\n"\
    "    // Skip the starting dialogue and instead use the orange pinging one\n"\
    "    Entities.FindByClassnameNearest( \"trigger_once\", Vector( -10056, -4400, 2784 ), 3 ).Destroy();\n"\
    "\n"\
    "    // Skip the blue pinging area\n"\
    "    local pBluePingTrigger = Entities.FindByClassnameNearest( \"trigger_once\", Vector( -9984, -4384, 2072 ), 3 );\n"\
    "    EntFireByHandle( pBluePingTrigger, \"AddOutput\", \"OnTrigger relay_ping_1_move_on:Trigger\", 0, null, null );\n"\
    "\n"\
    "    // Catch up our state to enable Orange pinging the elements\n"\
    "    EntFire( \"@global_no_pinging_blue\", \"TurnOn\" ); // Blue cannot ping in this state, so just disable it from the start of the map\n"\
    "    local pRedPingTrigger = Entities.FindByClassnameNearest( \"trigger_once\", Vector( -9984, -4392, 1015 ), 3 );\n"\
    "    EntFireByHandle( pRedPingTrigger, \"AddOutput\", \"OnTrigger trigger_orange_ping_hint_2:Enable\", 0, null, null ); // Make the hint show\n"\
    "    EntFireByHandle( pRedPingTrigger, \"AddOutput\", \"OnTrigger relay_begin_ping_2:Trigger\", 0, null, null );\n"\
    "    EntFireByHandle( pRedPingTrigger, \"AddOutput\", \"OnTrigger script_ping_select_test:RunScriptCode:ORANGE_enabled = 1; SelectChoicesOrangeStart();\", 0, null, null ); // mp_coop_ping_select_test.nut (We do it this way so that Orange cannot ping the panels while falling down)\n"\
    "\n"\
    "    // Expose the two weapon_portalgun's faster and as soon as we get to them\n"\
    "    EntFire( \"player_1_portalgun_door_pedistal_removal_lvl2\", \"SetSpeed\", \"40\" ); // Red\n"\
    "    EntFire( \"player_2_portalgun_door_pedistal_removal_lvl2\", \"SetSpeed\", \"40\" ); // Blue\n"\
    "    pGunCalibrationTrigger <- Entities.FindByClassnameNearest( \"trigger_once\", Vector( -10004, -4400, 300 ), 3 );\n"\
    "    local AddCalibrationTriggerOutput = function( pszOutput ) { EntFireByHandle( pGunCalibrationTrigger, \"AddOutput\", \"OnTrigger \" + pszOutput, 0, null, null ); }\n"\
    "    AddCalibrationTriggerOutput( \"player_1_portalgun_door_pedistal_removal_lvl2:Close\" );\n"\
    "    AddCalibrationTriggerOutput( \"player_2_portalgun_door_pedistal_removal_lvl2:Close\" );\n"\
    "    AddCalibrationTriggerOutput( \"ping_detector_blue:Enable::1.5\" );\n"\
    "    AddCalibrationTriggerOutput( \"ping_detector_orange:Enable::1.5\" );\n"\
    "    AddCalibrationTriggerOutput( \"tank_portalgun_orange:Activate::1.5\" );\n"\
    "    AddCalibrationTriggerOutput( \"tank_portalgun_blue:Activate::1.5\" );\n"\
    "    AddCalibrationTriggerOutput( \"snd_gun_steam:PlaySound\" );\n"\
    "    AddCalibrationTriggerOutput( \"snd_gun_steam:Kill::1.00\" );\n"\
    "    AddCalibrationTriggerOutput( \"steam_guns:Start\" );\n"\
    "    AddCalibrationTriggerOutput( \"steam_guns:Stop::1.00\" );\n"\
    "    AddCalibrationTriggerOutput( \"steam_guns:Kill::1.00\" );\n"\
    "    delete pGunCalibrationTrigger;\n"\
    "\n"\
    "    // For an inbounds run, allow the final button to enable the team camera immediately\n"\
    "    EntFireByHandle( Entities.FindByName( null, \"relay_robots_meet\" ), \"AddOutput\", \"OnTrigger team_trigger_door:Enable\", 0, null, null );\n"\
    "\n"\
    "    // Silently award the taunt for highFive\n"\
    "    Entities.FindByName( null, \"@relay_come_together\" ).Destroy();\n"\
    "    Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"@relay_come_together\" );\n"\
    "    EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger team_trigger_door:Kill\" );\n"\
    "    EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:RespondToTaunt(1)\" );\n"\
    "    EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt highFive 1;\" );\n"\
    "    EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger ach_hi_five:FireEvent::3.00\" );\n"\
    "}\n"\
    "else if ( IsHub() )\n"\
    "{\n"\
    "    // Remove starting tube blocker\n"\
    "    EntFire( \"relay_elevator_open\", \"Trigger\" );\n"\
    "    EntFire( \"relay_elevator_open\", \"Disable\" );\n"\
    "    EntFire( \"relay_elevator_open1\", \"Trigger\" );\n"\
    "    EntFire( \"relay_elevator_open1\", \"Disable\" );\n"\
    "    EntFire( \"blocker_blue\", \"Kill\" );\n"\
    "    EntFire( \"blocker_orange\", \"Kill\" );\n"\
    "    Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( 3924.000000, 3352.000000, -480.000000 ), 1 ).Destroy();\n"\
    "    Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( 3924.000000, 3496.000000, -480.000000 ), 1 ).Destroy();\n"\
    "\n"\
    "    // Open C1-C4 hub doors\n"\
    "    // We do it this way since it is too early to tell if the game is splitscreen or not\n"\
    "    function OpenCourseDoors()\n"\
    "    {\n"\
    "        Entities.FindByName( null, \"trigger_run_script\" ).DisconnectOutput( \"OnStartTouchBluePlayer\", \"OpenCourseDoors\" ); // We only need to fire this function once\n"\
    "\n"\
    "        // Keep DLC door open after the DLC course is selected\n"\
    "        local pCoopScreen = Entities.FindByName( null, \"coopman_screen\" );\n"\
    "        if ( pCoopScreen )\n"\
    "        {\n"\
    "            pCoopScreen.Destroy();\n"\
    "        }\n"\
    "        local pCoopDoor = Entities.FindByName( null, \"track6-compare_coop_door_open\" );\n"\
    "        if ( pCoopDoor )\n"\
    "        {\n"\
    "            pCoopDoor.Destroy();\n"\
    "        }\n"\
    "        EntFire( \"track6-track_door_open_trigger\", \"AddOutput\", \"OnTrigger track6-prop_door_hall:Open:::1\" );\n"\
    "        EntFire( \"script_dlc_screen_logic\", \"RunScriptCode\", \"OpenScreenStart()\" );\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( 3635.199951, -1184.000000, -456.000000 ), 1 ), \"AddOutput\", \"OnTrigger hint_dlc:ShowHint\", 0, null, null );\n"\
    "\n"\
    "        if ( IsLocalSplitScreen() ) // Splitscreen has everything unlocked\n"\
    "        {\n"\
    "            for ( local iCourse = 1; iCourse < 5; iCourse++ ) // (5 and 6 taken care of outside of this function)\n"\
    "            {\n"\
    "                // Keep this particular track's door open\n"\
    "                EntFire( \"track\" + iCourse.tostring() + \"-math_coop_door_open\", \"Add\", \"1\" );\n"\
    "                EntFire( \"track\" + iCourse.tostring() + \"-math_coop_door_open\", \"Kill\" );\n"\
    "            }\n"\
    "\n"\
    "            return;\n"\
    "        }\n"\
    "\n"\
    "        // We must check the progress and unlock courses accordingly\n"\
    "        local iHighestActiveCourse = GetHighestActiveBranch();\n"\
    "        for ( local i = 1; i <= iHighestActiveCourse; i++ )\n"\
    "        {\n"\
    "            if ( i >= 6 ) { continue; } // 6 is handled much earlier and also applies to splitscreen\n"\
    "\n"\
    "            if ( i == 5 ) // Keep this particular track's door open\n"\
    "            {\n"\
    "                // If we aren't newbies, check if this is a reveal for Course 5 and speed it up\n"\
    "                if ( !( GetGladosSpokenFlags( 2 ) & ( 1 << 4 ) ) ) // Didn't say \"Congratulations on completing the Aperture Science standard cooperative testing courses.\"\n"\
    "                {\n"\
    "                    // Treat it as if the dialogue was spoken and we are returning back to the hub\n"\
    "                    EntFire( \"counter_return_to_hub\", \"SetValue\", \"5\" );\n"\
    "                    EntFire( \"counter_choose_course\", \"SetValue\", \"0\" ); // Set it back to 0\n"\
    "                    // AddGladosSpokenFlags( 2, ( 1 << 4 ) ); // Done through Valve's VScript (mp_coop_lobby.nut)\n"\
    "                }\n"\
    "            }\n"\
    "            else\n"\
    "            {\n"\
    "                // Note that C1 should be open if we have no progress whatsoever in any other levels\n"\
    "                EntFire( \"track\" + i.tostring() + \"-math_coop_door_open\", \"Add\", \"1\" );\n"\
    "                EntFire( \"track\" + i.tostring() + \"-math_coop_door_open\", \"Kill\" );\n"\
    "            }\n"\
    "        }\n"\
    "    }\n"\
    "    Entities.FindByName( null, \"trigger_run_script\" ).ConnectOutput( \"OnStartTouchAll\", \"OpenCourseDoors\" );\n"\
    "\n"\
    "    // Doesn't really matter in local games, but we run this anyway just for cleanliness\n"\
    "    // Remake the unlock course relays for courses 2 and 3 so that the taunts are silently awarded\n"\
    "    Entities.FindByName( null, \"relay_glados_fling\" ).Destroy();\n"\
    "    Entities.FindByName( null, \"relay_glados_bridge\" ).Destroy();\n"\
    "    Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"relay_glados_fling\" );\n"\
    "    Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"relay_glados_bridge\" );\n"\
    "    Entities.FindByName( null, \"relay_glados_fling\" ).SetOrigin( Vector( 5112.000000, 3848.000000, -472.000000 ) );\n"\
    "    Entities.FindByName( null, \"relay_glados_bridge\" ).SetOrigin( Vector( 5112.000000, 3832.000000, -472.000000 ) );\n"\
    "    EntFire( \"relay_glados_fling\", \"Disable\" );\n"\
    "    EntFire( \"relay_glados_bridge\", \"Disable\" );\n"\
    "    EntFire( \"relay_glados_fling\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:CoopHubTrack02()\" );\n"\
    "    EntFire( \"relay_glados_fling\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt laugh 1\" );\n"\
    "    EntFire( \"relay_glados_bridge\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:CoopHubTrack03()\" );\n"\
    "    EntFire( \"relay_glados_bridge\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt teamtease 1\" );\n"\
    "\n"\
    "    // Speed up C5 entry\n"\
    "    Entities.FindByName( null, \"trigger_paint_door_open\" ).Destroy();\n"\
    "    // EntFire( \"catapult_paint\", \"AddOutput\", \"playerSpeed 2850\" ); // TODO: Doesn't work as a network client??\n"\
    "    EntFire( \"catapult_paint\", \"AddOutput\", \"OnCatapulted AutoInstance1-relay_dooropen:Trigger\" );\n"\
    "    EntFire( \"track5-man_fall\", \"AddOutput\", \"OnChangeToAllTrue track5-template_movie_level_transition:ForceSpawn::1.00\" );\n"\
    "    EntFire( \"track5-man_fall\", \"AddOutput\", \"OnChangeToAllTrue track5-gladosendoflevelrelay:Trigger::1.50\" );\n"\
    "    EntFire( \"track5-man_fall\", \"AddOutput\", \"OnChangeToAllTrue track5-fade_exit_level:Fade\" );\n"\
    "\n"\
    "    // Exists in only the new lobby. Speed up the DLC course area\n"\
    "    if ( pszMapName == \"mp_coop_lobby_3\" )\n"\
    "    {\n"\
    "        // Reveal faster\n"\
    "        if ( !GetHaveSeenDLCTubesReveal() )\n"\
    "        {\n"\
    "            EntFire( \"dlc_transport_sheath\", \"SetSpeed\", \"500\" );\n"\
    "        }\n"\
    "\n"\
    "        // Skip the viewcontrol for the tube travel to the DLC area (Note that Blue will always end up on the left side, while Orange ends up on the right side)\n"\
    "        Entities.FindByName( null, \"blue_hub_to_dlc_cam\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"blue_hub_to_dlc_fade\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"orange_hub_to_dlc_cam\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"orange_hub_to_dlc_fade\" ).Destroy();\n"\
    "        local pFirstTeleportTrigger = Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( 2112, -1244, -832 ), 3 );\n"\
    "        EntFireByHandle( pFirstTeleportTrigger, \"AddOutput\", \"OnStartTouchBluePlayer left_hub_to_dlc_destination:TeleportEntity:!activator\", 0, null, null );\n"\
    "        EntFireByHandle( pFirstTeleportTrigger, \"AddOutput\", \"OnStartTouchOrangePlayer right_hub_to_dlc_destination:TeleportEntity:!activator\", 0, null, null );\n"\
    "\n"\
    "        // Skip viewcontrol back to hub (Note that unlike normal spawns, Blue ends up on the left side and Orange ends up on left)\n"\
    "        Entities.FindByName( null, \"blue_dlc_to_hub_cam\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"blue_dlc_to_hub_fade\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"orange_dlc_to_hub_cam\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"orange_dlc_to_hub_fade\" ).Destroy();\n"\
    "        local pSecondTeleportTrigger = Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( 2432, -1244, -832 ), 3 );\n"\
    "        EntFireByHandle( pSecondTeleportTrigger, \"AddOutput\", \"OnStartTouchBluePlayer left_dlc_to_hub_destination:TeleportEntity:!activator\", 0, null, null );\n"\
    "        EntFireByHandle( pSecondTeleportTrigger, \"AddOutput\", \"OnStartTouchOrangePlayer right_dlc_to_hub_destination:TeleportEntity:!activator\", 0, null, null );\n"\
    "\n"\
    "        // Offset the teleport so that Orange's velocity remains consistent with Blue's\n"\
    "        Entities.FindByName( null, \"right_hub_to_dlc_destination\" ).SetAbsOrigin( Vector( 3200, -1232, 1688 ) );\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse1() )\n"\
    "{\n"\
    "    if ( pszMapName == \"mp_coop_rat_maze\" )\n"\
    "    {\n"\
    "        // Prevent taunt at start of map, but also trigger the dialogue at the right time and conditions\n"\
    "        Entities.FindByName( null, \"coop_man_give_taunt\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_coop_manager\" ).__KeyValueFromString( \"targetname\", \"p2coop_taunt_manager\" );\n"\
    "        EntFire( \"p2coop_taunt_manager\", \"AddOutput\", \"OnChangeToAllTrue @glados:RunScriptCode:RespondToTaunt(3);\" );\n"\
    "        EntFire( \"p2coop_taunt_manager\", \"AddOutput\", \"OnChangeToAllTrue @command:Command:mp_earn_taunt rps 1;\" );\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( -176, -656, -632 ), 3 ), \"AddOutput\", \"OnStartTouchBluePlayer p2coop_taunt_manager:SetStateBTrue\", 0, null, null );\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_playerteam\", Vector( -176, -656, -632 ), 3 ), \"AddOutput\", \"OnStartTouchOrangePlayer p2coop_taunt_manager:SetStateATrue\", 0, null, null );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_teambts\" )\n"\
    "    {\n"\
    "        FastFall( \"InstanceAuto15-blue_dropper-cube_dropper_droptrigger_bottom\", \"InstanceAuto15-red_dropper-cube_dropper_droptrigger_bottom\" );\n"\
    "\n"\
    "        AddDiscTransition( 1164, -3051, 5682 );\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse2() )\n"\
    "{\n"\
    "    if ( pszMapName == \"mp_coop_catapult_1\" )\n"\
    "    {\n"\
    "        // Silently award the taunt for robotDance\n"\
    "        Entities.FindByName( null, \"@relay_exit_door_opened\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"@relay_exit_door_opened\" );\n"\
    "        EntFire( \"@relay_exit_door_opened\", \"AddOutput\", \"OnTrigger team_trigger_door:Kill\" );\n"\
    "        EntFire( \"@relay_exit_door_opened\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt robotDance 1;\" );\n"\
    "        EntFire( \"@relay_exit_door_opened\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:RespondToTaunt(5)\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_multifling_1\" )\n"\
    "    {\n"\
    "        // Make catapults in second room faster\n"\
    "        EntFire( \"catapult2a\", \"AddOutput\", \"playerSpeed 2000\" );\n"\
    "        EntFire( \"catapult2a\", \"AddOutput\", \"physicsSpeed 1800\" );\n"\
    "        EntFire( \"catapult2a1\", \"AddOutput\", \"playerSpeed 800\" );\n"\
    "        EntFire( \"catapult2a1\", \"AddOutput\", \"physicsSpeed 600\" );\n"\
    "        EntFire( \"catapult2a2\", \"AddOutput\", \"playerSpeed 2300\" );\n"\
    "        EntFire( \"catapult2a2\", \"AddOutput\", \"physicsSpeed 1000\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_fan\" )\n"\
    "    {\n"\
    "        FastFall( \"blue_dropper-blue_dropper-cube_dropper_droptrigger_bottom\", \"blue_dropper-red_dropper-cube_dropper_droptrigger_bottom\" );\n"\
    "\n"\
    "        // Make fan faster at slowing down, but not speeding up\n"\
    "        EntFire( \"catcher\", \"AddOutput\", \"OnPowered brush_fan:AddOutput:fanfriction 90\" );\n"\
    "        EntFire( \"catcher\", \"AddOutput\", \"OnUnPowered brush_fan:AddOutput:fanfriction 18\" );\n"\
    "\n"\
    "        AddDiscTransition( -259.8, 881.89, 231.28 );\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse3() )\n"\
    "{\n"\
    "    if ( pszMapName == \"mp_coop_wall_5\" )\n"\
    "    {\n"\
    "        FastFall( \"blue_dropper-cube_dropper_droptrigger_bottom\", \"red_dropper-cube_dropper_droptrigger_bottom\" ); // Red team can strafe on the word \"official\"\n"\
    "\n"\
    "        // Silently award the taunt for teamhug\n"\
    "        Entities.FindByName( null, \"@relay_come_together\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"@relay_come_together\" );\n"\
    "        EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:RespondToTaunt(8)\" );\n"\
    "        EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt teamhug 1;\" );\n"\
    "        EntFire( \"@relay_come_together\", \"AddOutput\", \"OnTrigger success_detector:Kill\" );\n"\
    "\n"\
    "        AddDiscTransition( 2133.35, -1587.71, 297.86 );\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse4() )\n"\
    "{\n"\
    "    if ( pszMapName == \"mp_coop_tbeam_redirect\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"prop_tractor_beam\", Vector( 352.000000, -416.000000, 512.000000 ), 1 ), \"SetLinearForce\", \"400\", 0, null, null );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_drill\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"prop_tractor_beam\", Vector( 736.000000, 544.000000, 576.000000 ), 1 ), \"SetLinearForce\", \"350\", 0, null, null );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_catch_grind_1\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tractorbeam_emitter\", \"SetLinearForce\", \"500\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_laser_1\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tbeam_ride\", \"SetLinearForce\", \"400\" );\n"\
    "\n"\
    "        // Prevent taunt at start of map, but also trigger the dialogue at the right time and conditions\n"\
    "        Entities.FindByName( null, \"@relay_grant_taunt\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"@relay_grant_taunt\" );\n"\
    "        EntFire( \"@relay_grant_taunt\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:RespondToTaunt(9);\" );\n"\
    "        EntFire( \"@relay_grant_taunt\", \"AddOutput\", \"OnTrigger @command:Command:mp_earn_taunt trickfire 1;\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_polarity\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tbeam\", \"SetLinearForce\", \"600\" );\n"\
    "        EntFire( \"button_1_pressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1_unpressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed tbeam:SetLinearForce:-600\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed toggle_indicators:SetTextureIndex:1\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed tbeam:SetLinearForce:600\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed toggle_indicators:SetTextureIndex:0\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_polarity2\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tbeam\", \"SetLinearForce\", \"600\" );\n"\
    "        EntFire( \"button_1_pressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1_unpressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1\", \"AddOutput\", \"OnPressed tbeam:SetLinearForce:-600\" );\n"\
    "        EntFire( \"button_1\", \"AddOutput\", \"OnPressed toggle_indicators:SetTextureIndex:1\" );\n"\
    "        EntFire( \"button_1\", \"AddOutput\", \"OnUnPressed tbeam:SetLinearForce:600\" );\n"\
    "        EntFire( \"button_1\", \"AddOutput\", \"OnUnPressed toggle_indicators:SetTextureIndex:0\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_polarity3\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tbeam\", \"SetLinearForce\", \"600\" );\n"\
    "        EntFire( \"button_1_pressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1_unpressed\", \"Disable\", \"\", 1.5 ); // A logic_auto invokes this, so give ample time for it to trigger\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed tbeam:SetLinearForce:-600\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed toggle_indicators:SetTextureIndex:1\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed fizzler1_enable_rl:Trigger\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed tbeam:SetLinearForce:600\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed toggle_indicators:SetTextureIndex:0\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed fizzler1_disable_rl:Trigger\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_maze\" )\n"\
    "    {\n"\
    "        // Make tractor beam only slightly faster. We want to make sure that it's not too fast for doing the actual maze\n"\
    "        EntFire( \"tbeam\", \"SetLinearForce\", \"300\" );\n"\
    "        EntFire( \"button_1_pressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1_unpressed\", \"Disable\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed tbeam:SetLinearForce:-300\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnPressed toggle_indicators:SetTextureIndex:1\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed tbeam:SetLinearForce:300\" );\n"\
    "        EntFire( \"button_1-button\", \"AddOutput\", \"OnUnPressed toggle_indicators:SetTextureIndex:0\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_tbeam_end\" )\n"\
    "    {\n"\
    "        // Make both tractor beams faster\n"\
    "        EntFire( \"tractorbeam_emitter\", \"SetLinearForce\", \"300\" );\n"\
    "\n"\
    "        FastFall( \"InstanceAuto68-blue_dropper-cube_dropper_droptrigger_bottom\", \"InstanceAuto68-red_dropper-cube_dropper_droptrigger_bottom\" );\n"\
    "\n"\
    "        AddDiscTransition( 2572.99, -907.71, 130 );\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse5() )\n"\
    "{\n"\
    "    SpeedUpGel(); // Gel exists in course 5\n"\
    "\n"\
    "    if ( pszMapName == \"mp_coop_paint_come_along\" )\n"\
    "    {\n"\
    "        // Speed up both of the lifts\n"\
    "        EntFire( \"lift3_rm5\", \"SetSpeed\", \"150\" );\n"\
    "        EntFire( \"lift2\", \"SetSpeed\", \"150\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_redirect\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster\n"\
    "        EntFire( \"tbeam\", \"SetLinearForce\", \"400\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_red_racer\" )\n"\
    "    {\n"\
    "        // Make catapult trigger faster\n"\
    "        EntFire( \"catapult_launch_exit\", \"AddOutput\", \"lowerThreshold 0.575\" );\n"\
    "        EntFire( \"catapult_launch_exit\", \"AddOutput\", \"upperThreshold 5\" );\n"\
    "        EntFire( \"catapult_launch_exit\", \"AddOutput\", \"playerSpeed 1800\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_longjump_intro\" )\n"\
    "    {\n"\
    "        // Speed up spawning door\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( 224, -7366, 948 ), 1 ), \"AddOutput\", \"OnTrigger AutoInstance1-door_open:Trigger\", 0, null, null );\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( 224, -7366, 948 ), 1 ), \"AddOutput\", \"OnTrigger AutoInstance1-door_open:Disable::0.3\", 0, null, null );\n"\
    "        EntFire( \"AutoInstance1-door_upper\", \"SetSpeed\", \"150\" );\n"\
    "        EntFire( \"AutoInstance1-door_lower\", \"SetSpeed\", \"150\" );\n"\
    "\n"\
    "        // Prevent pushback with gagged turret\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"npc_portal_turret_floor\", Vector( 255.347458, -6304.168945, 896.335876 ), 1 ), \"AddOutput\", \"DamageForce 0\", 0, null, null );\n"\
    "\n"\
    "        // Add enough speed to a paint sprayer that allows for the challenge mode ending strat without failure\n"\
    "        EntFire( \"paint_speed_2\", \"AddOutput\", \"min_speed 1200\" );\n"\
    "\n"\
    "        // Make gel spread faster after pressing both of the final levers\n"\
    "        // We need to remake a trigger's outputs here with different timings\n"\
    "        Entities.FindByName( null, \"trigger_bluedoor\" ).Destroy();\n"\
    "\n"\
    "        // Open the next area\n"\
    "        pTargetCoopManager <- Entities.FindByName( null, \"coopman_turret_door1\" );\n"\
    "        local AddLongJumpOutput = function( pszOutput ) { EntFireByHandle( pTargetCoopManager, \"AddOutput\", \"OnChangeToAllTrue \" + pszOutput, 0, null, null ); }\n"\
    "        AddLongJumpOutput( \"chamber_exit_a-proxy:OnProxyRelay1:::1\" );\n"\
    "        AddLongJumpOutput( \"chamber_exit_a-proxy:OnProxyRelay1:::1\" );\n"\
    "        AddLongJumpOutput( \"turrets:Enable::1.00:1\" );\n"\
    "        AddLongJumpOutput( \"snd_alarm:StopSound::2.00:1\" );\n"\
    "        AddLongJumpOutput( \"snd_paint_speed_mid:PlaySound:::1\" );\n"\
    "        AddLongJumpOutput( \"snd_paint_speed_mid_end:PlaySound::3.00:1\" );\n"\
    "\n"\
    "        // Paint\n"\
    "        EntFire( \"paint_speed_mid\", \"AddOutput\", \"blobs_per_second 50\" ); // Dispenses slowly by default\n"\
    "        Entities.FindByName( null, \"paint_speed_mid\" ).SetAngles( 49, 90, 90 ); // Push it up so that there isn't a gap in the sheet of gel\n"\
    "        EntFire( \"paint_speed_mid_main\", \"AddOutput\", \"blobs_per_second 50\" ); // Dispenses slowly by default\n"\
    "        AddLongJumpOutput( \"part_paint_speed_1:Start:::1\" );\n"\
    "        AddLongJumpOutput( \"part_paint_speed_1:Stop::1.00:1\" );\n"\
    "        AddLongJumpOutput( \"part_paint_speed_2:Start:::1\" );\n"\
    "        AddLongJumpOutput( \"part_paint_speed_2:Stop::1.00:1\" );\n"\
    "        AddLongJumpOutput( \"paint_speed_mid:Start:::1\" );\n"\
    "        AddLongJumpOutput( \"paint_speed_mid:Stop::1.00:1\" );\n"\
    "        AddLongJumpOutput( \"paint_speed_mid_main:Start:::1\" );\n"\
    "        AddLongJumpOutput( \"paint_speed_mid_main:Stop::1.00:1\" );\n"\
    "        delete pTargetCoopManager;\n"\
    "    }\n"\
    "}\n"\
    "else if ( IsCourse6() )\n"\
    "{\n"\
    "    SpeedUpGel(); // Gel exists in course 6\n"\
    "\n"\
    "    if ( pszMapName == \"mp_coop_tripleaxis\" )\n"\
    "    {\n"\
    "        // Make crusher trigger faster\n"\
    "        Entities.FindByName( null, \"crusher_sequence_start_rl\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"crusher_sequence_start_rl\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger crusher:SetAnimation:Smash_in\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger gibshooter_0:Shoot::0.60\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger breakpanel_0_shake:StartShake::0.60\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger smashed_panels_0:Enable::0.60\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger breakpanel_0:Break::0.60\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger crusher_track_wav:PlaySound::0.60\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger @glados:RunScriptCode:DLC1_mp_coop_tripleaxis_intro():1.25\" ); // The delay works here...\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger crusher:SetAnimation:Smash_out:1.25\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger crusher_blocker:Kill::2.50\" );\n"\
    "        EntFire( \"crusher_sequence_start_rl\", \"AddOutput\", \"OnTrigger trigger_hide_crusher:Enable::2.50\" );\n"\
    "\n"\
    "        Entities.FindByClassnameNearest( \"trigger_once\", Vector( 1760.000000, 2888.000000, 128.000000 ), 1 ).Destroy();\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( 1036.000000, 3072.000000, 168.000000 ), 1 ), \"AddOutput\", \"OnStartTouch crusher_sequence_start_rl:Trigger\", 0, null, null );\n"\
    "\n"\
    "        // Make ending platform rise twice as fast\n"\
    "        EntFire( \"lift_1_door_ride\", \"SetSpeed\", \"300\" );\n"\
    "\n"\
    "        // Make tractor beam faster\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"prop_tractor_beam\", Vector( 1440.000000, 1727.969971, 192.000000 ), 1 ), \"SetLinearForce\", \"500\", 0, null, null );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_catapult_catch\" )\n"\
    "    {\n"\
    "        // Make catapults faster\n"\
    "        EntFire( \"catapult_01\", \"AddOutput\", \"playerSpeed 1200\" );\n"\
    "        EntFire( \"catapult_01\", \"AddOutput\", \"physicsSpeed 1200\" );\n"\
    "        EntFire( \"catapult_02\", \"AddOutput\", \"playerSpeed 1200\" );\n"\
    "        EntFire( \"catapult_02\", \"AddOutput\", \"physicsSpeed 1200\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_2paints_1bridge\" )\n"\
    "    {\n"\
    "        // Make paint sprayer in the first room distill faster\n"\
    "        EntFire( \"paint_sprayer_blue_1\", \"AddOutput\", \"max_speed 600\" );\n"\
    "        EntFire( \"paint_sprayer_blue_1\", \"AddOutput\", \"maxblobcount 60\" );\n"\
    "        EntFire( \"paint_sprayer_blue_1\", \"AddOutput\", \"blobs_per_second 30\" );\n"\
    "\n"\
    "        // Make checkpoint door stop being weird upon first entry\n"\
    "        Entities.FindByName( null, \"entry_airlock-trig_diable_entry\" ).Destroy();\n"\
    "\n"\
    "        // Make second room paint sprayers always stay on once we get to them\n"\
    "        Entities.FindByName( null, \"paint_timer_blue\" ).Destroy();\n"\
    "        Entities.FindByName( null, \"paint_timer_orange\" ).Destroy();\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( 1472.000000, 1650.000000, 256.000000 ), 1 ), \"AddOutput\", \"OnTrigger paint_sprayer_blue:Start\", 0, null, null );\n"\
    "        EntFireByHandle( Entities.FindByClassnameNearest( \"trigger_once\", Vector( 1472.000000, 1650.000000, 256.000000 ), 1 ), \"AddOutput\", \"OnTrigger paint_sprayer_orange:Start\", 0, null, null );\n"\
    "\n"\
    "        // Special case for broken disassemblers fast transition\n"\
    "        EntFire( \"InstanceAuto9-start_malfunction_rl\", \"Disable\" );\n"\
    "        EntFire( \"InstanceAuto9-coopman_exit_level\", \"AddOutput\", \"OnChangeToAllTrue @malfunction_end_rl:Trigger\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_conversion\" )\n"\
    "    {\n"\
    "        // Make breaker room faster\n"\
    "        Entities.FindByName( null, \"disassembler_start_relay\" ).Destroy();\n"\
    "        Entities.CreateByClassname( \"logic_relay\" ).__KeyValueFromString( \"targetname\", \"disassembler_start_relay\" );\n"\
    "        pDisassemblerRelay <- Entities.FindByName( null, \"disassembler_start_relay\" );\n"\
    "        local AddRelayOutput = function( pszOutput ) { EntFireByHandle( pDisassemblerRelay, \"AddOutput\", \"OnTrigger \" + pszOutput, 0, null, null ); }\n"\
    "        AddRelayOutput( \"disassembler_door_2_blocker_disable_relay:CancelPending\" );\n"\
    "        AddRelayOutput( \"disassembler_door_1_blocker_disable_relay:CancelPending\" );\n"\
    "        AddRelayOutput( \"disassembler_door_1_blocker_disable_relay:Kill\" );\n"\
    "        AddRelayOutput( \"disassembler_door_2_blocker_disable_relay:Kill\" );\n"\
    "        AddRelayOutput( \"disassembler_2_door_2:Close\" );\n"\
    "        AddRelayOutput( \"disassembler_2_door_1:Close\" );\n"\
    "        AddRelayOutput( \"disassembler_1_door_2:Close\" );\n"\
    "        AddRelayOutput( \"disassembler_1_door_1:Close\" );\n"\
    "        AddRelayOutput( \"@glados:RunScriptCode:DLC1_2Paints_1Bridge_PowerCycle()\" );\n"\
    "        AddRelayOutput( \"disassembler_2_door_antiblocker:Disable\" );\n"\
    "        AddRelayOutput( \"disassembler_1_door_antiblocker:Disable\" );\n"\
    "        AddRelayOutput( \"disassembler_1_steam:Start\" );\n"\
    "        AddRelayOutput( \"disassembler_1_steam_fx:PlaySound\" );\n"\
    "        AddRelayOutput( \"disassembler_2_steam:Start\" );\n"\
    "        AddRelayOutput( \"disassembler_2_steam_fx:PlaySound\" );\n"\
    "        AddRelayOutput( \"disassembler_start_clunk:PlaySound::0.50\" );\n"\
    "        AddRelayOutput( \"disassembler_dropper_props:SetAnimation:item_dropper_open:1.50\" );\n"\
    "        AddRelayOutput( \"disassembler_dropper_props_sound:PlaySound::1.50\" );\n"\
    "        AddRelayOutput( \"disassembler_glow_sprites:Color:0 255 0:1.50\" );\n"\
    "        AddRelayOutput( \"disassembler_start_ding:PlaySound::1.50\" );\n"\
    "        delete pDisassemblerRelay;\n"\
    "\n"\
    "        // Make conversion gel in the final room distill faster\n"\
    "        EntFire( \"paint_sprayer_white\", \"AddOutput\", \"maxblobcount 60\" );\n"\
    "        EntFire( \"paint_sprayer_white\", \"AddOutput\", \"blobs_per_second 20\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_laser_tbeam\" )\n"\
    "    {\n"\
    "        // Make tractor beam faster (Hack: There's no real good way to do it for this map...)\n"\
    "        Entities.FindByName( null, \"tbeam\" ).__KeyValueFromString( \"targetname\", \"tbeam_renamed\" )\n"\
    "        EntFire( \"tbeam_renamed\", \"SetLinearForce\", \"500\" );\n"\
    "        local pCatcher = Entities.FindByClassnameNearest( \"prop_laser_catcher\", Vector( -144.000000, 320.000000, -480.000000 ), 1 );\n"\
    "        EntFireByHandle( pCatcher, \"AddOutput\", \"OnPowered tbeam_renamed:SetLinearForce:-500\", 0, null, null );\n"\
    "        EntFireByHandle( pCatcher, \"AddOutput\", \"OnUnpowered tbeam_renamed:SetLinearForce:500\", 0, null, null );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_rat_maze\" )\n"\
    "    {\n"\
    "        // Make paint sprayers always stay on\n"\
    "        Entities.FindByName( null, \"bounce_paint_timer\" ).Destroy();\n"\
    "        EntFire( \"bounce_paint_sprayer\", \"Start\" );\n"\
    "        EntFire( \"speed_paint_sprayer\", \"Start\" );\n"\
    "    }\n"\
    "    else if ( pszMapName == \"mp_coop_paint_crazy_box\" )\n"\
    "    {\n"\
    "        // Prevent night vision view\n"\
    "        EntFire( \"bts_nightvision_fade_to_black\", \"Disable\" );\n"\
    "        EntFire( \"bts_nightvision_enable\", \"Disable\" );\n"\
    "        EntFire( \"bts_nightvision_fade_to_white\", \"Disable\" );\n"\
    "        EntFire( \"bts_nightvision_disable\", \"Disable\" );\n"\
    "        EntFire( \"team_door-exit_door-trigger_glados_exit_door\", \"AddOutput\", \"OnStartTouch bts_wall_undamaged:Disable\" );\n"\
    "        EntFire( \"team_door-exit_door-trigger_glados_exit_door\", \"AddOutput\", \"OnStartTouch bts_wall_damaged:Enable\" );\n"\
    "        EntFire( \"team_door-exit_door-trigger_glados_exit_door\", \"AddOutput\", \"OnStartTouch bts_wall_areaportal:Open\" );\n"\
    "        EntFire( \"team_door-exit_door-trigger_glados_exit_door\", \"AddOutput\", \"OnStartTouch bts_wall_playerpusher:Disable::1.00\" );\n"\
    "\n"\
    "        // Make ending faster\n"\
    "        EntFire( \"both_players_in_rl\", \"Disable\" );\n"\
    "        EntFire( \"coopman_airlock_success\", \"AddOutput\", \"OnChangeToAllTrue team_trigger_walkway:Kill\" );\n"\
    "        EntFire( \"coopman_airlock_success\", \"AddOutput\", \"OnChangeToAllTrue walkway_gate_door:Open\" );\n"\
    "        EntFire( \"coopman_airlock_success\", \"AddOutput\", \"OnChangeToAllTrue InstanceAuto76-fade_return_hub:Fade\" );\n"\
    "        EntFire( \"coopman_airlock_success\", \"AddOutput\", \"OnChangeToAllTrue final_door_fade_to_black:Trigger::1.5\" );\n"\
    "    }\n"\
    "}\n"\
    "else if ( pszMapName == \"mp_coop_credits\" )\n"\
    "{\n"\
    "    AddCoopCreditsName( \"\" ); // First slot doesn't work apparently...\n"\
    "    AddCoopCreditsName( \"P2: Coop SRM by Nanoman2525\" );\n"\
    "    AddCoopCreditsName( \"Special thanks:\" );\n"\
    "    AddCoopCreditsName( \"Some really cool guy named Bumpy\" );\n"\
    "    AddCoopCreditsName( \"(P.S. he is green)\" );\n"\
    "    AddCoopCreditsName( \"That's about it lol\" );\n"\
    "    AddCoopCreditsName( \"Valve Credits:\" );\n"\
    "}\n"\
    "\n"\
    "delete pszMapName;\n"\
    "delete iTargetMapIndex;\n";