class BaseType:
    def __init__(self, data, isNullable):
        if data == None and isNullable:
            self.data = "null"
        else:
            self.data = self.convert(data)

    def convert(data):
        return data


class Integer(BaseType):
    def __init__(self, data, isNullable):
        super().__init__(data, isNullable)

    def convert(self, data):
        return int(data)


class Float(BaseType):
    def __init__(self, data, isNullable):
        super().__init__(data, isNullable)

    def convert(self, data):
        return float(data)


class String(BaseType):
    def __init__(self, data, isNullable):
        super().__init__(data, isNullable)

    def convert(self, data):
        return '"' + str(data) + '"'
