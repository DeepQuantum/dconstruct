(and
    (or
        (shambler-standing-explode-line-of-motion-check)
        (and
            (is-rogue-mode?)
            (is-player?)
            (or
                (player-is-prone?)
                (player-is-supine?)
            )
            (player-in-prone-hiding-region?)
        )
    )
    (> (melee-fact-get-time-since player shambler-explode) 5) 
    (> (melee-fact-get-time-since arg_2 time-since-in-finisher-fail) 2) 
)