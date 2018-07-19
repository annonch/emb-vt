import ctypes

_vtclock = ctypes.CDLL('./libvtclock.so')
_vtclock.getTime.argtype = (ctypes.c_int)

def getTime(type_of_clock):
    """
    Arg:
    type_of_clock...int
        case '1': CLOCK_REALTIME;
        case '2': CLOCK_MONOTONIC;
        case '3': CLOCK_MONOTONIC_COARSE;
        case '4': CLOCK_MONOTONIC_RAW;
        case '5': CLOCK_BOOTTIME;
        default: CLOCK_REALTIME;
    
    Return:
    String e.g. '1521588964.827307889' in nano sec
    """
    global _vtclock
    se = ctypes.c_int(0)
    na = ctypes.c_int(0)
    _vtclock.getTime(ctypes.c_int(type_of_clock), ctypes.byref(se), ctypes.byref(na))

    return str(se.value) + '.' + str(na.value)