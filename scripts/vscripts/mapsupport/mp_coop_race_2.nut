// Speed up checkpoint airlock
EntFire( "entry_airlock-coopman_airlock_success", "AddOutput", "OnChangeToAllTrue entry_airlock-airlock_exit_door_open_rl:Trigger::0.3" );
EntFire( "entry_airlock-airlock_exit_door_open_rl", "AddOutput", "OnTrigger entry_airlock-airlock_exit_door_open_rl:Disable" );

// Speed up ending transition
EntFire( "rl_start_exit", "Disable" );
EntFire( "coopman_exit_level", "AddOutput", "OnChangeToAllTrue rl_start_exit_finished:Trigger" );