
# GeoIP City Edition, Rev 1: CZ, 90, Zlinsky kraj, Roznov Pod Radhostem, 756 52, 49.460602, 18.136700, 0, 0
# GeoIP City Edition, Rev 1: GB, N/A, N/A, N/A, N/A, 51.496399, -0.122400, 0, 0
# GeoIP City Edition, Rev 1: UA, 07, Kharkivs'ka Oblast', Kharkiv, N/A, 49.980801, 36.252701, 0, 0

def process(line):
    parts = line.split(',')
    parts = [ x.strip() for x in parts ]
    assert parts[0] == "GeoIP City Edition"
    p = parts[1].split(' ')
    if len(p) != 3:
        return None
    rev, _, country = p

    assert len(parts) == 10
    d = {
        'whole' : line,
        'country' : country,
        'lat' : parts[6],
        'lon' : parts[7],
        'code' : parts[2],
        'addr' : ", ".join(parts[3:6]),
        # not sure what these are, US only?
        'x' : parts[8],
        'y' : parts[9],
    }
    return d

# FIN
