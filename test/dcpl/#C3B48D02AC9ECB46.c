
#define function int
#define unknown int

function _C3B48D02AC9ECB46()
{
    unknown var_0 = #is_final_build();
    if (!var_0)
    {
        unknown var_1 = _5445173390656D6D("DEB_WM: %2s VOX encounter start facts are set", "");
        unknown var_0 = display(var_1, 19);
    }
    else
    {
    }
    unknown var_3 = new_boxed_value(1, 1);
    unknown var_0 = fact_set(global, search_start_played, var_3);
    unknown var_3 = new_boxed_value(1, 1);
    unknown var_0 = fact_set(global, ambush_played, var_3);
    return var_0;
}