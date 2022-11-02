def insert(table, content):
    if len(content) == 0:
        return None
    s = f"INSERT INTO {table}"
    c = " ("
    i = " VALUES("
    for k, v in content.items():
        c += f"{k}, "
        i += f"{v}, "
    c = c[:-2] + ")"
    i = i[:-2] + ")"
    s += c + i
    s += " RETURNING *;"
    return s


def update(table, content, f={}):
    if len(content) == 0:
        return None
    s = f"UPDATE {table} SET"
    for k, v in content.items():
        s += f" {k} = {v},"
    s = s[:-1]
    if len(f) > 0:
        s += " WHERE"
        first = True
        for k, v in f.items():
            if not first:
                script += " AND"
            s += f" {k} = {v}"
            first = False
    s += " RETURNING *;"
    return s


def select(table, f={}):
    s = f"SELECT * FROM {table}"
    if len(f) > 0:
        s += " WHERE"
        first = True
        for k, v in f.items():
            if not first:
                s += " AND"
            s += f" {k} = {v}"
            first = False
    return s + ";"
