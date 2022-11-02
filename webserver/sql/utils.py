def convertData(data={}, types={}, nullable=[]):
    for k in data.keys():
        if k not in types.keys():
            raise Exception(f"Invalid parameter {k}")
    for k, v in types.items():
        if not data.get(k) and k in nullable:
            data[k] = "null"
        elif data.get(k):
            try:
                data[k] = v(data[k], k in nullable).data
            except ValueError:
                raise Exception(f"Invalid value for parameter {k}")
        else:
            raise Exception(f"Parameter {k} can't be empty")
