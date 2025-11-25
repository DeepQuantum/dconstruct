
(and
    (or 
        shambler-standing-explode-line-of-motion-check()
        (and 
            is-rogue-mode?() 
            is-player?()  
            (or
                player-is-prone?()  
                player-is-supine?() 
            )
            player-in-prone-hiding-region?()
        )
        (and 
            is-rogue-mode?()
            is-player?()
            npc-can-path-to-object?(arg_2, arg_3)
            (< distance-between-points(get-nav-destination(arg_2), get_object-position(arg_3)) 4.0)
        )
    )
    (and 
        (> melee-fact-get-time-since(player, shambler-explode) 5) 
        (> melee-fact-get-time-since(arg_2, time-since-in-finisher-fail) 2) 
    )
)